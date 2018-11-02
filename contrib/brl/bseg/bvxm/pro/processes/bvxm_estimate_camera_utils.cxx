#include "bvxm_estimate_camera_utils.h"
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vnl/vnl_double_3.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Default constructor
bvxm_camera_estimator::bvxm_camera_estimator()
{
  world_params_set_ = false;
  estimation_params_set_ = false;
}

void bvxm_camera_estimator::set_world_params(int nx, int ny, int nz, double sx, double sy, double sz, double dx, double dy, double dz)
{
  nx_ = nx;
  ny_ = ny;
  nz_ = nz;
  sx_ = sx;
  sy_ = sy;
  sz_ = sz;
  dx_ = dx;
  dy_ = dy;
  dz_ = dz;
  world_params_set_ = true;
}

void bvxm_camera_estimator::set_estimation_params(double theta_range, double theta_step, double phi_range, double phi_step, double rot_range, double rot_step, int max_iter_rot_angle)
{
  theta_range_ = theta_range;
  theta_step_ = theta_step;
  phi_range_ = phi_range;
  phi_step_ = phi_step;
  rot_range_ = rot_range;
  rot_step_ = rot_step;
  max_iter_rot_angle_ = max_iter_rot_angle;
  estimation_params_set_ = true;
}

vil_image_view<float> bvxm_camera_estimator::convert_to_spherical_coordinates(const vil_image_view<float> &img,
                                                                              const vpgl_perspective_camera<double> &cam,
                                                                              const double rotate)
{
  int theta_size = 2*vnl_math::ceil(theta_range_/theta_step_);
  int phi_size = 2*vnl_math::ceil(phi_range_/phi_step_);

  vil_image_view<float> imgs(theta_size,phi_size,1);
  imgs.fill(0.0f);

  vnl_double_3x3 K = cam.get_calibration().get_matrix();
  // vnl_double_3x3 K_inv = vnl_inverse<double>(K); // unused?! - FIXME

  vnl_matrix<double> R(3,3,0.0);
  R(0,0) = 1.0;
  R(1,1) = std::cos(rotate);
  R(1,2) = std::sin(rotate);
  R(2,1) = -std::sin(rotate);
  R(2,2) = std::cos(rotate);

  for (unsigned i=0; i<imgs.ni(); i++) {
    for (unsigned j=0; j<imgs.nj(); j++) {
      double curr_theta = (0.5*vnl_math::pi) - theta_range_ + (theta_step_*(double)i);
      double curr_phi = -phi_range_ + (phi_step_*(double)j);

      double x = std::sin(curr_theta)*std::cos(curr_phi);
      double y = std::sin(curr_theta)*std::sin(curr_phi);
      double z = std::cos(curr_theta);

      vnl_double_3 curr_vector(x,y,z);
      curr_vector = R*curr_vector;

      x = curr_vector[0];
      y = curr_vector[1];
      z = curr_vector[2];

      curr_vector[0] = -z;
      curr_vector[1] = y;
      curr_vector[2] = x;
      vnl_double_3 curr_pixel = K*curr_vector;

      int u = vnl_math::rnd(curr_pixel[0]/curr_pixel[2]);
      int v = vnl_math::rnd(curr_pixel[1]/curr_pixel[2]);

      if (img.in_range(u,v)) {
        imgs(i,j) = img(u,v);
      }
    }
  }

  return imgs;
}

std::vector<vgl_point_3d<double> > bvxm_camera_estimator::convert_3d_box_to_3d_points(const vgl_box_3d<double> box_3d)
{
  double box_x[2],box_y[2],box_z[2];
  box_x[0] = box_3d.min_x();
  box_y[0] = box_3d.min_y();
  box_z[0] = box_3d.min_z();
  box_x[1] = box_3d.max_x();
  box_y[1] = box_3d.max_y();
  box_z[1] = box_3d.max_z();

  std::vector<vgl_point_3d<double> > box_pts_3d;
  for (double & i : box_x) {
    for (double & j : box_y) {
      for (double & k : box_z) {
        box_pts_3d.emplace_back(i,j,k);
      }
    }
  }

  return box_pts_3d;
}

vgl_polygon<double> bvxm_camera_estimator::convert_3d_box_to_2d_polygon(const vgl_box_3d<double> box_3d,
                                                                        const vpgl_perspective_camera<double> *cam)
{
  std::vector<vgl_point_3d<double> > box_pts_3d = convert_3d_box_to_3d_points(box_3d);

  std::vector<vgl_point_2d<double> > box_pts_2d;
  for (auto & i : box_pts_3d) {
    double u,v;
    cam->project(i.x(),i.y(),i.z(),u,v);
    vgl_point_2d<double> curr_pt_2d(u,v);
    box_pts_2d.push_back(curr_pt_2d);
  }

  vgl_convex_hull_2d<double> ch(box_pts_2d);
  vgl_polygon<double> polygon_2d = ch.hull();

  return polygon_2d;
}

void bvxm_camera_estimator::convert_angles_to_vector(const double theta,
                                                     const double phi,
                                                     double &vx,
                                                     double &vy,
                                                     double &vz)
{
  vx = std::sin(theta)*std::cos(phi);
  vy = std::sin(theta)*std::sin(phi);
  vz = std::cos(theta);
}

double bvxm_camera_estimator::edge_prob_cross_correlation(const vil_image_view<float> &img1, const vil_image_view<float> &img2)
{
  vil_image_view<float> img1n; img1n.deep_copy(img1);
  vil_image_view<float> img2n; img2n.deep_copy(img2);

  vil_math_normalise<float>(img1n);
  vil_math_normalise<float>(img2n);

  vil_image_view<float> img_product;
  vil_math_image_product(img1n,img2n,img_product);

  double img_product_sum;
  vil_math_sum<float,double>(img_product_sum,img_product,0);

  return img_product_sum;
}

void bvxm_camera_estimator::get_expected_edge_image(const bvxm_voxel_slab<float> &data,
                                                    const vpgl_perspective_camera<double> *cam,
                                                    vil_image_view<float> *img_eei)
{
  img_eei->fill(0.0f);

  int ni = img_eei->ni();
  int nj = img_eei->nj();

  // Layer version of expected edge image
  vgl_box_3d<double> box_3d(sx_,sy_,(sz_+((double)nz_)*dz_),(sx_+((double)nx_)*dx_),(sy_+((double)ny_)*dy_),sz_);
  vgl_polygon<double> poly_2d = convert_3d_box_to_2d_polygon(box_3d,cam);

  vgl_homg_plane_3d<double> plane_0(0.0,0.0,1.0,-sz_);
  vgl_homg_plane_3d<double> plane_1(0.0,0.0,1.0,-sz_-dz_);

  for (int i=0; i<ni; i++) {
    for (int j=0; j<nj; j++) {
      if (poly_2d.contains(double(i),(double)j)) {
        // following line 640 milliseconds
        vgl_line_3d_2_points<double> bp = cam->backproject(vgl_point_2d<double>((double)i,(double)j));
        vgl_homg_line_3d_2_points<double> bp_h(vgl_homg_point_3d<double>(bp.point1()),vgl_homg_point_3d<double>(bp.point2()));
        vgl_point_3d<double> pt_0(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_0));
        vgl_point_3d<double> pt_1(vgl_homg_operators_3d<double>::intersect_line_and_plane(bp_h,plane_1));

        vgl_vector_3d<double> diff = pt_1 - pt_0;

        double start_x = pt_0.x();
        double start_y = pt_0.y();
        //double start_z = pt_0.z();

        double diff_x = diff.x();
        double diff_y = diff.y();
        //double diff_z = diff.z();

        for (int vz=0; vz<nz_; vz++) {
          int vx = (int)((((start_x + (diff_x*vz)) - sx_)/dx_) + 0.5f);
          if (vx<0 || vx>=nx_) {
            continue;
          }

          int vy = (int)((((start_y + (diff_y*vz)) - sy_)/dy_) + 0.5f);
          if (vy<0 || vy>=ny_) {
            continue;
          }

          // this line takes 2200 milliseconds
          // with float ***data it takes 1100 ms
          (*img_eei)(i,j) = std::max((*img_eei)(i,j),data(vx,vy,vz));
        }
      }
    }
  }
}

double bvxm_camera_estimator::estimate_rotation_angle(const vil_image_view<float> &img1c,
                                                      const vil_image_view<float> &img2c)
{
  assert(img1c.ni()==img2c.ni());
  assert(img1c.nj()==img2c.nj());

  double center_x = 0.5*(double)img1c.ni();
  double center_y = 0.5*(double)img1c.nj();

  vil_image_view<float> img1;
  normalize_to_interval(img1c,img1,0.0f,1.0f);
  vil_image_view<float> img2;
  normalize_to_interval(img2c,img2,0.0f,1.0f);

  int rot_size = vnl_math::ceil(rot_range_/rot_step_);

  double best_score = 0.0;
  double best_rot = 0.0;

  for (int r=-rot_size; r<rot_size+1; r++) {
    double curr_rot = rot_step_*(double)r;

    vgl_h_matrix_2d<double> H;
    vgl_h_matrix_2d<double> H_temp;

    H.set_identity().set_translation(-center_x,-center_y);

    H_temp.set_identity().set_rotation(curr_rot);
    H = H_temp*H;

    H_temp.set_identity().set_translation(center_x,center_y);
    H = H_temp*H;

    vil_image_view<float> img2_rot(img2.ni(),img2.nj());

    brip_vil_float_ops::homography(img2, H, img2_rot,true,0.0f);

    double curr_score = edge_prob_cross_correlation(img1,img2_rot);

    if (curr_score > best_score) {
      best_score = curr_score;
      best_rot = curr_rot;
    }
  }

  return best_rot;
}

void bvxm_camera_estimator::estimate_rotation_iterative(const bvxm_voxel_slab<float> &data,
                                                        const vil_image_view<float>& img_e,
                                                        vpgl_perspective_camera<double> *cam)
{
  if ((!estimation_params_set_) || (!world_params_set_)) {
    std::cerr << "Error: world and estimation parameters are not set\n";
    return;
  }

  int ni = img_e.ni();
  int nj = img_e.nj();

  for (int iter=0; iter<max_iter_rot_angle_; iter++) {
    std::cout << '.';
    vil_image_view<float> img_eei(ni,nj,1);
    vil_image_view<vxl_byte> img_temp(ni,nj,1);

    get_expected_edge_image(data,cam,&img_eei);

    vil_image_view<float> img_eei_s = convert_to_spherical_coordinates(img_eei,*cam,0.0);
    vil_image_view<float> img_e_s = convert_to_spherical_coordinates(img_e,*cam,0.0);

    int offset_x, offset_y;
    float offset_score;
    estimate_offsets_fd<float,float,float>(img_e_s,img_eei_s,offset_x,offset_y,offset_score);

    if (offset_x==0 && offset_y==0) {
      break;
    }

    double theta = (0.5*vnl_math::pi) + (theta_step_*(double)offset_x);
    double phi = (phi_step_*(double)offset_y);

    double vx,vy,vz;
    bvxm_camera_estimator::convert_angles_to_vector(theta,phi,vx,vy,vz);

    vgl_vector_3d<double> vec_from(0.0,0.0,1.0);
    vgl_vector_3d<double> vec_to(-vz,vy,vx);

    vgl_rotation_3d<double> rot_offsets(vec_from,vec_to);

    cam->set_rotation(rot_offsets*cam->get_rotation());

    // Estimating the rotation angle around the axis

    get_expected_edge_image(data,cam,&img_eei);

    double rot_ang = estimate_rotation_angle(img_e,img_eei);

    if (rot_ang==0.0) {
      break;
    }

    vnl_vector<double> rot_rot_vec(3,0.0);
    rot_rot_vec[2] = rot_ang;
    vgl_rotation_3d<double> rot_rot(rot_rot_vec);

    cam->set_rotation(rot_rot*cam->get_rotation());
  }
}
