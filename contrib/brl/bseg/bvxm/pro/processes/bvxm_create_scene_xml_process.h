// This is brl/bseg/bvxm/pro/processes/bvxm_create_scene_xml_process.h
#ifndef bvxm_create_scene_xml_process_h_
#define bvxm_create_scene_xml_process_h_
//:
// \file
// \brief A class for generating bvxm scene xml, given series of scene parameters
//
// \author Yi Dong
// \date   Jun, 2015
// \verbatim
//  Modifications
// \endverbatim
#include <bprb/bprb_func_process.h>

// generate a bvxm scene xml
//: global variables
namespace bvxm_create_scene_xml_process_globals
{
  const unsigned n_inputs_  = 14;
  const unsigned n_outputs_ =  0;
}

//: set input and output types
bool bvxm_create_scene_xml_process_cons(bprb_func_process& pro);

//: generate a bvxm scene xml
bool bvxm_create_scene_xml_process(bprb_func_process& pro);

#endif // bvxm_create_scene_xml_process_h_