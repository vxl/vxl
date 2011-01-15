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
#include <boxm2/cpp/boxm2_cpp_render_process.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/boxm2_opencl_render_process.h>
#include <boxm2/ocl/boxm2_opencl_update_process.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vul/vul_file_iterator.h>

void test_cpp_update_process()
{
    vcl_string scene_file=boxm2_test_utils::save_test_empty_scene();
    vpgl_camera_double_sptr cam=boxm2_test_utils::test_camera();
    vil_image_view<float> * inimg=new  vil_image_view<float>(8,8);
    brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(inimg);

    for (unsigned i=0;i<inimg->ni();i++)
        for (unsigned j=0;j<inimg->nj();j++)
            (*inimg)(i,j)=(float)i/(float)inimg->ni();

    boxm2_scene_sptr scene = new boxm2_scene(scene_file);
    brdb_value_sptr brdb_scene_sptr = new brdb_value_t<boxm2_scene_sptr>(scene);

    // get relevant blocks
    boxm2_nn_cache cache( scene.ptr() );

    // initialize gpu pro / manager
    boxm2_cpp_processor cpp_pro;
    cpp_pro.init();
    cpp_pro.set_scene(scene.ptr());
    brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

    // set inputs
    vcl_vector<brdb_value_sptr> input;
    input.push_back(brdb_scene_sptr);
    input.push_back(brdb_cam);
    input.push_back(brdb_inimg);
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

    vpl_unlink(scene_file.c_str());
}


TESTMAIN(test_cpp_update_process);
