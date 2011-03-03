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

#include <vnl/vnl_random.h>

//boxm2 scene stuff
#include <boxm2/boxm2_util.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_process.h>
#include <boxm2/ocl/pro/boxm2_opencl_update_process.h>
#include <boxm2/ocl/pro/boxm2_opencl_refine_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
    vcl_cout<<"Updating and rendering trajectory "<<vcl_endl;
    vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
    vul_arg<vcl_string> dir("-dir", "output image directory", "");
    vul_arg<vcl_string> camdir("-cam", "camera directory", "");
    vul_arg<vcl_string> imgdir("-img", "image directory", "");
    vul_arg<unsigned> ni("-ni", "Width of output image", 1024);
    vul_arg<unsigned> nj("-nj", "Height of output image", 768);
    vul_arg<double> incline0("-init_incline", "Initial angle of incline (degrees) away from zenith", 25.0); 
    vul_arg<double> incline1("-end_incline", "Final angle of incline (degrees) away from zenith", 55.0); 
    vul_arg<double> radius("-radius", "Distance from center of bounding box", 5.0);
    vul_arg<bool> bit8("-bit8", "True for 8 bit and false for 16 bit", true); 
    vul_arg<bool> save_model("-save", "True for 8 bit and false for 16 bit", true); 
    vul_arg_parse(argc, argv);
    
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

    //store image filenames and cams
    vcl_vector<vpgl_perspective_camera<double>* > cams = boxm2_util::cameras_from_directory(camdir()); 
    vcl_vector<vcl_string> imgs = boxm2_util::images_from_directory(imgdir());
    vcl_vector<vcl_string> camfiles = boxm2_util::camfiles_from_directory(camdir());

    //----------------------------------------------------------------------------
    //--- setup scene and processors------------------------------------------------
    //----------------------------------------------------------------------------
    //start out rendering with the CPU
    boxm2_scene_sptr scene = new boxm2_scene(scene_file());
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene);

    //initialize a block and data cache
    boxm2_lru_cache cache( scene.ptr() );

    //initialize gpu pro / manager
    boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
    gpu_pro->set_scene(scene.ptr());
    gpu_pro->set_cpu_cache(&cache);
    gpu_pro->init();
    
    //initialize the GPU render process
    boxm2_opencl_render_process gpu_render;
    vcl_string opts = (!bit8()) ? " -D MOG_TYPE_16" : " -D MOG_TYPE_8"; 
    gpu_render.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], opts);

    ////initialize GPU update process
    boxm2_opencl_update_process gpu_update;
    gpu_update.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], opts); 
    
    //initialize GPU refine process
    boxm2_opencl_refine_process gpu_refine;
    gpu_refine.init_kernel(&gpu_pro->context(), &gpu_pro->devices()[0], opts); 
  
    //create output image buffer (for rendering)
    vil_image_view<unsigned int>* expimg = new vil_image_view<unsigned int>(ni(), nj());
    expimg->fill(0);
    vil_image_view_base_sptr expimg_sptr(expimg); 
    brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg_sptr);

    //create vis image buffer (for rendering)
    vil_image_view<float>* vis_img = new vil_image_view<float>(ni(), nj());
    vis_img->fill(1.0f);
    brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img);

    brdb_value_sptr brdb_data_type;
    if(!bit8())
        brdb_data_type= new brdb_value_t<vcl_string>(vcl_string("16bit"));
    else
        brdb_data_type= new brdb_value_t<vcl_string>(vcl_string("8bit"));
 
    //----------------------------------------------------------------------------
    //--- LOOP OVER TRAJECTORY scene and processors------------------------------------------------
    //----------------------------------------------------------------------------
    //introduce a trajectory
    vnl_random rand; 
    int frame = 0; 
    boxm2_trajectory cameras(incline0(), incline1(), radius(), scene->bounding_box(), ni(), nj()); 
    boxm2_trajectory::iterator cam_iter; 
    for(cam_iter = cameras.begin(); cam_iter != cameras.end(); cam_iter++, frame++)
    {
      
      ////////////////////////////////////////////////////////////////////////
      //Update 
      int curr_frame = rand.lrand32(0,cams.size()-1);
      
      //build the camera from file
      vcl_ifstream ifs(camfiles[curr_frame].c_str());
      vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
      if (!ifs.is_open()) {
          vcl_cerr << "Failed to open file " << camfiles[curr_frame] << '\n';
          return -1;
      }
      ifs >> *pcam;
      vpgl_camera_double_sptr cam_sptr(pcam);
      brdb_value_sptr brdb_update_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam_sptr);

      
      //load image from file
      vil_image_view_base_sptr loaded_image = vil_load(imgs[curr_frame].c_str());
      vil_image_view<float>* floatimg = new vil_image_view<float>(loaded_image->ni(), loaded_image->nj(), 1);
      if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
      {
        vil_convert_stretch_range_limited(*img_byte, *floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
      }
      else {
        vcl_cerr << "Failed to load image " << imgs[curr_frame] << vcl_endl;
        return -1;
      }
          
      //create input image buffer
      vil_image_view_base_sptr floatimg_sptr(floatimg);// = new vil_image_view<unsigned int>(ni(), nj());
      brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(floatimg_sptr);
      
      vcl_vector<brdb_value_sptr> input;
      input.push_back(brdb_scene);
      input.push_back(brdb_update_cam);
      input.push_back(brdb_inimg);
      input.push_back(brdb_data_type); 
      
      //initoutput vector
      vcl_vector<brdb_value_sptr> output;
      gpu_pro->run(&gpu_update, input, output); 

      
      ////////////////////////////////////////////////////////////////////////
      ////Refine if it's every 5
      //if( frame%5==0 ) {
        //vcl_cout<<"refine being run"<<vcl_endl;
        ////set inputs
        //vcl_vector<brdb_value_sptr> refine_inputs;
        //refine_inputs.push_back(brdb_scene);
        //refine_inputs.push_back(brdb_data_type);
      
        ////initoutput vector
        //vcl_vector<brdb_value_sptr> refine_output;

        ////execute gpu_update
        //gpu_pro->run(&gpu_refine, refine_inputs, refine_output);
      //}
      ////////////////////////////////////////////////////////////////////////
      
      //////////////////////////////////////////////////////////////////////////////
      // Render image from trajectory 
      brdb_value_sptr brdb_cam_render = new brdb_value_t<vpgl_camera_double_sptr>(*cam_iter);
      vcl_vector<brdb_value_sptr> r_input;
      r_input.push_back(brdb_scene);
      r_input.push_back(brdb_cam_render);
      r_input.push_back(brdb_expimg);
      r_input.push_back(brdb_vis);
      r_input.push_back(brdb_data_type);
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
      delete byte_img;   
        
    }
    
    gpu_update.clean(); 
    gpu_render.clean();
    gpu_pro->finish();

    //print out cache for posterities sake
    vcl_cout<<cache<<vcl_endl;
    
    if(save_model())
    {
      //save blocks and data to disk for debugging
      vcl_map<boxm2_block_id, boxm2_block_metadata> blocks = scene->blocks();
      vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter;
      for(iter = blocks.begin(); iter != blocks.end(); ++iter)
      { 
        boxm2_block_id id = iter->first; 
        boxm2_sio_mgr::save_block(scene->data_path(), cache.get_block(id)); 
        boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_ALPHA>(id) );
        boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_MOG3_GREY>(id) );
        boxm2_sio_mgr::save_block_data(scene->data_path(), id, cache.get_data<BOXM2_NUM_OBS>(id) );
      }
    }
    

  return 0;
}
