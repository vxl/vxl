/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "gmvl_image_node.h"


gmvl_image_node::gmvl_image_node( vil_image image)
  : image_(image)
{
  type_= "gmvl_image_node";
}

gmvl_image_node::~gmvl_image_node()
{
}

