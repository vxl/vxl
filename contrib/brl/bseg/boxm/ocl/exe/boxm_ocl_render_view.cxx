
//    Volume rendering sample

//    This sample loads a 3D volume


// Utilities, OpenCL and system includes
#include <GL/glew.h>

#include <bocl/bocl_cl.h>
#include <bocl/bocl_utils.h>
#include <boxm/ocl/boxm_ocl_scene.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>

#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau.h>
#include <boxm/ocl/view/boxm_ocl_draw_glbuffer_tableau_sptr.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

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
    boxm_ocl_scene ocl_scene(scene_file());

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
    vcl_cout<<"I'm in boxm_ocl_render_view.cxx"<<vcl_endl;
    boxm_ocl_draw_glbuffer_tableau_new glbuffer_tableau;  
    //vgui_window* win = vgui::produce_window(ni(), nj(), "OpenCl Volume Visualizer");
    //win->get_adaptor()->make_current();
    //win->get_adaptor()->set_tableau( glbuffer_tableau  ); 
    vcl_cout.flush();
   
    glbuffer_tableau->init(&ocl_scene,ni(),nj(),pcam);
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    return  vgui::run(glbuffer_tableau, ni(), nj());


    // Load image (given in the first command line param) into an image tableau.
    //vgui_image_tableau_new image("/home/acm/Pictures/Flowerings_90_by_love1008.jpg");

    //// Put the image tableau inside a 2D viewer tableau (for zoom, etc).
    //vgui_viewer2D_tableau_new viewer(image);

    ////// Put a shell tableau at the top of our tableau tree.
    //vgui_shell_tableau_new shell(viewer);

    ////vgui_window* win = vgui::adapt(shell, image->width(), image->height(), "OpenCl Volume Visualizer");
    ////win->get_adaptor()->set_tableau( shell  ); 
    ////win->get_adaptor()->make_current();

    //// Create a window, add the tableau and show it on screen.
    //return vgui::run(shell, image->width(), image->height());


}
