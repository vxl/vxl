//    Volume rendering of an articulated model
//
//    The scene must have a companion articulation class with iterator
//    an iterator that emits a sequence of scene parameters that
//    constitute the articulation of the model.  The articulation
//    is initiatied/paused by pressing the 'v' key in a toggle mode
//
#include <vcl_fstream.h>
// Utilities, OpenCL and system includes
#include <GL/glew.h>
#include <bocl/bocl_cl.h>

//vil and camera includes
#include <vpgl/vpgl_perspective_camera.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>

//vgui stuff
#include <boxm2/view/boxm2_ocl_render_tableau.h>
#include <boxm2/view/boxm2_ocl_update_tableau.h>
#include <boxm2/view/boxm2_ocl_change_tableau.h>
#include <boxm2/view/boxm2_view_utils.h>
#include <boxm2/boxm2_util.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include "../../boxm2_vecf_orbit_scene.h"
#include "../../boxm2_vecf_composite_head_model.h"
#include "../../ocl/boxm2_vecf_ocl_composite_head_model.h"
#include "../../boxm2_vecf_orbit_articulation.h"
#include "../../boxm2_vecf_composite_head_model_articulation.h"
#include "../boxm2_ocl_articulated_render_tableau.h"

int main(int argc, char ** argv)
{
#ifdef WIN32
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = new char[13];
  vcl_strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
#else
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);
#endif
  vul_arg_info_list arglist;
  vul_arg<vcl_string> base_dir_path_arg(arglist, "-bdir", "Base model directory", "");
  vul_arg<vcl_string> model_path_arg(arglist, "-model", "model_xml_file", "");
  vul_arg<vcl_string> target_path_arg(arglist, "-target", "target_xml_file", "");
  vul_arg<vcl_string> camera_path_arg(arglist, "-cam", "default camera", "");
  vul_arg<vcl_string> background_arg(arglist, "-bkgnd", "dark background", "true");
  vul_arg<vcl_string> scene_arg(arglist, "-scene_t", " which scene ", "eye");
  vul_arg<unsigned> gpu_idx(arglist, "-platform", " platform index ", 0);

  arglist.parse(argc, argv, false);
  vcl_string base_dir_path = base_dir_path_arg();
  vcl_string model_path = model_path_arg();
  vcl_string target_path = target_path_arg();
  vcl_string cam_path = camera_path_arg();
  bool dark_background = background_arg()=="true";
  vcl_string scene_t = scene_arg();
  unsigned device_id = gpu_idx();

  // check if base directory exists
  bool good = vul_file::exists(base_dir_path);
  if(!good){
    vcl_cout << base_dir_path << " is not valid\n";
    return -1;
  }
  vcl_string articulated_scene_path = base_dir_path + model_path;
  vcl_string target_scene_path = base_dir_path + target_path;
  vcl_string eye_model_path = base_dir_path + "eye/eye.xml";
  vcl_string default_cam_path = base_dir_path + cam_path;
  // check for valid file paths
  good = vul_file::exists(articulated_scene_path);
  if(!good){
    vcl_cout << articulated_scene_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(target_scene_path);
  if(!good){
    vcl_cout << target_scene_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(default_cam_path);
  if(!good){
    vcl_cout << default_cam_path << " is not valid\n";
    return -1;
  }
  unsigned ni = 1280, nj = 720;
  vcl_string device_name = "gpu";
    bocl_device_sptr  device( NULL );
    //make bocl manager
    bocl_manager_child &mgr =bocl_manager_child::instance();
    if(device_name=="gpu" || device_name =="")
    {
        vcl_vector<bocl_device_sptr> devices;
        for(unsigned i = 0; i < mgr.gpus_.size(); i++)
          devices.push_back( mgr.gpus_[i] );
        if(device_id >= devices.size()){
            vcl_cout << "GPU index out of bounds" << vcl_endl;
            return -1;
        }
        vcl_string device_ident = devices[device_id]->device_identifier();
        if(device_ident.find("NVIDIA") != vcl_string::npos)
          device = devices[device_id];
        else
          return -1;
    }
    vcl_cout << "Using: " << *device;

    boxm2_scene_sptr target_scene = new boxm2_scene(target_scene_path);


    //create initial cam
#if 0
    double currInc = 0.0;//45.0
    double currRadius = 3.0*target_scene->bounding_box().height(); //2.0f;
    double currAz = 180.0;
    vpgl_perspective_camera<double>* pcam;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj,
                                        target_scene->bounding_box(), false);
#endif
    vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>();
    vcl_ifstream istr(default_cam_path.c_str());
    if(!istr.is_open()){
      vcl_cout << default_cam_path << " is not a valid camera path\n";
      return -1;
    }
    istr >> *pcam;
    istr.close();
    //create cache, grab singleton instance
    //boxm2_lru_cache::create(orbit_scene);
    boxm2_lru_cache::create(target_scene);
    //boxm2_cache::instance()->add_scene(target_scene);

    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(device);

      //create a new ocl_draw_glbuffer_tableau, window, and initialize it
    boxm2_ocl_articulated_render_tableau_new bit_tableau;

    if(scene_t == "eye"){

      boxm2_vecf_orbit_scene* orbit_scene = new boxm2_vecf_orbit_scene(articulated_scene_path, true,true);
      boxm2_vecf_orbit_articulation* orbit_articulation =new boxm2_vecf_orbit_articulation();
      orbit_articulation->set_play_sequence("default");
      orbit_scene->set_target_background(dark_background);
      boxm2_scene_sptr pula = orbit_scene->scene();
      //      boxm2_lru_cache::instance()->write_to_disk(pula);
      bit_tableau->init(device, opencl_cache, orbit_scene, orbit_articulation,target_scene, ni, nj, pcam, "");

    }else if (scene_t =="head"){

      boxm2_vecf_composite_head_model* composite_head_model = new boxm2_vecf_composite_head_model(articulated_scene_path, eye_model_path);
      //      boxm2_vecf_ocl_composite_head_model* composite_head_model = new boxm2_vecf_ocl_composite_head_model(articulated_scene_path, eye_model_path,device,opencl_cache);
      boxm2_vecf_composite_head_model_articulation* head_model_articulation =new boxm2_vecf_composite_head_model_articulation();
      head_model_articulation->set_play_sequence("eyelid_open_and_close");
    vgl_vector_3d<double> look_dir(0.0, 0.0, 1.0);
    vgl_vector_3d<double> face_scale(1,1,1);
    //initial parameters
    boxm2_vecf_composite_head_parameters params( face_scale, look_dir );

    params.l_orbit_params_.eye_pointing_dir_ = params.look_dir_;
    params.r_orbit_params_.eye_pointing_dir_ = params.look_dir_;

    params.r_orbit_params_.eyelid_dt_ = 0.1 ;
    params.l_orbit_params_.eyelid_dt_ = 0.1 ;

    composite_head_model->set_params( params );
    composite_head_model->map_to_target(target_scene);

      bit_tableau->init(device, opencl_cache, composite_head_model, head_model_articulation,target_scene, ni, nj, pcam, "");
    }
      //create window, attach the new tableau and status bar
      vgui_window* win = vgui::produce_window(ni, nj, "OpenCl Volume Visualizer (Render)");
      win->get_adaptor()->set_tableau(bit_tableau);
      bit_tableau->set_statusbar(win->get_statusbar());
      win->show();


    //set vgui off
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    vcl_cout << "GLEW is supported= " << bGLEW << vcl_endl;

    return vgui::run();
}
