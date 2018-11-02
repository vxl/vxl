// This is brl/bpro/core/sdet_pro/processes/sdet_segment_image_process.h
#ifndef sdet_segment_image_process_h_
#define sdet_segment_image_process_h_
//:
// \file
// \brief A process that takes a grayscale image and returns the superpixel image - uses Felzenswalb's graph based image segmentation algorithm
//
// \author Ozge C. Ozcanli
// \date Sep 06, 2013
// \verbatim
//  Modifications
//
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace sdet_segment_image_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
}

bool sdet_segment_image_process_cons(bprb_func_process& pro);
bool sdet_segment_image_process(bprb_func_process& pro);


namespace sdet_segment_image_using_edge_map_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
}

bool sdet_segment_image_using_edge_map_process_cons(bprb_func_process& pro);
bool sdet_segment_image_using_edge_map_process(bprb_func_process& pro);


namespace sdet_segment_image_using_height_map_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
}

bool sdet_segment_image_using_height_map_process_cons(bprb_func_process& pro);
bool sdet_segment_image_using_height_map_process(bprb_func_process& pro);

namespace sdet_segment_image_using_height_map_process2_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 1;
}

bool sdet_segment_image_using_height_map_process2_cons(bprb_func_process& pro);
bool sdet_segment_image_using_height_map_process2(bprb_func_process& pro);



#endif // sdet_segment_image_process_h_
