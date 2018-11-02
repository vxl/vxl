#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_cross.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include "../breg3d_ekf_camera_optimizer.h"
#include "../breg3d_ekf_camera_optimizer_state.h"
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/bvxm_voxel_traits.h>
#define CREATE_SYNTHETIC_IMAGE
static void init_ground_plane(double plane_z, double plane_std, bvxm_voxel_world_sptr vox_world)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  bvxm_voxel_grid<ocp_datatype> *grid = dynamic_cast<bvxm_voxel_grid<ocp_datatype>*>(vox_world->get_grid<OCCUPANCY>(0,0).ptr());
  unsigned nz = grid->grid_size().z();
  unsigned nx = grid->grid_size().x();
  unsigned ny = grid->grid_size().y();

  bvxm_voxel_grid<ocp_datatype>::iterator ocp_it = grid->begin();
  double groundz = nz - plane_z - 1;
  for (unsigned z=0; z < nz; z++, ++ocp_it) {

    bvxm_voxel_slab<ocp_datatype> plane_slab(nx,ny,1);

    ocp_datatype min_prob = vox_world->get_params()->min_occupancy_prob();
    ocp_datatype max_prob = vox_world->get_params()->max_occupancy_prob();
    ocp_datatype plane_prob =
      (ocp_datatype)(vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/plane_std) * std::exp(-(((z-groundz)*(z-groundz))/(2*plane_std*plane_std))));
    if (plane_prob < min_prob)
      plane_prob = min_prob;
    if (plane_prob > max_prob)
      plane_prob = max_prob;

    std::cout << "z = " << z << "  plane_prob = " << plane_prob << std::endl;

    plane_slab.fill(plane_prob);

    bvxm_voxel_slab<ocp_datatype>::iterator vox_it_in = plane_slab.begin(), vox_it_out = (*ocp_it).begin();
    for (; vox_it_out != (*ocp_it).end(); ++vox_it_out, ++vox_it_in) {
      *vox_it_out = *vox_it_in;
    }
  }
}

static void test_ekf_camera_optimizer_seq()
{
  START("breg3d_ekf_camera_optimizer_seq test");

  // create the directory under build to put the intermediate files and the generated images
  std::string model_dir("./test_ekf_camera_optimizer_seq");
  vul_file::make_directory(model_dir);

  std::string reg_dir("./test_ekf_camera_optimizer_seq/reg");
  vul_file::make_directory(reg_dir);

  unsigned nx = 400;
  unsigned ny = 400;
  unsigned nz = 30;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 0.5f;

  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world(params);
  vox_world->clean_grids();

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0,0);
  bvxm_voxel_grid<float> *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;

  // create a synthetic image to fill layers with
  bvxm_voxel_slab<float> plane_img(nx,ny,1);
#ifdef CREATE_SYNTHETIC_IMAGE
  for (unsigned i=0; i<nx; ++i) {
    for (unsigned j=0; j<ny; ++j) {
      // mark the origin/x axis
      if ( (i < 20) && (j < 5) ) {
        plane_img(i,j) = 0.2f;
      }
      // just make some squares of constant color
      else if ( (i > 10) && (i < 90) && (j > 10) && (j < 90) ) {
        plane_img(i,j) = 0.7f;
      }
      else if ((i > 110) && (i < 190) && (j > 10) && (j < 90) ) {
        plane_img(i,j) = 0.5f;
      }
      else if ((i > 10) && (i < 90) && (j > 110) && (j < 190) ) {
        plane_img(i,j) = 0.3f;
      }
      else if ((i > 110) && (i < 190) && (j > 110) && (j < 190) ) {
        plane_img(i,j) = 0.1f;
      }
      else {
        plane_img(i,j) = 1.0;
      }
      //plane_img(i,j) = plane_img(i,j) + ((0.2f*i)/(float)nx + (0.2f*j)/(float)ny);
    }
  }
#else
  vil_image_view_base_sptr plane_img_byte = vil_load("./crop400x400.tiff");
  bvxm_util::img_to_slab(plane_img_byte, plane_img);

#endif


  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  // create a slab of constant weights for update
  bvxm_voxel_slab<float> ones(nx,ny,1);
  ones.fill(1.0f);

  // iterate through layers of apm grid and update each level with the same synthetic image
  // if you want different levels to look different youll have to create a different image for each level
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0,0);
  bvxm_voxel_grid<mog_type> *apm_grid = dynamic_cast<bvxm_voxel_grid<mog_type>*>(apm_base.ptr());
  // initialize the appearance model data to get rid of any previous data on disk
  apm_grid->initialize_data(bvxm_voxel_traits<APM_MOG_GREY>::initial_val());

  bvxm_voxel_grid<mog_type>::iterator apm_it = apm_grid->begin();
  for (; apm_it != apm_grid->end(); ++apm_it) {
    apm_processor.update(*apm_it, plane_img, ones);
  }


  // now create a sequence of cameras and generate the expected images
  vnl_matrix_fixed<double,3,3> K(0.0);
  unsigned nx_image = 320;
  unsigned ny_image = 240;
  double f = 500.0;
  double offx = nx_image / 2.0;
  double offy = ny_image / 2.0;
  K(0,0) = f; K(1,1) = f;
  K(0,2) = offx; K(1,2) = offy;
  K(2,2) = 1.0;
  double h = 325;
  double path_radius = 300;
  double path_dist_to_scene = 300;
  unsigned ncameras = 30;
  double total_angle = vnl_math::pi / 2;

  std::vector<vpgl_camera_double_sptr> cameras;
  double a=0;
  for (unsigned n=0; n < ncameras; ++n, a+= total_angle/ncameras) {
    double centerx =  std::cos(a + vnl_math::pi_over_2)*path_radius - (path_dist_to_scene - path_radius);
    double centery = -std::sin(a + vnl_math::pi_over_2)*path_radius - (path_dist_to_scene - path_radius);
    double centerz = h;
    vgl_point_3d<double> center(centerx,centery,centerz);
    vnl_vector_fixed<double,3> lz((nx*double(voxel_length)/2) - centerx, (ny*double(voxel_length)/2) - centery, 0.0 - centerz);
    lz = lz.normalize();
    vnl_vector_fixed<double,3> ly1(0.0, 0.0, -1.0);
    vnl_vector_fixed<double,3> lx = vnl_cross_3d(ly1,lz);
    lx = lx.normalize();
    vnl_vector_fixed<double,3> ly = vnl_cross_3d(lz,lx);
    vnl_matrix_fixed<double,3,3> R;
    R(0,0) = lx(0); R(0,1) = lx(1); R(0,2) = lx(2);
    R(1,0) = ly(0); R(1,1) = ly(1); R(1,2) = ly(2);
    R(2,0) = lz(0); R(2,1) = lz(1); R(2,2) = lz(2);
    vgl_rotation_3d<double> rot(R);
    vpgl_perspective_camera<double> *cam_persp = new vpgl_perspective_camera<double>(K,center,rot);
    vpgl_camera_double_sptr cam = cam_persp;
    cameras.push_back(cam);

    // save camera
    std::stringstream cam_fname;
    cam_fname << model_dir << "/cam_" << std::setw(5) << std::setfill('0') << n << ".txt";
    // write matrices to the text file.
    std::ofstream ofs(cam_fname.str().c_str());
    if (!ofs.is_open()) {
      std::cerr << "Failed to open file " << cam_fname.str() << '\n';
    }
    ofs << cam_persp->get_calibration().get_matrix() << '\n'
        << R << std::endl;
    vnl_vector_fixed<double,3> T = -R*vnl_vector_fixed<double,3>(center.x(),center.y(),center.z());
    ofs << T << std::endl;
    ofs.close();
  }

  // generate synthetic images
  std::vector<vil_image_view_base_sptr> images;
  vil_image_view<float> mask(nx_image,ny_image,1);
  for (unsigned n=0; n<ncameras; ++n) {
    vil_image_view_base_sptr img = new vil_image_view<unsigned char>(nx_image,ny_image);
    bvxm_image_metadata meta(vil_image_view_base_sptr(0),cameras[n]);
    vox_world->expected_image<APM_MOG_GREY>(meta,img,mask);
    images.push_back(img);
    // debug: write out images
    std::stringstream fname;
    fname << model_dir << "/image_" << std::setw(5) << std::setfill('0') << n << ".tiff" << std::ends;
    vil_save(*img,fname.str().c_str());
  }

  // create a camera optimizer
  vpgl_perspective_camera<double> *cam0 = dynamic_cast<vpgl_perspective_camera<double>*>(cameras[0].ptr());
  breg3d_ekf_camera_optimizer_state opt_state(1e-3,cam0->get_camera_center(),cam0->get_rotation(),0.1,0.1);
  breg3d_ekf_camera_optimizer optimizer(20.0,20.0,0.0001,0.0001,20.01,20.01,true,true,false);

  // create an empty world for registration
  bvxm_world_params_sptr params_reg = new bvxm_world_params();
  params->set_params(reg_dir, corner, num_voxels, voxel_length, 0, 0.001f, 0.999f);
  bvxm_voxel_world_sptr vox_world_reg = new bvxm_voxel_world(params);
  vox_world_reg->clean_grids();
  bvxm_voxel_grid_base_sptr ocp_base = vox_world->get_grid<OCCUPANCY>(0,0);
  ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_base.ptr());

    bvxm_image_metadata img_meta;

  // initialize the ground plane
  init_ground_plane(0.0,0.001,vox_world_reg);

  // update world using first image(s)
  unsigned ntrain = ncameras;
  for (unsigned n=0; n<ntrain; ++n) {
    img_meta.camera = cameras[n];
    img_meta.img = images[n];
    vox_world_reg->update<APM_MOG_GREY>(img_meta,0);
  }

  // TEMP DEBUG
  for (unsigned n=0; n<ncameras; ++n) {
    vil_image_view_base_sptr img = new vil_image_view<unsigned char>(nx_image,ny_image);
    bvxm_image_metadata meta(vil_image_view_base_sptr(0),cameras[n]);
    vox_world_reg->expected_image<APM_MOG_GREY>(meta,img,mask);
    // debug: write out images
    std::stringstream fname;
    fname << model_dir << "/reg/image_" << std::setw(5) << std::setfill('0') << n << ".tiff" << std::ends;
    vil_save(*img,fname.str().c_str());
  }
  // END TEMP DEBUG


  // optimize the positions of the rest of the cameras
  for (unsigned n=1; n<ncameras; ++n) {
    cam0 = dynamic_cast<vpgl_perspective_camera<double>*>(cameras[n-1].ptr());
    vpgl_perspective_camera<double>* cam1 = dynamic_cast<vpgl_perspective_camera<double>*>(cameras[n].ptr());

    img_meta.camera = new vpgl_perspective_camera<double>(cam1->get_calibration(),cam1->get_camera_center(),cam1->get_rotation());
    img_meta.img = images[n];
    opt_state = optimizer.optimize(vox_world_reg,images[n-1],img_meta,opt_state);
    vgl_point_3d<double> center_est = opt_state.get_point();
    vgl_rotation_3d<double> rot_est = opt_state.get_rotation();

    std::cout << "camera " << n-1 << " center   : " << cam0->get_camera_center() << '\n'
             << "camera " << n-1 << " rotation :\n" << cam0->get_rotation().as_rodrigues() << '\n'
             << "camera " << n << " center   : " << cam1->get_camera_center() << '\n'
             << "camera " << n << " rotation :\n" << cam1->get_rotation().as_rodrigues() << '\n'

             << "est. center      : " << center_est << '\n'
             << "est. rotation    :\n" << rot_est.as_rodrigues() << std::endl;

    vgl_vector_3d<double> center_off = center_est - cam1->get_camera_center();
    vgl_rotation_3d<double> rot_off(rot_est.as_rodrigues() - cam1->get_rotation().as_rodrigues());

    std::stringstream test_string_center;
    test_string_center << "Camera Center Convergence Error (frame " << n << ')';
    TEST_NEAR(test_string_center.str().c_str(),center_off.length(),0.0, 0.1);
    std::stringstream test_string_rot;
    test_string_rot << "Camera Rotation Convergence Error (frame " << n << ')';
    TEST_NEAR(test_string_rot.str().c_str(),rot_off.as_rodrigues().magnitude(),0.0, 0.005);

    // debug - generate expected image from camera n true position
    vil_image_view_base_sptr expected_true = new vil_image_view<vxl_byte>(nx_image,ny_image,1);
    vil_image_view<float> expected_true_mask(nx_image,ny_image,1);
    bvxm_image_metadata cam1_true_meta(vil_image_view_base_sptr(0),cameras[n]);
    vox_world_reg->expected_image<APM_MOG_GREY>(cam1_true_meta,expected_true,expected_true_mask,0);
    vil_save(*expected_true,"./expected_true.tiff");

    // save camera
    std::stringstream cam_est_fname;
    cam_est_fname << model_dir << "/cam_est_" << std::setw(5) << std::setfill('0') << n << ".txt";
    // write matrices to the text file.
    std::ofstream ofs(cam_est_fname.str().c_str());
    if (!ofs.is_open()) {
      std::cerr << "Failed to open file " << cam_est_fname.str() << '\n';
    }
    vpgl_perspective_camera<double> cam_est(cam0->get_calibration(),center_est,rot_est);
    ofs << cam_est.get_calibration().get_matrix() << std::endl;
    vnl_matrix_fixed<double,3,3> R = cam_est.get_rotation().as_matrix();
    ofs << R << std::endl;
    vgl_point_3d<double> c = cam_est.get_camera_center();
    vnl_vector_fixed<double,3> T = -R*vnl_vector_fixed<double,3>(c.x(),c.y(),c.z());
    ofs << T << std::endl;
    ofs.close();

    // update voxel world with optimized camera
    img_meta.camera = vpgl_camera_double_sptr(&cam_est);
    img_meta.img = images[n];
    //vox_world_reg->update<APM_MOG_GREY>(img_meta,0);

#if 0
    // save voxel world
    vox_world_reg->save_occupancy_raw<APM_MOG_GREY>(model_dir + "/ocp_reg.raw");
#endif
  }
  return;
}

TESTMAIN( test_ekf_camera_optimizer_seq );
