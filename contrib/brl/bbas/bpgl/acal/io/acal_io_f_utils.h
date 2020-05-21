// This is acal/io/acal_io_f_utils.h
#ifndef acal_io_f_utils_h_
#define acal_io_f_utils_h_

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <acal/acal_f_utils.h>

//: acal_corr
void vsl_b_write(vsl_b_ostream &os, const f_params& obj);
void vsl_b_read(vsl_b_istream &is, f_params& obj);


#endif