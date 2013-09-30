//    Volume rendering sample
//    This sample loads a 3D volume

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
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>


int main(int argc, char ** argv)
{
  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
#ifdef WIN32
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = new char[13];
  vcl_strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
  delete[] my_argv[argc];
  delete[] my_argv;
#else
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);
#endif
    vul_arg<vcl_string> scene_file("-scene", "scene filename", vul_arg<vcl_string>::is_required);
    vul_arg<unsigned>   ni("-ni", "Width of output image", 1280);
    vul_arg<unsigned>   nj("-nj", "Height of output image", 720);
    vul_arg<vcl_string> imgdir("-imgdir", "image directory", "");
    vul_arg<vcl_string> camdir("-camdir", "camera directory", "");
    vul_arg<vcl_string> identifier("-ident", "identifier of the appearance data to be displayed, e.g. illum_bin_0", "");
    vul_arg<bool>       change("-change", "makes gui go into change detection mode - press n for next and p for previous", false);
    vul_arg<unsigned>   gpu_idx("-gpu_idx", "GPU index for multi GPU set up", 0);

    // need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);

    //make bocl manager
    bocl_manager_child_sptr mgr =bocl_manager_child::instance();
    if (gpu_idx() >= mgr->gpus_.size()){
      vcl_cout << "GPU index out of bounds" << vcl_endl;
      return -1;
    }
    bocl_device_sptr device = mgr->gpus_[gpu_idx()];
    vcl_cout << "Using: " << *device;
    boxm2_scene_sptr scene = new boxm2_scene(scene_file());

    //create initial cam
    double currInc = 45.0;
    double currRadius = scene->bounding_box().height(); //2.0f;
    double currAz = 0.0;
    vpgl_perspective_camera<double>* pcam;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni(), nj(),
                                        scene->bounding_box(), false);

    //create cache, grab singleton instance
    boxm2_lru_cache::create(scene);
    boxm2_opencl_cache_sptr opencl_cache=new boxm2_opencl_cache(scene, device, 4); //allow 4 blocks inthe cache

    //if image and camdir are valid and one to one, choose update or change
    if ( vul_file::is_directory(imgdir()) && vul_file::is_directory(camdir()) )
    {
      //populate the list of cams/ims
      vcl_vector<vcl_string> imgs = boxm2_util::images_from_directory(imgdir());
      vcl_vector<vcl_string> cams = boxm2_util::camfiles_from_directory(camdir());
      if (imgs.size() != cams.size()) {
        vcl_cout<<"num(cams) != num(imgs), returning!"<<vcl_endl;
        return -1;
      }
      
      //either in change detection or update mode
      if(change()) {
        //create a new ocl_draw_glbuffer_tableau, window, and initialize it
        boxm2_ocl_change_tableau_new bit_tableau;
        bit_tableau->init_change(device, opencl_cache, scene, ni(), nj(), pcam, imgs, cams);

        //create window, attach the new tableau and status bar
        vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer (Change Detection)");
        win->get_adaptor()->set_tableau(bit_tableau);
        bit_tableau->set_statusbar(win->get_statusbar());
        win->show();
      }
      else { //update mode
        //create a new ocl_draw_glbuffer_tableau, window, and initialize it
        boxm2_ocl_update_tableau_new bit_tableau;
        bit_tableau->init_update(device, opencl_cache, scene, ni(), nj(), pcam, imgs, cams);

        //create window, attach the new tableau and status bar
        vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer (Update)");
        win->get_adaptor()->set_tableau(bit_tableau);
        bit_tableau->set_statusbar(win->get_statusbar());
        win->show();
      }
    }
    //just render tableau
    else
    {
      //create a new ocl_draw_glbuffer_tableau, window, and initialize it
      boxm2_ocl_render_tableau_new bit_tableau;
      bit_tableau->init(device, opencl_cache, scene, ni(), nj(), pcam, identifier());

      //create window, attach the new tableau and status bar
      vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer (Render)");
      win->get_adaptor()->set_tableau(bit_tableau);
      bit_tableau->set_statusbar(win->get_statusbar());
      win->show();
    }

    //set vgui off
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    vcl_cout << "GLEW is supported= " << bGLEW << vcl_endl;
    return vgui::run();
}
