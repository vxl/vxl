#include "breg3d_ekf_camera_optimizer.h"

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_exp.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>

#include <vpgl/vpgl_perspective_camera.h>

// for computing homography
#include <ihog/ihog_transform_2d.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_minimizer.h>
#include <brip/brip_vil_float_ops.h>


#include "breg3d_ekf_camera_optimizer_state.h"
#include "breg3d_homography_generator.h"
#include "breg3d_lm_direct_homography_generator.h"
#include "breg3d_gdbicp_homography_generator.h"


breg3d_ekf_camera_optimizer::breg3d_ekf_camera_optimizer(double pos_var_predict, double rot_var_predict,
                                                         double pos_var_measure, double rot_var_measure,
                                                         double homography_var, double homography_var_t,
                                                         bool use_gps, bool use_expected, bool use_proj_homography)
  : use_gps_(use_gps), use_proj_homography_(use_proj_homography), use_expected_(use_expected)
{
  // fill in prediction error covariance matrix
  prediction_error_covar_ = vnl_matrix<double>(6,6,0.0);
  for (unsigned i=0; i<3; ++i)
    prediction_error_covar_(i,i) = pos_var_predict;
  for (unsigned i=3; i<6; ++i)
    prediction_error_covar_(i,i) = rot_var_predict;

  // fill in measurement error covariance matrix
  unsigned nh = 6, ngps = 0;
  if (use_proj_homography)
    nh =8;
  if (use_gps)
    ngps = 6;
  unsigned matrix_size = nh + ngps;

  measurement_error_covar_ = vnl_matrix<double>(matrix_size,matrix_size,0.0);
  for (unsigned i=0; i<2; ++i)
    measurement_error_covar_(i,i) = homography_var_t;
  for (unsigned i=2; i<nh; ++i)
    measurement_error_covar_(i,i) = homography_var;
  if (use_gps) {
    for (unsigned i=nh; i<nh+3; ++i)
      measurement_error_covar_(i,i) = pos_var_measure;
    for (unsigned i=nh+3; i<matrix_size; ++i)
      measurement_error_covar_(i,i) = rot_var_measure;
  }

  homography_gen_ = new breg3d_lm_direct_homography_generator();
  //homography_gen_ = new breg3d_gdbicp_homography_generator();
}

breg3d_ekf_camera_optimizer::~breg3d_ekf_camera_optimizer()
{
  if (homography_gen_)
    delete homography_gen_;
}

breg3d_ekf_camera_optimizer_state breg3d_ekf_camera_optimizer::optimize(bvxm_voxel_world_sptr const& vox_world,
                                                                        vil_image_view_base_sptr &prev_img,
                                                                        bvxm_image_metadata &curr_img,
                                                                        breg3d_ekf_camera_optimizer_state &prev_state,
                                                                        const std::string& apm_type, unsigned bin_idx)
{
  breg3d_ekf_camera_optimizer_state state_og = prev_state;
  auto* cam_est =
    dynamic_cast<vpgl_perspective_camera<double>*>(curr_img.camera.ptr());

  if (!cam_est) {
    std::cerr << "error: current camera estimate must be a vpgl_perspective_camera with at least the calibration matrix set.\n";
    return prev_state;
  }

  // create mask of all ones for previous image
  vil_image_view<float> mask(prev_img->ni(),prev_img->nj(),1);
  mask.fill(1.0f);

  // perform the first iteration, using gps as aditional observation if user requests
  breg3d_ekf_camera_optimizer_state step_state =
    optimize_once(vox_world,prev_img,mask,curr_img,prev_state, use_gps_);

  // debug
  std::vector<vpgl_perspective_camera<double> > step_vec;
  step_vec.emplace_back(cam_est->get_calibration(),state_og.get_point(),state_og.get_rotation());
  step_vec.emplace_back(cam_est->get_calibration(),step_state.get_point(),step_state.get_rotation());

  std::cout << "Pk =\n" << step_state.get_error_covariance() << std::endl;

  // iteratively update estimate, using expected images at intermediate steps as observations
  if (use_expected_) {
    breg3d_ekf_camera_optimizer substep_optimizer(1.0,1.0,0.0,0.0,0.0,0,false,false,false);
    breg3d_ekf_camera_optimizer_state substep_state(step_state);
    substep_state.set_state(vnl_vector_fixed<double,6>(0.0));
    unsigned max_iterations = 10;
    double min_step_length = 0.00075;
    unsigned nits = 0;
    bool iterate_again = true;
    while (iterate_again) {
      if (nits++ > max_iterations)
        break;
      // generate expected image to use as observation
      vpgl_camera_double_sptr step_cam =
        new vpgl_perspective_camera<double>(cam_est->get_calibration(),substep_state.get_point(),substep_state.get_rotation());
      // fill in metadata
      bvxm_image_metadata step_meta(vil_image_view_base_sptr(nullptr),step_cam);
      // allocate expected images
      vil_image_view_base_sptr step_expected =
        new vil_image_view<unsigned char>(curr_img.img->ni(),curr_img.img->nj(),curr_img.img->nplanes());
      vil_image_view<float> step_mask(step_expected->ni(),step_expected->nj(),1);

      // generate expected image
      if (apm_type == "apm_mog_grey") {
        vox_world->expected_image<APM_MOG_GREY>(step_meta,step_expected,step_mask,bin_idx);
      }
      else if (apm_type == "apm_mog_rgb") {
        vox_world->expected_image<APM_MOG_RGB>(step_meta,step_expected,step_mask,bin_idx);
      }
      else {
        std::cerr << "error: unsupported appearance model type " << apm_type << " !\n";
      }
      // optimize - do not use gps estimate (no new gps estimate for these sub-steps)
      vil_save(*step_expected,"C:/research/registration/output/step_expected.tiff");
      vil_save(step_mask,"C:/research/registration/output/step_expected_mask.tiff");
      substep_state = substep_optimizer.optimize_once(vox_world,step_expected,step_mask,curr_img,substep_state,false);

      step_vec.emplace_back(cam_est->get_calibration(),substep_state.get_point(),substep_state.get_rotation());

      double step_length = substep_state.get_state().magnitude();
      std::cout << " step length = " << step_length << '\n'
               << "Pk =\n" << substep_state.get_error_covariance() << std::endl;
      if (step_length < min_step_length)
        iterate_again = false;
    }
    // figure out total step distance
    vnl_matrix_fixed<double,3,3> dR =
      prev_state.get_rotation().inverse().as_matrix()*substep_state.get_rotation().as_matrix();

    vnl_matrix_fixed<double,3,1> dC;
    dC(0,0) = prev_state.get_point().x() - substep_state.get_point().x();
    dC(1,0) = prev_state.get_point().y() - substep_state.get_point().y();
    dC(2,0) = prev_state.get_point().z() - substep_state.get_point().z();

    vnl_matrix_fixed<double,3,1> dT = substep_state.get_rotation().as_matrix()*(dC);
    vnl_matrix_fixed<double,4,4> dE; dE.set_identity().update(dR,0,0).update(dT,0,3);

    vnl_vector_fixed<double,6> total_step = matrix_to_coeffs_SE3(dE);
    // scale down translation coefficients
    total_step[0] *= step_state.t_scale();
    total_step[1] *= step_state.t_scale();
    total_step[2] *= step_state.t_scale();

    step_state.set_state(total_step);
    //step_state.set_error_covariance(substep_state.get_error_covariance());
    step_state.set_base_point(substep_state.get_point());
    step_state.set_base_rotation(substep_state.get_rotation());
  }
  // debug
  for (unsigned i=0; i<step_vec.size(); ++i) {
    std::cout << "step " << i << '\n'
             << "center = " << step_vec[i].get_camera_center() << '\n'
             << "rot = " << step_vec[i].get_rotation().as_rodrigues() << std::endl;
  }
  return step_state;
}


breg3d_ekf_camera_optimizer_state breg3d_ekf_camera_optimizer::optimize_once(bvxm_voxel_world_sptr const& vox_world,
                                                                             vil_image_view_base_sptr &prev_img, vil_image_view<float> &prev_mask,
                                                                             bvxm_image_metadata &curr_img, breg3d_ekf_camera_optimizer_state &prev_state,
                                                                             bool use_gps)
{
  // get the transformation from world coordinates to the previous camera coordinates
  vnl_matrix_fixed<double,3,3> R_prev = prev_state.get_rotation().as_matrix();
  vgl_point_3d<double> center_prev = prev_state.get_point();
  double t_scale = prev_state.t_scale();

  // Project the state ahead
  // assume constant velocity
  vnl_vector_fixed<double,6> x_pred = prev_state.get_state();
  //vnl_vector_fixed<double,6> x_pred(0.0);

  // Project the error covariance ahead
  vnl_matrix_fixed<double,6,6> P_pred = prev_state.get_error_covariance() + prediction_error_covar_;
  // vnl_matrix_fixed<double,6,6> P_pred = prediction_error_covar_;

  // get a planar approximation of the world
  vgl_plane_3d<double> world_plane = vox_world->fit_plane();
  // transform the plane to camera coordinates
  vnl_vector_fixed<double,3> plane_normal(world_plane.normal().x(),world_plane.normal().y(),world_plane.normal().z());
  vnl_vector_fixed<double,3> center_prev_v(center_prev.x(),center_prev.y(),center_prev.z());
  vnl_vector_fixed<double,3> plane_normal_cam = R_prev * plane_normal;
  double dist_cam = dot_product(center_prev_v,plane_normal) + world_plane.d();
  vgl_plane_3d<double> world_plane_cam(plane_normal_cam(0),plane_normal_cam(1),plane_normal_cam(2),dist_cam);
  // compute plane parameters theta,phi,and dz
  double dz = -world_plane_cam.d() / world_plane_cam.c();
  //double theta = std::acos(-world_plane_cam.nz()/sqrt(world_plane_cam.nx()*world_plane_cam.nx() + world_plane_cam.nz()*world_plane_cam.nz()));
  double theta = std::atan2(world_plane_cam.nx(),-world_plane_cam.nz());
  //double phi = std::acos(-world_plane_cam.nz()/sqrt(world_plane_cam.ny()*world_plane_cam.ny() + world_plane_cam.nz()*world_plane_cam.nz()));
  double phi = std::atan2(world_plane_cam.ny(),-world_plane_cam.nz());

  std::cout << "dz = " << dz << '\n'
           << "theta = " << theta << '\n'
           << "phi = " << phi << '\n' << std::endl;

  // construct the measurement Jacobian
  unsigned nhomography = 6 + (use_proj_homography_? 2:0);
  unsigned nmeasurements = nhomography + (use_gps? 6:0);
  vnl_matrix<double> H(nmeasurements,6,0.0);

  vnl_matrix<double> J = SE3_to_H_Jacobian(theta,phi,dz*t_scale);
  H.update(J,0,0);

  if (use_gps) {
    H.update(vnl_matrix<double>(6,6).set_identity(),nhomography,0);
  }

  // Compute the Kalman Gain
  vnl_matrix<double> H_trans = H.transpose();
  vnl_matrix<double> K = P_pred*H_trans*vnl_matrix_inverse<double>(H*P_pred*H_trans + measurement_error_covar_);

  std::cout << "H = " << H << '\n'
           << "measurement_error_covar = " << measurement_error_covar_ << std::endl;

  // predict measurement vector z
  vnl_vector<double> z_pred(nmeasurements);

  // use J to generate predicted homography
  vnl_vector<double> zh_pred = J*x_pred.as_vector();
  z_pred.update(zh_pred,0);
  if (use_gps) {
    // predicted gps estimate is simply x_pred
    z_pred.update(x_pred, nhomography);
  }

  // compute homography to get measurement vector
  vpgl_perspective_camera<double>  *curr_cam_perspective;
  curr_cam_perspective = dynamic_cast<vpgl_perspective_camera<double>*>(curr_img.camera.ptr());
  if (!curr_cam_perspective) {
    std::cerr << "ERROR camera_optimizer expects vpgl_perspective cameras.\n";
    return prev_state;
  }

  vnl_vector<double> zh = this->img_homography(curr_img.img,prev_img,prev_mask,
                                               curr_cam_perspective->get_calibration().get_matrix(),
                                               use_proj_homography_);
  vnl_vector<double> z(nmeasurements);
  z.update(zh,0);

  // add gps measurement to vector
  if (use_gps) {
    vnl_matrix<double> invTprev(4,4,0.0);
    invTprev.update(R_prev.transpose(),0,0);
    invTprev(0,3) = center_prev.x();
    invTprev(1,3) = center_prev.y();
    invTprev(2,3) = center_prev.z();
    invTprev(3,3) = 1.0;
    vnl_matrix<double> Tgps(4,4,0.0);
    Tgps.update(curr_cam_perspective->get_rotation().as_matrix(),0,0);
    vnl_matrix_fixed<double,3,1> Cgps;
    Cgps(0,0) = curr_cam_perspective->camera_center().x();
    Cgps(1,0) = curr_cam_perspective->camera_center().y();
    Cgps(2,0) = curr_cam_perspective->camera_center().z();

    Tgps.update(-curr_cam_perspective->get_rotation().as_matrix()*Cgps,0,3);
    Tgps(3,3) = 1.0;

    vnl_matrix<double> dE = Tgps*invTprev;
    vnl_vector_fixed<double,6> zgps_raw = matrix_to_coeffs_SE3(dE);

    z.update(zgps_raw.extract(3,0)*t_scale, nhomography);
    z.update(zgps_raw.extract(3,3), nhomography + 3);
  }

  // Update estimate with measurement zk
  vnl_vector_fixed<double,6> x_post = x_pred + K*(z - z_pred);

  std::cout << "K = " << K << '\n'
           << "z_pred = " << z_pred << '\n'
           << "z      = " << z << '\n'
           << "x_pred = " << x_pred << '\n'
           << "x_post = " << x_post << std::endl;


  // Update error covariance
  vnl_matrix_fixed<double,6,6> P_post = (vnl_matrix<double>(6,6).set_identity() - K*H)*P_pred;

  std::cout << "P_pred = " << P_pred << '\n'
           << "P_post = " << P_post << std::endl;

  // update camera
  vnl_vector_fixed<double,6> x_post_unscaled;
  x_post_unscaled.update(x_post.extract(3,0)/t_scale, 0);
  x_post_unscaled.update(x_post.extract(3,3), 3);
  vnl_matrix_fixed<double,4,4> dRT = coeffs_to_matrix_SE3(x_post_unscaled);
  vnl_matrix_fixed<double,3,1> C_prev;
  C_prev(0,0) = center_prev.x();
  C_prev(1,0) = center_prev.y();
  C_prev(2,0) = center_prev.z();
  vnl_matrix_fixed<double,3,1> T_prev = -R_prev*C_prev;
  vnl_matrix_fixed<double,3,3> Rnew = dRT.extract(3,3)*R_prev;
  vnl_matrix_fixed<double,3,1> Tnew = dRT.extract(3,3,0,0)*T_prev + dRT.extract(3,1,0,3);
  vnl_matrix_fixed<double,3,1> Cnew = -Rnew.transpose()*Tnew;

  vgl_point_3d<double> curr_center(Cnew(0,0),Cnew(1,0),Cnew(2,0));
  vgl_rotation_3d<double> curr_rot(Rnew);

  curr_cam_perspective->set_camera_center(curr_center);
  curr_cam_perspective->set_rotation(curr_rot);

  // create new state
  breg3d_ekf_camera_optimizer_state curr_state(prev_state.k()+1,prev_state.t_scale(),
                                               curr_center, curr_rot, x_post, P_post);

  return curr_state;
}


vnl_vector<double> breg3d_ekf_camera_optimizer::img_homography(vil_image_view_base_sptr &base_img_viewb, vil_image_view_base_sptr &img_viewb, vil_image_view<float> &mask_view, vnl_matrix_fixed<double,3,3> K, bool projective)
{
  vil_image_view<float> base_img_view, img_view;

  switch (base_img_viewb->pixel_format())
  {
   case VIL_PIXEL_FORMAT_RGB_BYTE: {
    auto *img_rgb_byte = dynamic_cast<vil_image_view<vil_rgb<vxl_byte> >*>(base_img_viewb.ptr());
    vil_convert_rgb_to_grey(*img_rgb_byte,base_img_view);
    break;
   }
   case VIL_PIXEL_FORMAT_BYTE: {
    auto *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(base_img_viewb.ptr());
    vil_convert_stretch_range_limited(*img_byte,base_img_view,(vxl_byte)0,(vxl_byte)255,0.0f,1.0f);
    break;
   }
   default:
    std::cerr << "error: breg3d_ekf_camera_optimizer::img_homography : unsupported pixel type " << base_img_viewb->pixel_format() << '\n';
  }

  switch (img_viewb->pixel_format())
  {
   case VIL_PIXEL_FORMAT_RGB_BYTE: {
    auto *img_rgb_byte = dynamic_cast<vil_image_view<vil_rgb<vxl_byte> >*>(img_viewb.ptr());
    vil_convert_rgb_to_grey(*img_rgb_byte,base_img_view);
    break;
   }
   case VIL_PIXEL_FORMAT_BYTE: {
    auto *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(img_viewb.ptr());
    vil_convert_stretch_range_limited(*img_byte,img_view,(vxl_byte)0,(vxl_byte)255,0.0f,1.0f);
    break;
   }
   default:
    std::cerr << "error: breg3d_ekf_camera_optimizer::img_homography : unsupported pixel type " << img_viewb->pixel_format() << '\n';
  }

  // computed homography maps pixels in current image to pixels in base image
  homography_gen_->set_image0(&base_img_view);
  homography_gen_->set_image1(&img_view);
  homography_gen_->set_mask1(&mask_view);
  homography_gen_->set_projective(use_proj_homography_);
  ihog_transform_2d xform = homography_gen_->compute_homography();

  // convert to normalized camera matrix
  vnl_matrix<double> invK = vnl_matrix_inverse<double>(K);
  vnl_matrix<double> H = invK * xform.inverse().get_matrix() * K;

  // now extract Lie Generator coefficients
  vnl_vector<double> lie_vector;
  if (projective)
    lie_vector = matrix_to_coeffs_P2(H);
  else
    lie_vector = matrix_to_coeffs_GA2(H);

  std::cout << "optimized homography =\n" << xform.inverse().get_matrix() << '\n'
           << "normalized homography =\n" << H << '\n'
           << "homography lie coeffs = " << lie_vector << '\n' << std::endl;

  return lie_vector;
}


vnl_vector_fixed<double,6> breg3d_ekf_camera_optimizer::matrix_to_coeffs_SE3(vnl_matrix_fixed<double,4,4> const& M)
{
  vnl_matrix<double> logM(4,4);
  vnl_vector_fixed<double,6> coeffs;

  if (!logm_approx(M,logM)) {
    std::cerr << "error converting matrix to Lie coefficients.  matrix could be too far from Identity.\n";
    coeffs.fill(0.0);
    return coeffs;
  }

  coeffs(0) = logM(0,3);
  coeffs(1) = logM(1,3);
  coeffs(2) = logM(2,3);
  coeffs(3) = logM(2,1);
  coeffs(4) = logM(0,2);
  coeffs(5) = logM(1,0);

  return coeffs;
}

vnl_vector_fixed<double,6> breg3d_ekf_camera_optimizer::matrix_to_coeffs_GA2(vnl_matrix_fixed<double,3,3> const& M)
{
  vnl_matrix<double> logM(3,3);
  vnl_vector_fixed<double,6> coeffs;

  if (!logm_approx(M,logM)) {
    std::cerr << "error converting matrix to Lie coefficients.  matrix could be too far from Identity.\n";
    coeffs.fill(0.0);
    return coeffs;
  }

  std::cout << "M = " << M << '\n'
           << "logM = " << logM << std::endl;

  coeffs(0) = logM(0,2);
  coeffs(1) = logM(1,2);
  coeffs(2) = (-logM(0,1) + logM(1,0))/2.0;
  coeffs(3) = ( logM(0,0) + logM(1,1))/2.0;
  coeffs(4) = ( logM(0,0) - logM(1,1))/2.0;
  coeffs(5) = ( logM(0,1) + logM(1,0))/2.0;


  return coeffs;
}

vnl_vector_fixed<double,8> breg3d_ekf_camera_optimizer::matrix_to_coeffs_P2(vnl_matrix_fixed<double,3,3> const& M)
{
  vnl_matrix<double> logM(3,3);
  vnl_vector_fixed<double,8> coeffs;

  if (!logm_approx(M,logM)) {
    std::cerr << "error converting matrix to Lie coefficients.  matrix could be too far from Identity.\n";
    coeffs.fill(0.0);
    return coeffs;
  }

  coeffs(0) = logM(0,2);
  coeffs(1) = logM(1,2);
  coeffs(2) = (-logM(0,1) + logM(1,0))/2.0;
  coeffs(3) = ( logM(0,0) + logM(1,1))/2.0;
  coeffs(4) = ( logM(0,0) - logM(1,1))/2.0;
  coeffs(5) = ( logM(0,1) + logM(1,0))/2.0;

  // not sure if these are right -DC
  coeffs(6) = logM(2,0);
  coeffs(7) = logM(2,1);

  return coeffs;
}

vnl_matrix_fixed<double,4,4> breg3d_ekf_camera_optimizer::coeffs_to_matrix_SE3(vnl_vector_fixed<double,6> const& a)
{
  vnl_matrix_fixed<double,4,4> A(0.0);
  // create Lie Algebra element A = |r t| where r is 3x3 skew-symmetric and t is a 3x1 vector.
  //                                |0 0|

  A(0,3) = a(0);
  A(1,3) = a(1);
  A(2,3) = a(2);
  A(1,2) = -a(3); A(2,1) =  a(3);
  A(0,2) =  a(4); A(2,0) = -a(4);
  A(0,1) = -a(5); A(1,0) =  a(5);

  vnl_matrix_fixed<double,4,4> M = vnl_matrix_exp< vnl_matrix_fixed<double,4,4> >(A);

  return M;
}

vnl_matrix_fixed<double,3,3> breg3d_ekf_camera_optimizer::coeffs_to_matrix_GA2(vnl_vector_fixed<double,6> const& /*a*/)
{
  std::cerr << "breg3d_ekf_camera_optimizer::coeffs_to_matrix_GA2 not implemented yet\n";
  return vnl_matrix_fixed<double,3,3>(0.0);
}

vnl_matrix_fixed<double,3,3> breg3d_ekf_camera_optimizer::coeffs_to_matrix_P2(vnl_vector_fixed<double,8> const& /*a*/)
{
  std::cerr << "breg3d_ekf_camera_optimizer::coeffs_to_matrix_P2 not implemented yet\n";
  return vnl_matrix_fixed<double,3,3>(0.0);
}


bool breg3d_ekf_camera_optimizer::logm_approx(vnl_matrix<double> const& A, vnl_matrix<double> &logA, double tol)
{
  unsigned max_iterations = 1000;

  unsigned nr = A.rows();
  unsigned nc = A.cols();
  if (nr != nc) {
    std::cerr << "error: logm_approx called with non-square matrix.\n";
    return false;
  }
  logA.set_size(nr,nr);
  logA.fill(0.0);

  vnl_matrix<double> I(nr,nr);
  I.set_identity();

  vnl_matrix<double> W = I - A;
  double term_norm = tol + 1.0;

  unsigned i = 1;
  vnl_matrix<double> Wpow = I;
  while (term_norm > tol) {
    if (i >= max_iterations) {
      std::cerr << '\n'
               << "*************************************************************\n"
               << "ERROR: logm_approx did not converge.\n"
               << "*************************************************************\n\n";
      return false;
    }
    Wpow = Wpow*W;
    vnl_matrix<double> term = -Wpow/i;
    term_norm = term.frobenius_norm();
    logA += term;
    //std::cout << "iteration " << i <<": W = " << W << '\n' << "Wpow = " << Wpow << '\n'
    //         << "term = " << term << '\n' << "logA = " << logA << std::endl;
    ++i;
  }
  std::cout << "logM converged in " << i << " iterations." << std::endl;

  return true;
}

vnl_matrix<double> breg3d_ekf_camera_optimizer::SE3_to_H_Jacobian(double plane_theta, double plane_phi, double plane_dz)
{
  vnl_matrix<double> J;

  if (use_proj_homography_)
    J.set_size(8,6);
  else
    J.set_size(6,6);

  J.fill(0.0);

  double inv_dz = 1.0/plane_dz;
  double t_phi = std::tan(plane_phi) / (2*plane_dz);
  double t_theta = std::tan(plane_theta) / (2*plane_dz);

  J(0,0) = inv_dz;                                                       J(0,4) = 1.0;
                     J(1,1) = inv_dz;                     J(1,3) = -1.0;
  J(2,0) = t_phi;    J(2,1) = -t_theta;                                                J(2,5) = 1.0;
  J(3,0) = -t_theta; J(3,1) = -t_phi;    J(3,2) = -inv_dz;
  J(4,0) = -t_theta; J(4,1) = t_phi;
  J(5,0) = -t_phi;   J(5,1) = -t_theta;
  if (use_proj_homography_) {
    J(6,2) = t_phi*2;    J(6,3) = -1.0;
    J(7,2) = t_theta*2;  J(7,4) = 1.0;
  }

  return J;
}

vnl_matrix<double> breg3d_ekf_camera_optimizer::H_to_SE3_Jacobian(double plane_theta, double plane_phi, double plane_dz)
{
  vnl_matrix<double> J = SE3_to_H_Jacobian(plane_theta,plane_phi,plane_dz);
  vnl_matrix<double> invJ = vnl_matrix_inverse<double>(J);

  return invJ;
}
