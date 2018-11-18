//This is brl/bpro/core/bbas_pro/processes/bpgl_depth_map_processes.cxx
//:
// \file
#include <string>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>
#include <bpgl/depth_map/depth_map_scene.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//:sets input and output types
bool bpgl_generate_depth_maps_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(4);
  input_types_[0] = "vcl_string"; // the path to the binary stream for depth_scene object
  input_types_[1] = "unsigned";   // down sampling level to generate the depth maps
  input_types_[2] = "vcl_string"; // output folder
  input_types_[3] = "vcl_string"; // name prefix for output depth maps

  if (!pro.set_input_types(input_types_))
    return false;
  //output
  std::vector<std::string> output_types_(0);
  return pro.set_output_types(output_types_);
}

bool bpgl_generate_depth_maps_process(bprb_func_process& pro)
{

 //check number of inputs
  if(!pro.verify_inputs())
  {
    std::cout << pro.name() << " Invalid inputs " << std::endl;
    return false;
  }

  //get the inputs
  std::string filename = pro.get_input<std::string>(0);
  auto level = pro.get_input<unsigned>(1);
  std::string output_folder = pro.get_input<std::string>(2);
  std::string name_prefix = pro.get_input<std::string>(3);

  depth_map_scene scene;
  vsl_b_ifstream is(filename.c_str());
  if (!is) {
    std::cout << "invalid binary stream for path " << filename << std::endl;
    return false;
  }
  scene.b_read(is);

  unsigned cnt = 0;
  for (scene_depth_iterator iter = scene.begin(); iter != scene.end(); ++iter) {
    vil_image_view<float> depth_map = scene.depth_map(level);
    std::stringstream ss; ss << cnt++;
    std::string name = output_folder + "/" + name_prefix + "_" + ss.str() + ".tif";
    vil_save(depth_map, name.c_str());
  }

  return true;
}
