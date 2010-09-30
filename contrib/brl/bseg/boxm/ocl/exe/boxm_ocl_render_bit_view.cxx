
//    Volume rendering sample

//    This sample loads a 3D volume


// Utilities, OpenCL and system includes
#include <GL/glew.h>

#include <bocl/bocl_cl.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>
#include <boxm/ocl/boxm_render_bit_scene_manager.h>

#include <boxm/ocl/view/boxm_ocl_render_bit_tableau.h>
#include <boxm/ocl/view/boxm_ocl_render_bit_tableau_sptr.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>

#include <vpgl/vpgl_perspective_camera.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

int main(int argc, char ** argv)
{
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);

    vul_arg<vcl_string> camfile("-cam", "camera filename", "");
    vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
    vul_arg<unsigned> ni("-ni", "Width of image", 640);
    vul_arg<unsigned> nj("-nj", "Height of image", 480);

    //// need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);

    //create ocl_scene from xml file 
    boxm_ocl_bit_scene ocl_scene(scene_file());
    vcl_cout<<"Scene Initialized... "<<vcl_endl
            <<ocl_scene<<vcl_endl;

    //load camera from file
    vcl_ifstream ifs(camfile().c_str());
    vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
    if (!ifs.is_open()) {
        vcl_cerr << "Failed to open file " << camfile() << vcl_endl;
        return -1;
    }
    else  {
        ifs >> *pcam;
    }

    //create a new ocl_draw_glbuffer_tableau, window, and initialize it
    boxm_ocl_render_bit_tableau_new bit_tableau;  
    bit_tableau->init(&ocl_scene,ni(),nj(),pcam);

    //create window, attach the new tableau and status bar
    vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
    win->get_adaptor()->set_tableau(bit_tableau); 
    bit_tableau->set_statusbar(win->get_statusbar());
    win->show();
   
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    vcl_cout << "GLEW is supported= " << bGLEW << vcl_endl;
    return vgui::run();
}
