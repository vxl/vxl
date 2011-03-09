#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_refine_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 refine: BOXM2 SCENE"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  //start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);

  //data type   
  brdb_value_sptr brdb_data_type = new brdb_value_t<vcl_string>(vcl_string("8bit"));

  //initialize a block and data cache
  boxm2_lru_cache::create(scene.ptr()); 
  boxm2_cache* cache = boxm2_cache::instance(); 

  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr());
  gpu_pro->set_cpu_cache(cache);
  gpu_pro->init();

  ////set inputs
  vcl_vector<brdb_value_sptr> input;
  input.push_back(brdb_scene);
  input.push_back(brdb_data_type);

  //initoutput vector
  vcl_vector<brdb_value_sptr> output;

  ////initialize GPU update process
  boxm2_opencl_refine_process refine;
  refine.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], " -D MOG_TYPE_8"); 
  
  gpu_pro->run(&refine, input, output); 
  
  //print cache for debugging
  vcl_cout<<cache<<vcl_endl;

  //////////////////////////////////////////////////////////////////////////////
  //save blocks and data to disk for debugging
  //vcl_cout<<"SAVING BLOCKS FOR DEBUGGING!"<<vcl_endl;
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for(iter = blocks.begin(); iter != blocks.end(); ++iter)
  { 
    boxm2_block_id id = iter->first; 
    boxm2_sio_mgr::save_block(scene->data_path(), cache->get_block(id)); 
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_ALPHA>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_MOG3_GREY>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_NUM_OBS>(id) );
  }


  return 0;
}
