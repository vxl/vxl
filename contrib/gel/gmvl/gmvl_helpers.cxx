// This is gel/gmvl/gmvl_helpers.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gmvl_helpers.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <gmvl/gmvl_corner_node.h>
#include <gmvl/gmvl_image_node.h>


// input output
vcl_ostream &operator<<( vcl_ostream &os, const vcl_vector<gmvl_node_sptr> &r)
{
  for (unsigned int i=0; i< r.size(); ++i) os << *r[i];
  return os;
}


// loaders and savers

vcl_vector<gmvl_node_sptr> gmvl_load_raw_corners( const vcl_string filename)
{
  vcl_vector<gmvl_node_sptr> corners;

  vcl_ifstream fin( filename.c_str());

  while (fin.good() && !fin.eof())
    {
      double x,y;

      fin >> x >> y;

      if (fin.good()) corners.push_back( new gmvl_corner_node( x, y));
    }

  vcl_cerr << "gmvl_load_raw_corners: loaded " << corners.size() << " corners from " << filename << vcl_endl;

  return corners;
}

gmvl_node_sptr gmvl_load_image( const vcl_string filename)
{
  vil_image image= vil_load( filename.c_str());
  gmvl_image_node *node= new gmvl_image_node( image);

  if (image)
    vcl_cerr << "gmvl_load_image: load image from " << filename << vcl_endl;
  else
    vcl_cerr << "gmvl_load_image: failed to load image from " << filename << vcl_endl;

  return gmvl_node_sptr( node);
}
