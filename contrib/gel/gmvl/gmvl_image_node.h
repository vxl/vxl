#ifndef gmvl_image_node_h_
#define gmvl_image_node_h_
#ifdef __GNUC__
#pragma interface "gmvl_image_node"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_string.h>
#include <vil/vil_image.h>
#include <gmvl/gmvl_node.h>

class gmvl_image_node : public gmvl_node
{
public:

  gmvl_image_node( vil_image image);
  ~gmvl_image_node();

  vil_image get_image() const { return image_; }

protected:

  vil_image image_;

};

#endif
