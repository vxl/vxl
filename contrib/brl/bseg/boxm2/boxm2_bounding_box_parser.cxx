//:
// \file
// \brief Parses the configuration file for bounding box tool.
//
#include <sstream>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "boxm2_bounding_box_parser.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  std::stringstream strm(t);
  strm >> d;
}

boxm2_bounding_box_parser::boxm2_bounding_box_parser()
{
  init_params();
}


void boxm2_bounding_box_parser::init_params()
{
  dataset_ = "" ;
  entity_id_= -1;
  volume_id_=-1;
  version_ = 1;
}


//-----------------------------------------------------------------------------
//: Start Element needs to parse the following tags

void
boxm2_bounding_box_parser::startElement(const char* name, const char** atts)
{
  if (std::strcmp(name,DATASET_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "name") == 0)
        convert(atts[i+1], dataset_);
    }
  }
  else if (std::strcmp(name,ENTITY_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "id") == 0)
      {
        convert(atts[i+1], entity_id_);
      }
    }
  }
  else if (std::strcmp(name,VOLUME_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "height") == 0)
        convert(atts[i+1], height_);
      else if (std::strcmp(atts[i], "id") == 0)
        convert(atts[i+1], volume_id_);
    }
    heights_[entity_id_][volume_id_] = height_;
  }
  else if (std::strcmp(name,VERT_TAG)== 0) {
    double x=0.0;
    double y=0.0;
    double z=0.0;
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "id") == 0)
        {
        int vertid;
        convert(atts[i+1], vertid);
        }
      else if (std::strcmp(atts[i], "x") == 0)
        {
        convert(atts[i+1], x);
        }
      else if (std::strcmp(atts[i], "y") == 0)
        {
        convert(atts[i+1], y);
        }
      else if (std::strcmp(atts[i], "z") == 0)
        {
        convert(atts[i+1], z);
        }
    }
    const vgl_point_3d<double> temp(x,y,z);
    verts_[entity_id_][volume_id_].push_back(temp);
  }
}
