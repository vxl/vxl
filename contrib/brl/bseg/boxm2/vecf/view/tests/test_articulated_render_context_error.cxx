//:
// \file
// \author J.L. Mundy
// \date 4/10/15


#include <testlib/testlib_test.h>
#include <vcl_fstream.h>
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
#include "../boxm2_ocl_articulated_render_tableau.h"
#include <vul/vul_timer.h>
#include "../../boxm2_vecf_orbit_params.h"
#include "../../boxm2_vecf_eyelid.h"
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

typedef vnl_vector_fixed<unsigned char, 16> uchar16;

int main(int argc, char ** argv)
{
#ifdef WIN32
  argc = 1;
  argv = new char*[1];
  argv[0] = new char[13];
  vcl_strcpy(argv[0], "--mfc-use-gl");
  vgui::init(argc, argv);
#else
  return 0;
#endif
  vcl_string base_dir_path = "c:/Users/mundy/VisionSystems/Janus/experiments/vector_flow/";
  vcl_string orbit_scene_path = base_dir_path + "orbit/orbit.25.xml";
  vcl_string target_scene_path = base_dir_path + "orbit/target_orbit.25.xml";
  unsigned ni = 1280, nj = 720;
  unsigned device_id = 1;
  bocl_device_sptr  device( NULL );
  vcl_vector<bocl_device_sptr> devices;
  bocl_manager_child_sptr mgr =bocl_manager_child::instance();
  vcl_string device_name = "gpu";
  if(device_name=="gpu" || device_name=="")
    {
        vcl_vector<bocl_device_sptr> devices;
        for(unsigned i = 0; i < mgr->gpus_.size(); i++)
          devices.push_back( mgr->gpus_[i] );
        if(device_id >= devices.size()){
            vcl_cout << "GPU index out of bounds" << vcl_endl;
            return 0;
        }
        vcl_string device_ident = devices[device_id]->device_identifier();
        if(device_ident.find("NVIDIA") != vcl_string::npos)
          device = devices[device_id];
        else
          return 0;
    }
  
  vcl_cout << "Using: " << *device;
  boxm2_scene_sptr scene = new boxm2_scene(target_scene_path);
      //create initial cam
    double currInc = 45.0;
    double currRadius = scene->bounding_box().height(); //2.0f;
    double currAz = 0.0;
    vpgl_perspective_camera<double>* pcam;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni, nj,
                                        scene->bounding_box(), false);

    //create cache, grab singleton instance
    boxm2_lru_cache::create(scene);
    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(device); 

      //create a new ocl_draw_glbuffer_tableau, window, and initialize it
      boxm2_ocl_articulated_render_tableau_new bit_tableau;
      bit_tableau->init(device, opencl_cache, scene, ni, nj, pcam, "");

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

 
