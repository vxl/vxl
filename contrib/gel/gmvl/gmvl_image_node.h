// This is gel/gmvl/gmvl_image_node.h
#ifndef gmvl_image_node_h_
#define gmvl_image_node_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vil1/vil1_image.h>
#include <gmvl/gmvl_node.h>

class gmvl_image_node : public gmvl_node
{
 public:
  gmvl_image_node(vil1_image image) : image_(image) { type_="gmvl_image_node"; }
  ~gmvl_image_node() {}

  vil1_image image() const { return image_; }

 protected:
  vil1_image image_;
};

#endif // gmvl_image_node_h_
