// This is gel/gmvl/gmvl_image_node.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_image_node.h"

gmvl_image_node::gmvl_image_node( vil_image image)
  : image_(image)
{
  type_= "gmvl_image_node";
}

gmvl_image_node::~gmvl_image_node()
{
}

