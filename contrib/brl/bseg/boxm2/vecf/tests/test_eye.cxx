//:
// \file
// \author J.L. Mundy
// \date 11/19/14


#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vul/vul_timer.h>
#include <boct/boct_bit_tree.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <vul/vul_file.h>
#include "../boxm2_vecf_eye_scene.h"
#include "../boxm2_vecf_eye_params.h"
typedef vnl_vector_fixed<unsigned char, 16> uchar16;

void test_eye()
{
#if 0
  // Set up the scenes
  std::string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/eye/";
  std::string eye_scene_path = base_dir_path + "eye.xml";
  std::string target_scene_path = base_dir_path + "target_eye.xml";
  if(!vul_file::exists(eye_scene_path))
  {
      std::cout<<"eye scene file) does not exist"<<std::endl;
      return;
  }
  bool init = false;
  boxm2_vecf_eye_scene* eye = new boxm2_vecf_eye_scene(eye_scene_path, init);
  if(init)
    boxm2_cache::instance()->write_to_disk();

  boxm2_scene_sptr target_scene = new boxm2_scene(target_scene_path);
  boxm2_cache::instance()->add_scene(target_scene);
  eye->map_to_target(target_scene);
  boxm2_cache::instance()->write_to_disk();
#if 0
  boxm2_vecf_eye_params ep;
  ep.offset_ = vgl_vector_3d<double>(36.6, 31.4, 65.7);
  eye->set_params(ep);
 // double ang =0.7853982;
  double ang =0.0;
  vnl_vector_fixed<double, 3> rod(0.0, ang, 0.0);
  vgl_rotation_3d<double> rot(rod);
  std::vector<vgl_vector_3d<double> > invf = eye.inverse_vector_field(rot);
  eye.apply_vector_field_to_self(invf);

  vul_timer t;
  for(unsigned i = 0; i<500; ++i)
    eye.rotate(rot);
  std::cout << "time = " << t.real()/1000.0 << '\n'<< std::flush;
  boxm2_vecf_eye eye;
  eye.initialize_scene(eye_scene_path);
  eye.create_eye();
  double ang =0.7853982;
  vnl_vector_fixed<double, 3> rod(0.0, ang, 0.0);
  vgl_rotation_3d<double> rot(rod);
  vul_timer t;
  for(unsigned i = 0; i<500; ++i)
    eye.rotate(rot);
  std::cout << "time = " << t.real()/1000.0 << '\n'<< std::flush;
  boxm2_cache::instance()->write_to_disk();

  boxm2_scene_sptr eye_scene = new boxm2_scene(eye_scene_path);
  boxm2_lru_cache::create(eye_scene);
  std::vector<boxm2_block_id> blocks = eye_scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator iter_blk = blocks.begin();
  boxm2_block_sptr blk = boxm2_cache::instance()->get_block(eye_scene, *iter_blk);
  vgl_point_3d<double> p(0.0, 0.0, 0.0);
  unsigned indx, depth;
  double side_len;
  blk->data_index(p, indx, depth, side_len);
  boxm2_data_base *  alpha_base  = boxm2_cache::instance()->get_data_base(eye_scene,*iter_blk,boxm2_data_traits<BOXM2_ALPHA>::prefix());
  alpha_base->enable_write();
  boxm2_data<BOXM2_ALPHA>* alpha_data=new boxm2_data<BOXM2_ALPHA>(alpha_base->data_buffer(),alpha_base->buffer_length(),alpha_base->block_id());
  alpha_data->data()[indx]= 2.0f;
#if 1
  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(eye_scene,*iter_blk,boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  app_base->enable_write();
  boxm2_data<BOXM2_MOG3_GREY>* app_data=new boxm2_data<BOXM2_MOG3_GREY>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());
  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype app;
  app.fill(0);
  app[0]=0; app[1]=32; app[2] = 255;
  app_data->data()[indx] = app;
#elif
  boxm2_data_base *  app_base  = boxm2_cache::instance()->get_data_base(eye_scene,*iter_blk,boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
  app_base->enable_write();
  boxm2_data<BOXM2_GAUSS_RGB>* app_data=new boxm2_data<BOXM2_GAUSS_RGB>(app_base->data_buffer(),app_base->buffer_length(),app_base->block_id());
  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype app;
  app.fill(0);
  //  app[0]=150; app[1]=43; app[2]=21;
  app[0]=150; app[1]=43;
  app_data->data()[indx] = app;
#endif

#if 1
  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(eye_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
  nobs_base->enable_write();
  boxm2_data<BOXM2_NUM_OBS>* nobs_data=new boxm2_data<BOXM2_NUM_OBS>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());
  boxm2_data_traits<BOXM2_NUM_OBS>::datatype nobs;
  nobs.fill(0);
  nobs_data->data()[indx] = nobs;
#elif
  boxm2_data_base *  nobs_base  = boxm2_cache::instance()->get_data_base(eye_scene,*iter_blk,boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix());
  nobs_base->enable_write();
  boxm2_data<BOXM2_NUM_OBS_SINGLE>* nobs_data=new boxm2_data<BOXM2_NUM_OBS_SINGLE>(nobs_base->data_buffer(),nobs_base->buffer_length(),nobs_base->block_id());
  boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::datatype nobs = 0;
  nobs_data->data()[indx] = nobs;
#endif
#endif
#endif

  }
TESTMAIN( test_eye );
