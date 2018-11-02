#include <iostream>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include "../breg3d_ekf_camera_optimizer.h"
#include "../breg3d_ekf_camera_optimizer_state.h"
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/bvxm_voxel_traits.h>

static void test_ekf_camera_optimizer()
{
  START("breg3d_ekf_camera_optimizer test");

  // create the directory under build to put the intermediate files and the generated images
  std::string model_dir("./test_ekf_camera_optimizer");
  vul_file::make_directory(model_dir);

  unsigned nx = 200;
  unsigned ny = 200;
  unsigned nz = 4;
  vgl_point_3d<float> corner(0,0,0);
  vgl_vector_3d<unsigned> num_voxels(nx,ny,nz);
  float voxel_length = 1.0f;

  // create a synthetic world
  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(model_dir, corner, num_voxels, voxel_length);
  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world(params);

  bvxm_voxel_grid_base_sptr ocp_grid_ptr = vox_world->get_grid<OCCUPANCY>(0,0);
  auto *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>*>(ocp_grid_ptr.ptr());
  // fill in grid with zeros to start
  ocp_grid->initialize_data(0.0f);
  // now make a ground plane
  bvxm_voxel_grid<float>::iterator ocp_it = ocp_grid->slab_iterator(nz-1);
  (*ocp_it).fill(1.0f);
  // data not written to disk until iterator is iterated
  ++ocp_it;


  // create a synthetic image to fill layers with
  bvxm_voxel_slab<float> plane_img(nx,ny,1);
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
  typedef bvxm_voxel_traits<APM_MOG_GREY>::voxel_datatype mog_type;

  bvxm_voxel_traits<APM_MOG_GREY>::appearance_processor apm_processor;

  // create a slab of constant weights for update
  bvxm_voxel_slab<float> ones(nx,ny,1);
  ones.fill(1.0f);

  // iterate through layers of apm grid and update each level with the same synthetic image
  // if you want different levels to look different youll have to create a different image for each level
  bvxm_voxel_grid_base_sptr apm_base = vox_world->get_grid<APM_MOG_GREY>(0,0);
  auto *apm_grid = dynamic_cast<bvxm_voxel_grid<mog_type>*>(apm_base.ptr());
  // initialize the appearance model data to get rid of any previous data on disk
  apm_grid->initialize_data(bvxm_voxel_traits<APM_MOG_GREY>::initial_val());

  bvxm_voxel_grid<mog_type>::iterator apm_it = apm_grid->begin();
  for (; apm_it != apm_grid->end(); ++apm_it) {
    apm_processor.update(*apm_it, plane_img, ones);
  }


 // now create a couple of cameras and generate the expected images
  vnl_matrix_fixed<double,3,3> K(0.0);
  double f = 550.0;
  double offx = 320.0;
  double offy = 240.0;
  K(0,0) = f; K(1,1) = f;
  K(0,2) = offx; K(1,2) = offy;
  K(2,2) = 1.0;
  vgl_point_3d<double> center0(100,-50,275);
  vgl_rotation_3d<double> rot0(5*vnl_math::pi/6,0.0,0.0);
  vgl_rotation_3d<double> dR(0.1,-0.15,0.2);
  vgl_vector_3d<double> dC(-8.0,-10.0, 11.0);
  vgl_rotation_3d<double> rot1 = rot0*dR;
  vgl_point_3d<double> center1 = center0 + dC;
  vgl_rotation_3d<double> rot2= rot1*dR.inverse();
  vgl_point_3d<double> center2 = center1 - dC;

  vpgl_camera_double_sptr cam0 = new vpgl_perspective_camera<double>(K,center0,rot0);
  vpgl_camera_double_sptr cam1 = new vpgl_perspective_camera<double>(K,center1,rot1);
  vpgl_camera_double_sptr cam2 = new vpgl_perspective_camera<double>(K,center2,rot2);

  bvxm_image_metadata meta0(vil_image_view_base_sptr(nullptr),cam0);
  bvxm_image_metadata meta1(vil_image_view_base_sptr(nullptr),cam1);
  bvxm_image_metadata meta2(vil_image_view_base_sptr(nullptr),cam2);

  vil_image_view<float> mask(640,480,1);
  vil_image_view_base_sptr img0 = new vil_image_view<unsigned char>(640,480);
  vil_image_view_base_sptr img1 = new vil_image_view<unsigned char>(640,480);
  vil_image_view_base_sptr img2 = new vil_image_view<unsigned char>(640,480);

  vox_world->expected_image<APM_MOG_GREY>(meta0,img0,mask);
  vox_world->expected_image<APM_MOG_GREY>(meta1,img1,mask);
  vox_world->expected_image<APM_MOG_GREY>(meta2,img2,mask);

  // create a camera optimizer
  breg3d_ekf_camera_optimizer_state init_state(1e-3,center0,rot0,0.1,0.1);
  breg3d_ekf_camera_optimizer optimizer(20.0,20.0,0.01,0.01,0.01,0.01,false,true,false);

  bvxm_image_metadata img1_meta(img1,cam1);
  breg3d_ekf_camera_optimizer_state end_state =
    optimizer.optimize(vox_world,img0,img1_meta,init_state);

  vgl_point_3d<double> center1_est = end_state.get_point();
  vgl_rotation_3d<double> rot1_est = end_state.get_rotation();

  std::cout << "camera0 center   : " << center0 << std::endl
           << "camera0 rotation : " << std::endl << rot0.as_rodrigues() << std::endl
           << "camera1 center   : " << center1 << std::endl
           << "camera1 rotation : " << std::endl << rot1.as_rodrigues() << std::endl
           << "est. center      : " << center1_est << std::endl
           << "est. rotation    : " << std::endl << rot1_est.as_rodrigues() << std::endl;

  vgl_vector_3d<double> center_off = center1_est - center1;
  vgl_rotation_3d<double> rot_off(rot1_est.as_rodrigues() - rot1.as_rodrigues());

  TEST_NEAR("Camera Center Convergance Error (1st frame)",center_off.length(),0.0, 0.1);
  TEST_NEAR("Camera Rotation Convergance Error (1st frame)",rot_off.as_rodrigues().magnitude(),0.0, 0.005);

  // optimize again for next step
  //vpgl_perspective_camera<double> cam1_est(K,center1_est,rot1_est);

  bvxm_image_metadata img2_meta(img2,cam2);
   end_state = optimizer.optimize(vox_world,img1,img2_meta,end_state);

  vgl_point_3d<double> center2_est = end_state.get_point();
  vgl_rotation_3d<double> rot2_est = end_state.get_rotation();

  std::cout << "camera1 center   : " << center1 << std::endl
           << "camera1 rotation : " << std::endl << rot1.as_rodrigues() << std::endl
           << "camera2 center   : " << center2 << std::endl
           << "camera2 rotation : " << std::endl << rot2.as_rodrigues() << std::endl
           << "est. center      : " << center2_est << std::endl
           << "est. rotation    : " << std::endl << rot2_est.as_rodrigues() << std::endl;

  center_off = center2_est - center2;
  rot_off = vgl_rotation_3d<double>(rot2_est.as_rodrigues() - rot2.as_rodrigues());

  TEST_NEAR("Camera Center Convergance Error (2nd frame)",center_off.length(),0.0, 0.1);
  TEST_NEAR("Camera Rotation Convergance Error (2nd frame)",rot_off.as_rodrigues().magnitude(),0.0, 0.005);

  return;
}


TESTMAIN( test_ekf_camera_optimizer );
