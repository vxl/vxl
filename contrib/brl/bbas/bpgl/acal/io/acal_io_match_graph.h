// This is acal/io/acal_io_match_graph.h
#ifndef acal_io_match_graph_h_
#define acal_io_match_graph_h_

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <acal/acal_match_graph.h>

//: match_params
void vsl_b_write(vsl_b_ostream &os, const match_params& obj);
void vsl_b_read(vsl_b_istream &is, match_params& obj);

//: TODO acal_match_graph
// void vsl_b_write(vsl_b_ostream &os, const acal_match_graph& obj);
// void vsl_b_read(vsl_b_istream &is, acal_match_graph& obj);


#endif // acal_io_match_graph_h_
