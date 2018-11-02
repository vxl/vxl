#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vpgl/vpgl_perspective_camera.h>

#include "../breg3d_ekf_camera_optimizer.h"
#include "../breg3d_ekf_camera_optimizer_state.h"
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/bvxm_voxel_traits.h>
#include "../breg3d_lm_direct_homography_generator.h"

static void test_ekf_existing_world()
{
  START("breg3d_ekf_existing_world test");

  // create the directory under build to put the intermediate files and the generated images
  std::string model_dir("./test_ekf_camera_optimizer_seq");
  std::string reg_dir("./test_ekf_camera_optimizer_seq/reg");
  bvxm_world_params_sptr params = new bvxm_world_params();
  unsigned nx = 400;
  unsigned ny = 400;
  unsigned nz = 30;
  vgl_point_3d<float> corner(0.0f, 0.0f, 0.0f);
  vgl_vector_3d<unsigned int> num_voxels(nx, ny, nz);
  params->set_params(model_dir, corner, num_voxels, 1.0);
  bvxm_voxel_world_sptr world = new bvxm_voxel_world(params);
  std::string cam_f = model_dir + "/cam_00000.txt";
  std::string cam_fm = model_dir + "/cam_00000_m.txt";
  vpgl_perspective_camera<double> cam;
  std::ifstream is(cam_f.c_str());
  is >> cam;
  vpgl_perspective_camera<double> cam_m;
  std::ifstream ism(cam_fm.c_str());
  ism >> cam_m;

  unsigned nx_image = 320;
  unsigned ny_image = 240;
  vil_image_view<unsigned char>* img = new vil_image_view<unsigned char>(nx_image,ny_image);
  vil_image_view_base_sptr img_sptr = img;
  vil_image_view<unsigned char>* img_m = new vil_image_view<unsigned char>(nx_image,ny_image);
  vil_image_view_base_sptr img_m_sptr = img_m;

  vil_image_view<float> mask(nx_image,ny_image,1);
  bvxm_image_metadata meta(vil_image_view_base_sptr(0),&cam);
  world->expected_image<APM_MOG_GREY>(meta,img_sptr,mask);
  vil_image_view<float> mask_m(nx_image,ny_image,1);
  bvxm_image_metadata meta_m(vil_image_view_base_sptr(0),&cam_m);
  world->expected_image<APM_MOG_GREY>(meta_m,img_m_sptr,mask_m);

  std::string img_f = model_dir + "/joe_00000.pgm";
  vil_save(*img,img_f.c_str());

  std::string img_fm = model_dir + "/joe_00000_m.pgm";
  vil_save(*img_m,img_fm.c_str());


  vil_image_view<float>f_img = *vil_convert_cast(float(), img);
  vil_image_view<float>f_img_m = *vil_convert_cast(float(), img_m);
  breg3d_lm_direct_homography_generator lm_gen;
  lm_gen.set_image0(&f_img);
  lm_gen.set_mask0(&mask);
  lm_gen.set_image1(&f_img_m);
  lm_gen.set_mask1(&mask_m);
  lm_gen.set_projective(false);
  ihog_transform_2d xform = lm_gen.compute_homography();
  std::cout << xform.get_matrix() << std::endl << std::endl;
 }


  TESTMAIN( test_ekf_existing_world );
