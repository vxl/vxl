// This is gel/gmvl/gmvl_tag_node.h
#ifndef gmvl_tag_node_h_
#define gmvl_tag_node_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_string.h>
#include <gmvl/gmvl_node.h>

class gmvl_tag_node : public gmvl_node
{
 public:
  gmvl_tag_node(vcl_string const& name) : name_(name) { type_="gmvl_tag_node"; }
  ~gmvl_tag_node() {}

  // accessor
  vcl_string &get() { return name_; }

 protected:
  vcl_string name_;
};

#endif // gmvl_tag_node_h_
