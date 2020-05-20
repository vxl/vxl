// This is acal/io/acal_io_match_utils.h
#ifndef acal_io_match_utils_h_
#define acal_io_match_utils_h_

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <acal/acal_match_utils.h>

//: acal_corr
void vsl_b_write(vsl_b_ostream &os, const acal_corr& obj);
void vsl_b_read(vsl_b_istream &is, acal_corr& obj);

//: acal_match_pair
void vsl_b_write(vsl_b_ostream &os, const acal_match_pair& obj);
void vsl_b_read(vsl_b_istream &is, acal_match_pair& obj);


#endif // acal_io_match_utils_h_
