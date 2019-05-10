// This is brl/bseg/bvxm/algo/pro/processes/bvxm_create_scene_xml_process.h
#ifndef bvxm_create_scene_xml_process_h_
#define bvxm_create_scene_xml_process_h_
//:
// \file
// \brief A class for generating bvxm scene xml, given series of scene parameters
//
// \author Yi Dong
// \date   Jun, 2015
// \verbatim
// Modifications
//    July, 2015, Yi Dong, add a process to generate quad-tree structured scenes for a large region
//    Aug,  2018, Yi Dong, add extension on the input ROI to enlarge the region
//                         also returns scenes that cover the bounding box of input ROI
// \endverbatim
#include <bprb/bprb_func_process.h>
#include <vgl_box_2d.h>

// generate a bvxm scene xml
//: global variables
namespace bvxm_create_scene_xml_process_globals
{
  constexpr unsigned n_inputs_ = 14;
  constexpr unsigned n_outputs_ = 0;
}
//: set input and output types
bool bvxm_create_scene_xml_process_cons(bprb_func_process& pro);
//: generate a bvxm scene xml
bool bvxm_create_scene_xml_process(bprb_func_process& pro);

//: generate scenes that are arranged by a quad-tree structure to cover a large scale region.
//: global variables
namespace bvxm_create_scene_xml_large_scale_process_globals
{
  constexpr unsigned n_inputs_  = 10;
  constexpr unsigned n_outputs_ = 1;
}
//: set input and output types
bool bvxm_create_scene_xml_large_scale_process_cons(bprb_func_process& pro);
//: generate scene xmls
bool bvxm_create_scene_xml_large_scale_process(bprb_func_process& pro);


#endif // bvxm_create_scene_xml_process_h_
