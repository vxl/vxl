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
  std::string cam_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/PoseFrameCompute.txt";
  std::string cam_out_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/PoseFrameCamTrans.txt";
  std::string img_file = "D:/VisionSystems/Proposals/SBIRProposals/2017/STSData/RenderedCam4.tif";
  if(!vul_file::exists(shuttle_scene_path))
    {
      std::cout<<"shuttle scene file does not exist"<<std::endl;
      return;
    }
#if 0 // old code
  boxm2_scene_sptr sts_scene = new boxm2_scene(shuttle_scene_path);
  boxm2_lru_cache::create(sts_scene);
  std::vector<boxm2_block_id> blocks = sts_scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator iter_blk = blocks.begin();

  boxm2_block* blk_;
  blk_ = boxm2_cache::instance()->get_block(sts_scene, *iter_blk);
  boxm2_data<BOXM2_ALPHA>* alpha_data_;
  boxm2_data<BOXM2_MOG3_GREY>* app_data_;
  boxm2_data<BOXM2_NUM_OBS>* nobs_data_;
  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(sts_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  alpha_data_=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());

  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(sts_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base->enable_write();
  app_data_=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());

  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(sts_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base->enable_write();
  nobs_data_=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());
#if 0
  imesh_mesh m;
  imesh_read(model_file, m);
  boxm2_import_triangle_mesh(sts_scene, boxm2_cache::instance(), m);
  boxm2_cache::instance()->write_to_disk();
#endif
  std::vector<boxm2_data_base*> datas;
  datas.push_back(alpha_base);   datas.push_back(app_base); datas.push_back(nobs_base);
  boxm2_scene_info* linfo = sts_scene->get_blk_metadata(blk_->block_id());
  unsigned ni = 1280, nj = 720;

  vpgl_affine_camera<double> acam;
  std::ifstream istr(cam_file.c_str());
  istr >> acam;
  acam.set_viewing_distance(30.0);
  vnl_matrix_fixed<double,3,4> cam_mat = acam.get_matrix();
#if 0
  vnl_matrix<double> R(3,3);
  R[0][0]= 0.95;  R[0][1]= -0.025;   R[0][2]= -0.025;
  R[1][0]= 0.025;  R[1][1]= 0.95;   R[1][2]=  0.0;
  R[2][0]= 0.025;  R[2][1]= 0.0;   R[2][2]= 1.0;
  vnl_matrix<double> m = cam_mat.extract (3, 3);
  m = m*R;
  cam_mat.update(m);
#endif
  cam_mat[0][3]+=5.0;
  // cam_mat[1][3]+=5.0;
  vpgl_affine_camera<double> tacam(cam_mat);
  std::ofstream ostr(cam_out_file.c_str());
  ostr << tacam;
  ostr.close();
  tacam.set_viewing_distance(30.0);
  vpgl_generic_camera<double>* gcam = new vpgl_generic_camera<double>;
  bool good = vpgl_generic_camera_convert::convert(tacam, ni, nj, *gcam);
  if(good){
    vpgl_camera_double_sptr cam = (vpgl_camera<double>*)gcam;
    vil_image_view<float>* silhouette= new vil_image_view<float>(ni, nj);
    silhouette->fill(1.0f);
    vil_image_view<float>* vis= new vil_image_view<float>(ni, nj);
    vis->fill(1.0f);
    boxm2_render_silhouette(linfo, blk_, alpha_base, cam, silhouette, vis, ni, nj);
    vil_save(*silhouette, img_file.c_str());
  }
#endif //old code
  boxm2_vecf_shuttle_scene  sh_scene(shuttle_scene_path);
  unsigned ni = 1280, nj = 720;
  vil_image_view<float> silhouette = sh_scene.silhouette(cam_file.c_str(), ni, nj);
  vil_save(silhouette, img_file.c_str());
#endif
}
TESTMAIN( test_shuttle );
