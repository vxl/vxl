// This is gel/gmvl/gmvl_helpers.h
#ifndef gmvl_helpers_h_
#define gmvl_helpers_h_
//:
// \file
// \author crossge@crd.ge.com

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <gmvl/gmvl_node_sptr.h>

//: to print out a lot of nodes
std::ostream &operator<<( std::ostream &os, const std::vector<gmvl_node_sptr> &r);

// standard input and output

// standard double, double list
std::vector<gmvl_node_sptr> gmvl_load_raw_corners( const std::string& filename);
gmvl_node_sptr gmvl_load_image( const std::string& filename);

#endif // gmvl_helpers_h_
