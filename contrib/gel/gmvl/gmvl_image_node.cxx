/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_image_node"
#endif
#include "gmvl_image_node.h"


gmvl_image_node::gmvl_image_node( const vcl_string name, vil_image image)
  : gmvl_node( name),
    image_(image)
{
  type_= "gmvl_image_node";
}

gmvl_image_node::~gmvl_image_node()
{
  cerr << "gmvl_image_node: " << this << " destructor called..." << endl;
}

