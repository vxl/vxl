//:
// \file
// \author Vishal Jain
// \date 26-Dec-2010

#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vpl/vpl.h>
#include "test_utils.h"

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_cpp_processor.h>
#include <boxm2/boxm2_cpp_render_process.h>

vpgl_camera_double_sptr test_camera()
{
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_point_3d<double> t(0.5,0.5,100);

  vcl_cout<<mk<<mr<<t;
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,t,R);
  return cam;
}

void test_cpp_render_process()
{
  vcl_string scene_file=boxm2_test_utils::save_test_simple_scene();

  vpgl_camera_double_sptr cam=test_camera();
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

  // create output image buffer
  vil_image_view_base_sptr expimg = new vil_image_view<float>(8,8);
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg);

  // create vis image buffer
  vil_image_view<float>* vis_img = new vil_image_view<float>(8, 8);
  vis_img->fill(1.0f);
  brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);

  // start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file);

  // get relevant blocks
  boxm2_nn_cache cache( scene->data_path(), vgl_vector_3d<int>(1,1,1) );

  // initialize gpu pro / manager
  boxm2_cpp_processor cpp_pro;
  cpp_pro.init();
  cpp_pro.set_scene(scene.ptr());

  // === setting the inputs for process ===

  // set inputs
  vcl_vector<brdb_value_sptr> input;

  vcl_vector<boxm2_block_id> vis_order;
  vis_order.push_back(boxm2_block_id(0,0,0));
  for (unsigned int i=0; i<vis_order.size(); ++i) {
    boxm2_block_id         id  = vis_order[i];
    boxm2_block_sptr      blk  = cache.get_block(id);
    boxm2_data_base_sptr  alph = cache.get_data<BOXM2_ALPHA>(id);
    boxm2_data_base_sptr  mog  = cache.get_data<BOXM2_MOG3_GREY>(id);

    boxm2_scene_info_wrapper *scene_info_wrapper=new boxm2_scene_info_wrapper();
    scene_info_wrapper->info=scene->get_blk_metadata(id);

    brdb_value_sptr brdb_scene_info_wrapper = new brdb_value_t<boxm2_scene_info_wrapper_sptr>(scene_info_wrapper);
    input.push_back(brdb_scene_info_wrapper);
    brdb_value_sptr brdb_blk = new brdb_value_t<boxm2_block_sptr>(blk);
    input.push_back(brdb_blk);
    brdb_value_sptr brdb_alpha = new brdb_value_t<boxm2_data_base_sptr>(alph);
    input.push_back(brdb_alpha);
    brdb_value_sptr brdb_mog = new brdb_value_t<boxm2_data_base_sptr>(mog);
    input.push_back(brdb_mog);
  }

  input.push_back(brdb_cam);
  input.push_back(brdb_expimg);
  input.push_back(brdb_vis);

  //init output vector
  vcl_vector<brdb_value_sptr> output;

  //////initialize the GPU render process
  boxm2_cpp_render_process cpp_render;
  cpp_render.init();
  cpp_pro.run(&cpp_render, input, output);
  cpp_pro.finish();

  vpl_unlink(scene_file.c_str());
  vcl_string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";

  boxm2_test_utils::delete_test_scene_from_disk(test_dir);
}


TESTMAIN(test_cpp_render_process);
