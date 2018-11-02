// This is brl/bpro/core/sdet_pro/processes/sdet_detect_edge_tangent_interp_process.h
#ifndef sdet_detect_edge_tangent_interp_process_h_
#define sdet_detect_edge_tangent_interp_process_h_
//:
// \file
// \brief A process that detects sub-pixel edges and edge tangent directions using cubic interpolated Canny edge chains
//
// \author Ozge C. Ozcanli
// \date July 07, 2010
// \verbatim
//  Modifications
//    none
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace sdet_detect_edge_tangent_interp_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;

  // parameter strings
  const std::string param_noise_multiplier_ =  "noise_multiplier";
  const std::string param_smooth_ =  "smooth";
  const std::string param_automatic_threshold_ =  "automatic_threshold";
  const std::string param_junctionp_ =  "junctionp";
  const std::string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
}

//: initialize input and output types
bool sdet_detect_edge_tangent_interp_process_cons(bprb_func_process& pro);

//: generates the edge map
bool sdet_detect_edge_tangent_interp_process(bprb_func_process& pro);

//: global variables
namespace sdet_detect_edge_line_fitted_process_globals
{
  // parameter strings
  const std::string param_noise_multiplier_ =  "noise_multiplier";
  const std::string param_smooth_ =  "smooth";
  const std::string param_automatic_threshold_ =  "automatic_threshold";
  const std::string param_junctionp_ =  "junctionp";
  const std::string param_aggressive_junction_closure_ =  "aggressive_junction_closure";
  const std::string param_min_fit_length_ = "min_fit_length";
  const std::string param_rms_distance_ = "rms_distance";
}
//: a process that fits lines to detected canny edges and samples those lines to generate sub-pixel edges
bool sdet_detect_edge_line_fitted_process_cons(bprb_func_process& pro);
bool sdet_detect_edge_line_fitted_process(bprb_func_process& pro);


//: a helper process that prints out subpixel edge_image into a text file
bool sdet_write_edge_file_process_cons(bprb_func_process& pro);
bool sdet_write_edge_file_process(bprb_func_process& pro);

#endif // sdet_detect_edge_tangent_interp_process_h_
