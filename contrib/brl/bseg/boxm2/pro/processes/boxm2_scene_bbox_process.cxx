// This is brl/bseg/boxm2/pro/processes/boxm2_scene_bbox_process.cxx
//:
// \file
// \brief  A process for accessing scene illumination information, for now lat, lon and num_illumination bins
//
// \author Andrew Miller
// \date Dec 15, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>

namespace boxm2_scene_bbox_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 6;
}
bool boxm2_scene_bbox_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_bbox_process_globals;
  //process takes 1 input, the scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";

  // process has 3 outputs:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "double";// minx
  output_types_[1] = "double";// miny
  output_types_[2] = "double";// minz
  output_types_[3] = "double";// maxx
  output_types_[4] = "double";// maxy
  output_types_[5] = "double";// maxz
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_bbox_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_bbox_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  boxm2_scene_sptr   scene = pro.get_input<boxm2_scene_sptr>(0);

  //bbox and write out min/max pts
  vgl_box_3d<double> bbox = scene->bounding_box();

  // store outputs
  int i=0;
  pro.set_output_val<double>(i++, bbox.min_x());
  pro.set_output_val<double>(i++, bbox.min_y());
  pro.set_output_val<double>(i++, bbox.min_z());
  pro.set_output_val<double>(i++, bbox.max_x());
  pro.set_output_val<double>(i++, bbox.max_y());
  pro.set_output_val<double>(i++, bbox.max_z());
  return true;
}
