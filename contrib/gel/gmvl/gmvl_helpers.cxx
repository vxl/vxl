/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gmvl_helpers"
#endif
#include "gmvl_helpers.h"

#include <vcl/vcl_fstream.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <gmvl/gmvl_corner_node.h>
#include <gmvl/gmvl_image_node.h>


vcl_vector<gmvl_node_ref> gmvl_load_raw_corners( const vcl_string filename)
{
  vcl_vector<gmvl_node_ref> corners;

  ifstream fin( filename.c_str());

  while( fin.good() && !fin.eof())
    {
      double x,y;

      fin >> x >> y;
      
      if( fin.good()) corners.push_back( new gmvl_corner_node( x, y));
    }
  
  return corners;
}


gmvl_node_ref gmvl_load_image( const vcl_string filename)
{
  vil_image image= vil_load( filename.c_str());
  gmvl_image_node *node= new gmvl_image_node( image);

  return gmvl_node_ref( node);
}
