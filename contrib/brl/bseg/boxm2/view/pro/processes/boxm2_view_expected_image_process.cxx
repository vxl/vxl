// This is brl/bseg/boxm2/view/pro/processes/boxm2_view_expected_image_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011
#include <vnl/vnl_math.h>

#include <boxm2/view/boxm2_include_glew.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>

#include <boxm2/view/boxm2_ocl_render_tableau.h>
#include <boxm2/view/boxm2_view_utils.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_clear_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

namespace boxm2_view_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_view_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_expected_image_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned"; // ni
  input_types_[5] = "unsigned"; // nj


  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_view_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_view_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  return false;
#if 0
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);

  if (vpgl_perspective_camera<double> * pcam =dynamic_cast<vpgl_perspective_camera<double> * >(cam.ptr()))
  {
    unsigned ni=pro.get_input<unsigned>(i++);
    unsigned nj=pro.get_input<unsigned>(i++);
    //create a new ocl_draw_glbuffer_tableau, window, and initialize it
    boxm2_ocl_render_tableau_new bit_tableau;
    bit_tableau->init(device,opencl_cache,scene,ni,nj,pcam);
    int my_argc = 1;
    char** my_argv = new char*[1];
    my_argv[0] = new char[13];
    std::strcpy(my_argv[0], "--mfc-use-gl");
    vgui::init(my_argc, my_argv);

    //create window, attach the new tableau and status bar
    vgui_window* win = vgui::produce_window(ni, nj, "OpenCl Volume Visualizer");
    win->get_adaptor()->set_tableau(bit_tableau);
    bit_tableau->set_statusbar(win->get_statusbar());
    win->show();

    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0  GL_ARB_pixel_buffer_object");
    std::cout << "GLEW is supported= " << bGLEW << std::endl;
    device->context() = boxm2_view_utils::create_clgl_context(*(device->device_id()));

    return vgui::run();
  }
#endif // 0
}
