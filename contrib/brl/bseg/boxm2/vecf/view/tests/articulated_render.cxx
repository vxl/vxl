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

  vcl_string base_dir_path = "/home/imagedata/janus/models/composite/";
  //vcl_string orbit_scene_path = base_dir_path + "orbit/orbit.25.xml";
  //vcl_string target_scene_path = base_dir_path + "orbit/target_orbit.25.xml";
  vcl_string orbit_scene_path = base_dir_path + "orbit/orbit.xml";
  vcl_string target_scene_path = base_dir_path + "target_orbit/target_orbit.xml";
  vcl_string default_cam_path = base_dir_path + "orbit/default_orbit_cam.txt";
  unsigned ni = 1280, nj = 720;
  unsigned device_id = 1;
  vcl_string device_name = "gpu";
    bocl_device_sptr  device( NULL );
    //make bocl manager
    bocl_manager_child_sptr mgr =bocl_manager_child::instance();
    if(device_name=="gpu" || device_name =="")
    {
        vcl_vector<bocl_device_sptr> devices;
        for(unsigned i = 0; i < mgr->gpus_.size(); i++)
          devices.push_back( mgr->gpus_[i] );
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
    boxm2_vecf_orbit_scene* orbit_scene = new boxm2_vecf_orbit_scene(orbit_scene_path, true);
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
      bit_tableau->init(device, opencl_cache, orbit_scene, target_scene, ni, nj, pcam, "");
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
