#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_cstdio.h>


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
#include <boxm2/ocl/pro/boxm2_opencl_render_process.h>
#include <boxm2/cpp/boxm2_cpp_processor.h>
#include <boxm2/cpp/pro/boxm2_cpp_refine_process.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 refine: BOXM2 SCENE"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<bool> cpu("-cpu", "specify if you want refine done on the CPU", false);
  //export args
  vul_arg<vcl_string> dir("-dir", "output image directory", "");
  vul_arg<unsigned> ni("-ni", "Width of images", 1024);
  vul_arg<unsigned> nj("-nj", "Height of images", 768);
  vul_arg<double> incline0("-init_incline", "Initial angle of incline (degrees) away from zenith", 25.0);
  vul_arg<double> incline1("-end_incline", "Final angle of incline (degrees) away from zenith", 55.0);
  vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
  vul_arg<bool> bit8("-bit8", "True for 8 bit and false for 16 bit", true);

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

  ////set refine inputs
  vcl_vector<brdb_value_sptr> input;
  input.push_back(brdb_scene);
  input.push_back(brdb_data_type);

  //initoutput vector
  vcl_vector<brdb_value_sptr> output;

  if (cpu())
  {
    // initialize gpu pro / manager
    boxm2_cpp_processor cpp_pro;
    cpp_pro.init();
    cpp_pro.set_scene(scene.ptr());

    boxm2_cpp_refine_process cpu_refine;
    cpu_refine.init();
    cpu_refine.set_cache(cache);
    cpp_pro.run(&cpu_refine, input, output);
    cpp_pro.finish();
  }
  else {

    //initialize gpu pro / manager
    boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
    gpu_pro->set_scene(scene.ptr());
    gpu_pro->set_cpu_cache(cache);
    gpu_pro->init();

    //initialize GPU update process
    boxm2_opencl_refine_process refine;
    refine.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], "");
    gpu_pro->run(&refine, input, output);
  }

  //print cache for debugging
  boxm2_lru_cache* lru_cache = (boxm2_lru_cache*) cache;
  vcl_cout<<(*lru_cache)<<vcl_endl;

  ////////////////////////////////////////////////////////////////////////////
  //save blocks and data to disk for debugging
  vcl_cout<<"SAVING BLOCKS FOR DEBUGGING!"<<vcl_endl;
  vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
  for (iter = blocks.begin(); iter != blocks.end(); ++iter)
  {
    boxm2_block_id id = iter->first;
    boxm2_sio_mgr::save_block(scene->data_path(), cache->get_block(id));
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_ALPHA>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_MOG3_GREY>(id) );
    boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache->get_data<BOXM2_NUM_OBS>(id) );
  }
#if 0
  //////////////////////////////////////////////////////////////////////////////
  // IF OUTPUT DIR IS SPECIFIED, render a trajectory
  //////////////////////////////////////////////////////////////////////////////

  if (dir() != "")
  {
    //////////////////////////////////////////////////////////////////////////////
    //Set Up Directory Structure
    //////////////////////////////////////////////////////////////////////////////
    //see if directory exists
    if ( vul_file::exists(dir()) && vul_file::is_directory(dir()) ) {
      vcl_cout<<"Directory "<<dir()<<" exists - overwriting it."<<vcl_endl;
    }
    else {
      vul_file::make_directory_path(dir());
  #ifdef DEBUG
      vcl_cout<<"Couldn't make directory at "<<dir()<<vcl_endl;
      return -1;
  #endif
    }

    //grab instance of gpu processor
    boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
    gpu_pro->set_scene(scene.ptr());
    gpu_pro->set_cpu_cache(cache);
    gpu_pro->init();

    //initialize the GPU render process
    boxm2_opencl_render_process gpu_render;
    vcl_string opts = bit8() ? " -D MOG_TYPE_8 " : " -D MOG_TYPE_16 ";

    gpu_render.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], opts);

    //create output image buffer
    vil_image_view<unsigned int>* expimg = new vil_image_view<unsigned int>(ni(), nj());
    expimg->fill(0);
    vil_image_view_base_sptr expimg_sptr(expimg);
    brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg_sptr);

    //create vis image buffer
    vil_image_view<float>* vis_img = new vil_image_view<float>(ni(), nj());
    vis_img->fill(1.0f);
    brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);

    brdb_value_sptr brdb_data_type;
    if (!bit8())
      brdb_data_type= new brdb_value_t<vcl_string>(vcl_string("16bit"));
    else
      brdb_data_type= new brdb_value_t<vcl_string>(vcl_string("8bit"));

    float tf[256];
    bbas_1d_array_float_sptr tf_sptr;
    tf_sptr=new bbas_1d_array_float(256);
    float mini_=0.0f;
    float maxi_=1.0f;
    for (int i=0; i<256; ++i) tf_sptr->data_array[i] = (float)i/255.0f;
    brdb_value_sptr brdb_mini = new brdb_value_t<float>(mini_);
    brdb_value_sptr brdb_maxi = new brdb_value_t<float>(maxi_);
    brdb_value_sptr brdb_float_array = new brdb_value_t<bbas_1d_array_float_sptr>(tf_sptr);

    int frame = 0;
    boxm2_trajectory cameras(incline0(), incline1(), radius(), scene->bounding_box(), ni(), nj());
    boxm2_trajectory::iterator cam_iter;
    for (cam_iter = cameras.begin(); cam_iter != cameras.end(); cam_iter++, frame++)
    {
      brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(*cam_iter);

      //render scene
      vcl_vector<brdb_value_sptr> r_input;
      r_input.push_back(brdb_scene);
      r_input.push_back(brdb_cam);
      r_input.push_back(brdb_expimg);
      r_input.push_back(brdb_vis);
      r_input.push_back(brdb_data_type);
      r_input.push_back(brdb_mini);
      r_input.push_back(brdb_maxi);
      r_input.push_back(brdb_float_array);
      vcl_vector<brdb_value_sptr> output;
      expimg->fill(0);
      vis_img->fill(1.0f);
      gpu_pro->run(&gpu_render, r_input, output);

      //convert the image to bytes
      vil_image_view<unsigned int>* expimg_view = static_cast<vil_image_view<unsigned int>* >(expimg_sptr.ptr());
      vil_image_view<vxl_byte>* byte_img = new vil_image_view<vxl_byte>(ni(), nj());
      for (unsigned int i=0; i<ni(); ++i)
        for (unsigned int j=0; j<nj(); ++j)
          (*byte_img)(i,j) =  static_cast<vxl_byte>( (*expimg_view)(i,j) );   //just grab the first byte (all foura r the same)

      //write out the frame
      char filename[512];
      vcl_sprintf(filename, "%s/frame_%04d.png", dir().c_str(), frame);
      vil_save( *byte_img, filename );
    }
  }
#endif // 0

  return 0;
}
