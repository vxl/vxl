// This is gel/gmvl/gmvl_helpers.cxx
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <fstream>
#include "gmvl_helpers.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <gmvl/gmvl_corner_node.h>
#include <gmvl/gmvl_image_node.h>


// input output
std::ostream &operator<<( std::ostream &os, const std::vector<gmvl_node_sptr> &r)
{
  for (const auto & i : r) os << *i;
  return os;
}


// loaders and savers

std::vector<gmvl_node_sptr> gmvl_load_raw_corners( const std::string& filename)
{
  std::vector<gmvl_node_sptr> corners;

  std::ifstream fin( filename.c_str());

  while (fin.good() && !fin.eof())
    {
      double x,y;

      fin >> x >> y;

      if (fin.good()) corners.push_back( new gmvl_corner_node( x, y));
    }

  std::cerr << "gmvl_load_raw_corners: loaded " << corners.size() << " corners from " << filename << std::endl;

  return corners;
}

gmvl_node_sptr gmvl_load_image( const std::string& filename)
{
  vil1_image image= vil1_load( filename.c_str());
  gmvl_image_node *node= new gmvl_image_node( image);

  if (image)
    std::cerr << "gmvl_load_image: load image from " << filename << std::endl;
  else
    std::cerr << "gmvl_load_image: failed to load image from " << filename << std::endl;

  return gmvl_node_sptr( node);
}
