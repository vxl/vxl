//    Volume rendering sample
//    This sample loads a 4D volume

// Utilities, OpenCL and system includes
#include <GL/glew.h>
#include <bocl/bocl_cl.h>

//vil and camera includes
#include <vpgl/vpgl_perspective_camera.h>

//executable args
#include <vul/vul_arg.h>

//bstm scene stuff
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>

#include <boxm2/boxm2_util.h>

//vgui stuff
#include <bstm/view/bstm_ocl_render_tableau.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_easy3D_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

#include <vgui/vgui_slider_tableau.h>
#include <vgui/vgui_poly_tableau.h>

void slide_time( vgui_slider_tableau* tab, void* data )
{
  double* time = static_cast<double*>(data);
  (*time) = tab->value();
}


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
  std::strcpy(my_argv[argc], "--mfc-use-gl");
  vgui::init(my_argc, my_argv);
  delete[] my_argv[argc];
  delete[] my_argv;
#else
    //init vgui (should choose/determine toolkit)
    vgui::init(argc, argv);
#endif
    vul_arg<std::string> scene_file("-scene", "scene filename", "");
    vul_arg<unsigned>   ni("-ni", "Width of output image", 1280);
    vul_arg<unsigned>   nj("-nj", "Height of output image", 720);
    vul_arg<unsigned>   gpu_idx("-gpu_idx", "GPU index for multi GPU set up", 0);

    // need this on some toolkit implementations to get the window up.
    vul_arg_parse(argc, argv);

    //make bocl manager
    bocl_manager_child *mgr = &bocl_manager_child::instance();
    if (gpu_idx() >= mgr->gpus_.size()){
      std::cout << "GPU index out of bounds" << std::endl;
      return -1;
    }
    bocl_device_sptr device = mgr->gpus_[gpu_idx()];
    std::cout << "Using: " << *device;
    bstm_scene_sptr scene = new bstm_scene(scene_file());


    //create initial cam
    double currInc = 45.0;
    double currRadius = scene->bounding_box().height(); //2.0f;
    double currAz = 0.0;
    vpgl_perspective_camera<double>* pcam;
    pcam = boxm2_util::construct_camera(currInc, currAz, currRadius, ni(), nj(),
                                        scene->bounding_box(), false);


    //create cache, grab singleton instance
    bstm_lru_cache::create(scene);
    bstm_opencl_cache_sptr opencl_cache=new bstm_opencl_cache(scene, device);

    //create a new ocl_draw_glbuffer_tableau, window, and initialize it
    bstm_ocl_render_tableau_new bit_tableau;

    //create
    vgui_slider_tableau_new slider_h( vgui_slider_tableau::horiz );
    slider_h->add_motion_callback( slide_time, bit_tableau->time() );
    slider_h->set_value( 0.0f );

    bit_tableau->init(device, opencl_cache, scene, ni(), nj(), pcam, slider_h);

    // Fit the sliders and the easy2D into the window
    vgui_poly_tableau_new poly;
    poly->add( bit_tableau, 0.0f, 0.05f, 1.0f, 1.0f );
    int id = poly->add( slider_h, 0.0f, 0.0f, 1.0f, 0.05f );
    poly->set_outline_color(id,0,1,0);
    vgui_shell_tableau_new shell(poly);

    bit_tableau->shell_ = shell;

    //create window, attach the new tableau and status bar
    vgui_window* win = vgui::produce_window(ni(), nj(), "bstm OpenCl Volume Visualizer (Render)");
    win->get_adaptor()->set_tableau(shell);
    bit_tableau->set_statusbar(win->get_statusbar());
    win->show();

    //set vgui off
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    std::cout << "GLEW is supported= " << bGLEW << std::endl;
    return vgui::run();
}
