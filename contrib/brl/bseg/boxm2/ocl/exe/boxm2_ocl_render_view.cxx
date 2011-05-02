//    Volume rendering sample
//    This sample loads a 3D volume

// Utilities, OpenCL and system includes
#include <GL/glew.h>
#include <bocl/bocl_cl.h>

//vil and camera includes
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
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>


//vgui stuff
#include <boxm2/view/boxm2_ocl_render_tableau.h>
#include <boxm2/view/boxm2_ocl_update_tableau.h>
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
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);
    vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
    vul_arg<unsigned> ni("-ni", "Width of output image", 1280);
    vul_arg<unsigned> nj("-nj", "Height of output image", 720);
    vul_arg<vcl_string> imgdir("-imgdir", "image directory", ""); 
    vul_arg<vcl_string> camdir("-camdir", "camera directory", ""); 

    // need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);

    //make bocl manager
    bocl_manager_child_sptr mgr =bocl_manager_child::instance();
    bocl_device_sptr device = mgr->gpus_[0];
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

    //choose the update tableau or the render tableau....
    if ( vul_file::is_directory(imgdir()) && vul_file::is_directory(camdir()) )
    {
      //populate the list of cams/ims
      vcl_vector<vcl_string> imgs = boxm2_util::images_from_directory(imgdir());
      vcl_vector<vcl_string> cams = boxm2_util::camfiles_from_directory(camdir());
      if(imgs.size() != cams.size()) {
        vcl_cout<<"num(cams) != num(imgs), returning!"<<vcl_endl;
        return -1; 
      }
      
      //create a new ocl_draw_glbuffer_tableau, window, and initialize it
      boxm2_ocl_update_tableau_new bit_tableau;  
      bit_tableau->init_update(device, opencl_cache, scene, ni(), nj(), pcam, imgs, cams); 

      //create window, attach the new tableau and status bar
      vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
      win->get_adaptor()->set_tableau(bit_tableau); 
      bit_tableau->set_statusbar(win->get_statusbar());
      win->show();
    }
    else 
    {
      //create a new ocl_draw_glbuffer_tableau, window, and initialize it
      boxm2_ocl_render_tableau_new bit_tableau;  
      bit_tableau->init(device, opencl_cache, scene, ni(), nj(), pcam); 

      //create window, attach the new tableau and status bar
      vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
      win->get_adaptor()->set_tableau(bit_tableau); 
      bit_tableau->set_statusbar(win->get_statusbar());
      win->show();
    }

    //set vgui off
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    vcl_cout << "GLEW is supported= " << bGLEW << vcl_endl;
    return vgui::run();
}
