// This is gel/gmvl/gmvl_helpers.h
#ifndef gmvl_helpers_h_
#define gmvl_helpers_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vcl_vector.h>
#include <vcl_string.h>
#include <gmvl/gmvl_node_sptr.h>

//: to print out a lot of nodes
vcl_ostream &operator<<( vcl_ostream &os, const vcl_vector<gmvl_node_sptr> &r);

// standard input and output

// standard double, double list
vcl_vector<gmvl_node_sptr> gmvl_load_raw_corners( const vcl_string filename);
gmvl_node_sptr gmvl_load_image( const vcl_string filename);

#endif // gmvl_helpers_h_
