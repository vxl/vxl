//:
// \file
// \author J.L. Mundy
// \date 10/12/14


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_timer.h>
#include <bocl/bocl_manager.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_util.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_ocl_transform_scene.h"
#include "../boxm2_vecf_ocl_transform_minimizer.h"
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vpgl/vpgl_perspective_camera.h>
typedef vnl_vector_fixed<unsigned char, 16> uchar16;

void test_transform()
{
  // Set up the scenes
  //  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/putin_face/";
  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/mean_face/";
  std::string source_scene_path = base_dir_path + "source_scene.xml";
  std::string target_scene_path = base_dir_path + "target_scene.xml";
  if(!vul_file::exists(source_scene_path) || !vul_file::exists(target_scene_path) )
  {
      std::cout<<"One or both of the secene files do not exist"<<std::endl;
      return;
  }
  boxm2_scene_sptr source_scene = new boxm2_scene(source_scene_path);
  boxm2_scene_sptr target_scene = new boxm2_scene(target_scene_path);
  boxm2_lru_cache::create(source_scene);

  bocl_manager_child &mgr = bocl_manager_child::instance();
  unsigned gpu_idx = 1; //on JLM's alienware
  bocl_device_sptr device = mgr.gpus_[gpu_idx];
  boxm2_opencl_cache_sptr opencl_cache = new boxm2_opencl_cache(device);
  //=====test with identity transform========
  vnl_matrix_fixed<double, 3, 3> M(0.0);
  M[0][0] = 1.0;   M[1][1] = 1.0;   M[2][2] = 1.0;
  vgl_rotation_3d<double> R(M);
  vgl_vector_3d<double> t(0.0, 0.0, 0.0);
  vgl_vector_3d<double> si(1.0, 1.0, 1.0);
  vgl_vector_3d<double> s0(0.9, 1.0, 1.0);
  vgl_vector_3d<double> s1(1.0, 0.9, 1.0);
  //===========================================
   int ni = 360, nj = 360;
   boxm2_vecf_ocl_transform_scene trs(source_scene, target_scene, opencl_cache);
   trs.transform_1_blk_interp(R, t, si, true);
   return;
#if 0
   vil_image_view<float> exp, vis;
  //  std::string cam_path = base_dir_path + "putin_cam.txt";
    std::string cam_path = base_dir_path + "putin_small_cam.txt";
    //    std::string cam_path = base_dir_path + "putin_side_cam.txt";
  std::ifstream ist(cam_path.c_str());
  vpgl_perspective_camera<double> cam;
  ist >> cam;
  vpgl_camera_double_sptr camptr = new vpgl_perspective_camera<double>(cam);
 trs.render_scene_appearance(camptr, exp, vis, ni, nj, true);
 std::string img_path = base_dir_path + "putin_trans_rend.tiff";
  vil_save(exp, img_path.c_str());
  trs.transform_1_blk_interp(R, t, si, true);
#endif

#if 0
  int n = 1000;
  vul_timer tim;
   vil_image_view<float> exp, vis;
  //  std::string cam_path = base_dir_path + "putin_cam.txt";
  std::string cam_path = base_dir_path + "putin_small_cam.txt";
  std::ifstream ist(cam_path.c_str());
  vpgl_perspective_camera<double> cam;
  ist >> cam;
  vpgl_camera_double_sptr camptr = new vpgl_perspective_camera<double>(cam);
  for(int i = 0; i<n; ++i){
    trs.transform_1_blk(R, t, s0, false);
    trs.render_scene_appearance(camptr, exp, vis, ni, nj, false);
  }
  std::cout << "transform/render time " << tim.real()/(n*1000.0) << " sec\n" << std::flush;
#endif

#if 0// The minimizer example

  std::string img_path = base_dir_path + "putin_trans_rend.tiff";
  vil_image_view<float> ref_img = vil_load(img_path.c_str());
  std::string cam_path = base_dir_path + "putin_small_cam.txt";
  std::ifstream ist(cam_path.c_str());
  vpgl_perspective_camera<double> cam;
  ist >> cam;
  vpgl_camera_double_sptr camptr = new vpgl_perspective_camera<double>(cam);
  boxm2_vecf_ocl_transform_minimizer tm(source_scene, target_scene, opencl_cache, ref_img, camptr);
  tm.minimize();
  double smin =0.9, smax = 1.1, sinc = 0.001;
  tm.error_surface_1d(0, smin, smax, sinc);
  vil_image_view<float> dif = tm.opt_diff();
  std::string diff_img_path = base_dir_path + "putin_trans_diff.tiff";
  vil_save(dif, diff_img_path.c_str());
  tm.finish();
#endif
}

TESTMAIN( test_transform );
