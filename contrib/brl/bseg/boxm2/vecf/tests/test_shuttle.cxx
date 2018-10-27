//:
// \file
// \author J.L. Mundy
// \date 03/12/15


#include <iostream>
#include <fstream>
#include <string>
#include <testlib/testlib_test.h>
#include <vul/vul_timer.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/cpp/algo/boxm2_import_triangle_mesh.h>
#include <boxm2/cpp/algo/boxm2_render_functions.h>
#include <imesh/imesh_mesh.h>
#include <imesh/imesh_fileio.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_matrix.h>
#include <boxm2/vecf/boxm2_vecf_shuttle_scene.h>
//#define BUILD_TEST_SHUTTLE
void test_shuttle()
{
#ifdef BUILD_TEST_SHUTTLE

  std::string scene_dir = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/scene/";
  std::string shuttle_scene_path = scene_dir + "STS.xml";
  std::string model_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/STS_objects/STS.ply";
  //std::string cam_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/per_cam.txt";
  std::string obs_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/ShuttlePoseGrey.tif";
  std::string cam_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/PoseFrameCompute.txt";
  std::string cam_out_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/PoseFrameCamTrans.txt";
  std::string img_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/RenderedCam4.tif";
  std::string silhouette_dir = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/silhouettes/";
  std::string camera_dir = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/cameras/";
  if(!vul_file::exists(shuttle_scene_path))
    {
      std::cout<<"shuttle scene file does not exist"<<std::endl;
      return;
    }
  vpgl_affine_camera<double>* acam = new vpgl_affine_camera<double>();
  std::ifstream istr(cam_file.c_str());
  istr >> *acam;
  vpgl_camera_double_sptr cam = acam;
  vil_image_view<vxl_byte> obs = vil_load(obs_file.c_str());
  boxm2_vecf_shuttle_scene  sh_scene(shuttle_scene_path);
  unsigned ni = 1280, nj = 720;
  vgl_point_2d<int> pmin(250,240), pmax(470,600);
  vgl_box_2d<int> bb;
  bb.add(pmin); bb.add(pmax);
  //sh_scene.generate_particles_txy(cam, -50.0, 0.0, 50.0, -50.0, 0.0, 50.0);
  //sh_scene.generate_particles_txs(cam, -6.0, -2.0, 1.0, 0.95, 1.05, 0.025);
  //sh_scene.generate_particles(cam, -7.5, -5.0, 0.5);
  sh_scene.generate_particles_txrz(cam, -50.0, 0.0, 50.0, -0.5, 0.0, 0.5);
  sh_scene.evaluate_particles(obs, bb);
  sh_scene.save_silhouettes(silhouette_dir);
  sh_scene.save_cameras(camera_dir);
#endif
}
TESTMAIN( test_shuttle );
