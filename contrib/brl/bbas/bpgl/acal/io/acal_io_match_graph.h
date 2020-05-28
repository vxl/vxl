// This is acal/io/acal_io_match_graph.h
#ifndef acal_io_match_graph_h_
#define acal_io_match_graph_h_

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <tuple>
#include <utility>
#include <vector>

#include <acal/acal_match_graph.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>


// Typedef heterogeneous containers used in serialization
typedef std::pair<bool, std::vector<size_t> > vertex_representation_type;
typedef std::tuple<size_t, size_t, std::vector<acal_match_pair> > edge_representation_type;

//: match_params
void vsl_b_write(vsl_b_ostream &os, const match_params& obj);
void vsl_b_read(vsl_b_istream &is, match_params& obj);
void vsl_print_summary(std::ostream& os, const match_params& obj);

//: acal_match_graph
void vsl_b_write(vsl_b_ostream &os, const acal_match_graph& graph);
void vsl_b_read(vsl_b_istream &is, acal_match_graph& graph);
void vsl_print_summary(std::ostream& os, const acal_match_graph& graph);

#endif // acal_io_match_graph_h_
