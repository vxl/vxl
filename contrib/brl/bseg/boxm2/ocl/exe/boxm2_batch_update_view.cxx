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

#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_process.h>
#include <boxm2/ocl/pro/boxm2_opencl_render_depth_process.h>

//brdb stuff
#include <brdb/brdb_value.h>

//vgui stuff
#include <boxm2/view/boxm2_batch_update_tableau.h>
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
    vul_arg<vcl_string> camdir("-cam", "camera directory", "");
    vul_arg<vcl_string> imgdir("-img", "image directory", "");
    vul_arg<unsigned> ni("-ni", "Width of output image", 1280);
    vul_arg<unsigned> nj("-nj", "Height of output image", 720);

    // need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);
    //create a new ocl_draw_glbuffer_tableau, window, and initialize it
    boxm2_batch_update_tableau_new bit_tableau;  
    bit_tableau->init(scene_file(),ni(),nj(), camdir(), imgdir());

    //create window, attach the new tableau and status bar
    vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
    win->get_adaptor()->set_tableau(bit_tableau); 
    bit_tableau->set_statusbar(win->get_statusbar());
    win->show();
   
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    vcl_cout << "GLEW is supported= " << bGLEW << vcl_endl;
    return vgui::run();
}
