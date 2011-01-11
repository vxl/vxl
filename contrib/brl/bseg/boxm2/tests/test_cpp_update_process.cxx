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
#include <vul/vul_timer.h>
#include <vpl/vpl.h>
#include "test_utils.h"

#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/cpp/boxm2_cpp_processor.h>
#include <boxm2/cpp/boxm2_cpp_update_process.h>


void test_cpp_update_process()
{
  vcl_string scene_file=boxm2_test_utils::save_test_empty_scene();
#if 0
  vcl_string scene_file="F:/APL/boxm2_rep_scene/scene.xml";
  vcl_string camfile="F:/APL/cams/camera00001.txt";
  vcl_ifstream ifs(camfile.c_str());
  vpgl_perspective_camera<double>* cam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camfile << vcl_endl;
    return ;
  }
  else  {
    ifs >> *cam;
  }
#endif // 0
  vpgl_camera_double_sptr cam=boxm2_test_utils::test_camera();
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

  // create output image buffer
  vil_image_view_base_sptr expimg = new vil_image_view<float>(640,480);
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg);

  // create vis image buffer
  vil_image_view<float>* vis_img = new vil_image_view<float>(640, 480);
  vis_img->fill(1.0f);
  brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);

  // start out updating with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file);

  // get relevant blocks
  boxm2_nn_cache cache( scene.ptr() );

  // initialize gpu pro / manager
  boxm2_cpp_processor cpp_pro;
  cpp_pro.init();
  cpp_pro.set_scene(scene.ptr());

  // === setting the inputs for process ===

  // set inputs
  vcl_vector<brdb_value_sptr> input;
  brdb_value_sptr brdb_scene_sptr = new brdb_value_t<boxm2_scene_sptr>(scene);
  input.push_back(brdb_scene_sptr);

  input.push_back(brdb_cam);
  input.push_back(brdb_expimg);
  input.push_back(brdb_vis);

  //init output vector
  vcl_vector<brdb_value_sptr> output;

  vul_timer t;
  t.mark();
  //////initialize the GPU update process
  boxm2_cpp_update_process cpp_update;
  cpp_update.init();
  cpp_update.set_cache(&cache);
  cpp_pro.run(&cpp_update, input, output);
  cpp_pro.finish();

  vcl_cout<<"Time taken is :" <<t.all()<<vcl_endl;
  vil_save(*(expimg.ptr()),"f:/test.tiff");

  vpl_unlink(scene_file.c_str());
  vcl_string test_dir  = testlib_root_dir()+ "/contrib/brl/bseg/boxm2/tests/";

  boxm2_test_utils::delete_test_scene_from_disk(test_dir);
}


TESTMAIN(test_cpp_update_process);
