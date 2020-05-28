// This is acal/io/acal_io_match_tree.h
#ifndef acal_io_match_tree_h_
#define acal_io_match_tree_h_

#include "acal_io_match_utils.h"

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <tuple>
#include <vector>

#include <acal/acal_match_tree.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>


// Typedef heterogeneous container used in serialization
typedef std::tuple<size_t, int, std::vector<int>, std::vector<std::vector<acal_match_pair> > > node_representation_type;

// acal_match_tree
void vsl_b_write(vsl_b_ostream &os, const acal_match_tree& tree);
void vsl_b_read(vsl_b_istream &is, acal_match_tree& tree);
void vsl_print_summary(std::ostream& os, const acal_match_tree& tree);

#endif // acal_io_match_tree_h_
