// This is gel/gmvl/gmvl_tag_node.h
#ifndef gmvl_tag_node_h_
#define gmvl_tag_node_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vcl_string.h>
#include <gmvl/gmvl_node.h>

class gmvl_tag_node : public gmvl_node
{
 public:

  gmvl_tag_node( const vcl_string name);
  ~gmvl_tag_node();

  // accessor
  vcl_string &get() { return name_; }

 protected:
  vcl_string name_;
};

#endif // gmvl_tag_node_h_
