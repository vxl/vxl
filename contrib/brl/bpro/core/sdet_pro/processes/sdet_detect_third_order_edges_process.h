// This is brl/bpro/core/sdet_pro/processes/sdet_detect_third_order_edges_process.h
#ifndef sdet_detect_third_order_edges_process_h_
#define sdet_detect_third_order_edges_process_h_
//:
// \file
// \brief A process that takes a grayscale image and returns the corresponding third order edge map
//
// \author Ozge C. Ozcanli-Ozbay
// \date July 27, 2010
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

//: initialize input and output types
bool sdet_detect_third_order_edges_process_cons(bprb_func_process& pro);

//: generates the edge map
bool sdet_detect_third_order_edges_process(bprb_func_process& pro);

//: initialize input and output types
bool sdet_detect_third_order_edges_dt_process_cons(bprb_func_process& pro);

//: generates the edge map
bool sdet_detect_third_order_edges_dt_process(bprb_func_process& pro);


#endif // sdet_detect_third_order_edges_process_h_
