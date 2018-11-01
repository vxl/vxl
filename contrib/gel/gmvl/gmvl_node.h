// This is gel/gmvl/gmvl_node.h
#ifndef gmvl_node_h_
#define gmvl_node_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>

class gmvl_node_cache;
class gmvl_connection_cache;

//:
// Types of node:
//  -    images
//  -    image features (points, lines, conics...)
//  -    transformations (homographies, projection matrices...)
//  -    coordinate systems (?)

class gmvl_node : public vbl_ref_count
{
 public:
  gmvl_node() : type_("gmvl_node"), ref_(-1) {}
  gmvl_node(gmvl_node const& n) : vbl_ref_count(), type_(n.type_), ref_(n.ref_) {}
  ~gmvl_node() override = default;

  // getter
  std::string &type() { return type_; }

  // input output
  friend std::ostream &operator<<( std::ostream &os, gmvl_node &node);

  // to allow the cache to access ref
  friend class gmvl_node_cache;
  friend class gmvl_connection_cache;

 protected:
  // type name (name of class)
  std::string type_;

  // reference number (used by caches)
  int ref_;
};

std::ostream &operator<<(std::ostream &os, gmvl_node &node);

#endif // gmvl_node_h_
