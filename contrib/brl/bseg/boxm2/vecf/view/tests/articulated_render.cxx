//    Volume rendering of an articulated model
//
//    The scene must have a companion articulation class with iterator
//    an iterator that emits a sequence of scene parameters that
//    constitute the articulation of the model.  The articulation
//    is initiatied/paused by pressing the 'v' key in a toggle mode
//
#include <iostream>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
#include "../../boxm2_vecf_mandible_scene.h"
#include "../../boxm2_vecf_cranium_scene.h"
#include "../../boxm2_vecf_skull_scene.h"
#include "../../boxm2_vecf_composite_face_scene.h"
#include "../../boxm2_vecf_composite_head_model.h"
#include "../../ocl/boxm2_vecf_ocl_composite_head_model.h"
#include "../../boxm2_vecf_orbit_articulation.h"
#include "../../boxm2_vecf_mandible_articulation.h"
#include "../../boxm2_vecf_cranium_articulation.h"
#include "../../boxm2_vecf_skull_articulation.h"
#include "../../boxm2_vecf_composite_face_articulation.h"
#include "../../boxm2_vecf_composite_head_model_articulation.h"
#include "../../boxm2_vecf_middle_fat_pocket_scene.h"
#include "../../boxm2_vecf_middle_fat_pocket_articulation.h"
#include "../boxm2_ocl_articulated_render_tableau.h"

int main(int argc, char ** argv)
{
#ifdef WIN32
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = new char[13];
  std::strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
#else
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);
#endif
  vul_arg_info_list arglist;

  vul_arg<std::string> base_dir_path_arg(arglist, "-bdir", "Base model directory", "");
  vul_arg<std::string> model_path_arg(arglist, "-model", "model_xml_file", "");
  vul_arg<std::string> target_path_arg(arglist, "-target", "target_xml_file", "");
  vul_arg<std::string> geo_path_arg(arglist, "-geo", "geometry_data_file", "");
  vul_arg<std::string> camera_path_arg(arglist, "-cam", "default camera", "");
  vul_arg<std::string> background_arg(arglist, "-bkgnd", "dark background", "true");
  vul_arg<std::string> scene_arg(arglist, "-scene_t", " which scene ", "eye");
  vul_arg<unsigned> gpu_idx(arglist, "-platform", " platform index ", 0);

  arglist.parse(argc, argv, false);
  std::string base_dir_path = base_dir_path_arg();
  std::string model_path = model_path_arg();
  std::string target_path = target_path_arg();
  std::string geo_path = geo_path_arg();
  std::string cam_path = camera_path_arg();
  bool dark_background = background_arg()=="true";
  std::string scene_t = scene_arg();
  unsigned device_id = gpu_idx();

  // check if base directory exists
  bool good = vul_file::exists(base_dir_path);
  if(!good){
    std::cout << base_dir_path << " is not valid\n";
    return -1;
  }
  std::string target_base_dir = "";//may be defined in the future
  std::string articulated_scene_path = base_dir_path + model_path;
  std::string target_scene_path = base_dir_path + target_path;
  if(target_base_dir == "")
    target_scene_path = base_dir_path + target_path;
  else
    target_scene_path = target_base_dir + target_path;
  std::string geometry_path = base_dir_path + geo_path;
  std::string eye_model_path = base_dir_path + "eye/eye.xml";
  std::string default_cam_path = base_dir_path + cam_path;
  // check for valid file paths
  good = vul_file::exists(articulated_scene_path);
  if(!good){
    std::cout << articulated_scene_path << " is not valid\n";
    return -1;
  }
  good = vul_file::exists(target_scene_path);
  if(!good){
    std::cout << target_scene_path << " is not valid\n";
    return -1;
  }

  good = (geo_path != "") || vul_file::exists(geometry_path);
  if(!good){
    std::cout << geometry_path << " is not valid\n";
    return -1;
  }

  bool cam_path_exists = vul_file::exists(default_cam_path)&&cam_path!="";

  unsigned ni = 1280, nj = 720;
  std::string device_name = "gpu";
    bocl_device_sptr  device( NULL );
    //make bocl manager
    bocl_manager_child &mgr =bocl_manager_child::instance();
    if(device_name=="gpu" || device_name =="")
    {
        std::vector<bocl_device_sptr> devices;
        for(unsigned i = 0; i < mgr.gpus_.size(); i++)
          devices.push_back( mgr.gpus_[i] );
        if(device_id >= devices.size()){
            std::cout << "GPU index out of bounds" << std::endl;
            return -1;
        }
        std::string device_ident = devices[device_id]->device_identifier();
        if(device_ident.find("NVIDIA") != std::string::npos)
          device = devices[device_id];
        else
          return -1;
    }
    std::cout << "Using: " << *device;

    boxm2_scene_sptr target_scene = new boxm2_scene(target_scene_path);


    //create initial cam
    vpgl_perspective_camera<double>* pcam = 0;
    if(!cam_path_exists){
    double currInc = 0.0;//45.0
    double currRadius = 3.0*target_scene->bounding_box().height(); //2.0f;
    double currAz = 180.0;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj,
                                        target_scene->bounding_box(), false);
    }else{
    pcam = new vpgl_perspective_camera<double>();
    std::ifstream istr(default_cam_path.c_str());
    if(!istr.is_open()){
      std::cout << default_cam_path << " is not a valid camera path\n";
      return -1;
    }
    istr >> *pcam;
    istr.close();
    }
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
    }else if(scene_t == "mandible"||scene_t == "mandible_f"){
      boxm2_vecf_mandible_scene* mandible_scene= 0;
      if(scene_t == "mandible_f")
           mandible_scene = new boxm2_vecf_mandible_scene(articulated_scene_path);
      else
        mandible_scene = new boxm2_vecf_mandible_scene(articulated_scene_path, geometry_path);

      boxm2_vecf_mandible_articulation* mandible_articulation =new boxm2_vecf_mandible_articulation();
      mandible_articulation->set_play_sequence("default");
      mandible_scene->set_target_background(dark_background);
      //boxm2_scene_sptr pula = mandible_scene->scene();
      //      boxm2_lru_cache::instance()->write_to_disk(pula);
      bit_tableau->init(device, opencl_cache, mandible_scene, mandible_articulation,target_scene, ni, nj, pcam, "");
    }else if(scene_t == "cranium"||scene_t == "cranium_f"){

      boxm2_vecf_cranium_scene* cranium_scene= 0;
      if(scene_t == "cranium_f")
           cranium_scene = new boxm2_vecf_cranium_scene(articulated_scene_path);
      else
       cranium_scene = new boxm2_vecf_cranium_scene(articulated_scene_path, geometry_path);

      boxm2_vecf_cranium_articulation* cranium_articulation =new boxm2_vecf_cranium_articulation();
      cranium_articulation->set_play_sequence("default");
      cranium_scene->set_target_background(dark_background);
          if(scene_t=="cranium"){
        boxm2_scene_sptr crscn = cranium_scene->scene();
        boxm2_lru_cache::instance()->write_to_disk(crscn);
          }
      bit_tableau->init(device, opencl_cache, cranium_scene, cranium_articulation,target_scene, ni, nj, pcam, "");

    }else if(scene_t == "skull"){
      boxm2_vecf_skull_scene* skull_scene = new boxm2_vecf_skull_scene(base_dir_path, geometry_path);
      boxm2_vecf_skull_articulation* skull_articulation =new boxm2_vecf_skull_articulation();
      skull_articulation->set_play_sequence("default");
      skull_scene->set_target_background(dark_background);
      //boxm2_scene_sptr crscn = skull_scene->scene();
      //boxm2_lru_cache::instance()->write_to_disk(crscn);
      bit_tableau->init(device, opencl_cache, skull_scene, skull_articulation,target_scene, ni, nj, pcam, "");
    }else if(scene_t == "composite_face"){
      boxm2_vecf_composite_face_scene* composite_face_scene = new boxm2_vecf_composite_face_scene(articulated_scene_path);
      // this constructor for the articulation passes in the global transformation between source and target
      // needed to render the face of individual subjects
      boxm2_vecf_composite_face_articulation* composite_face_articulation =
        new boxm2_vecf_composite_face_articulation(composite_face_scene->params());
      composite_face_articulation->set_play_sequence("default");
      composite_face_scene->set_target_background(dark_background);
      //boxm2_scene_sptr crscn = skull_scene->scene();
      //boxm2_lru_cache::instance()->write_to_disk(crscn);
      bit_tableau->init(device, opencl_cache, composite_face_scene, composite_face_articulation,target_scene, ni, nj, pcam, "");
    }else if(scene_t == "fat_pocket"){
      // assumes source scene exists, i.e. initialize is false
      bool initialize = false;
      boxm2_vecf_middle_fat_pocket_scene* fat_pocket_scene = new boxm2_vecf_middle_fat_pocket_scene(articulated_scene_path, geometry_path, initialize);
      boxm2_vecf_middle_fat_pocket_articulation* fat_pocket_articulation =new boxm2_vecf_middle_fat_pocket_articulation();
      fat_pocket_articulation->set_play_sequence("default");
      fat_pocket_scene->set_target_background(dark_background);
      //boxm2_scene_sptr crscn = skull_scene->scene();
      //boxm2_lru_cache::instance()->write_to_disk(crscn);
      bit_tableau->init(device, opencl_cache, fat_pocket_scene, fat_pocket_articulation,target_scene, ni, nj, pcam, "");
    }
      //create window, attach the new tableau and status bar
      vgui_window* win = vgui::produce_window(ni, nj, "OpenCl Volume Visualizer (Render)");
      win->get_adaptor()->set_tableau(bit_tableau);
      bit_tableau->set_statusbar(win->get_statusbar());
      win->show();


    //set vgui off
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    std::cout << "GLEW is supported= " << bGLEW << std::endl;

    return vgui::run();
}
