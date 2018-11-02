// This is brl/bseg/bvpl/bvpl_octree/io/bvpl_io_taylor_scenes_map.h
#ifndef bvpl_io_taylor_scenes_map_h
#define bvpl_io_taylor_scenes_map_h
//:
// \file
// \brief Binary IO for bvpl_taylor_scenes_map.
//        Not implemented, only declared so it can be stored in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  15-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <bvpl/bvpl_octree/bvpl_taylor_scenes_map.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>

//: Binary save parameters to stream.
void vsl_b_write(vsl_b_ostream & os, bvpl_kernel const &bvpl_taylor_scenes_map);

//: Binary load parameters from stream.
void vsl_b_read(vsl_b_istream & is, bvpl_taylor_scenes_map &m);

void vsl_print_summary(std::ostream &os, const bvpl_taylor_scenes_map &m);

void vsl_b_read(vsl_b_istream& is, bvpl_taylor_scenes_map* m);

void vsl_b_write(vsl_b_ostream& os, const bvpl_taylor_scenes_map* &m);

void vsl_print_summary(std::ostream& os, const bvpl_taylor_scenes_map* &m);

#endif
