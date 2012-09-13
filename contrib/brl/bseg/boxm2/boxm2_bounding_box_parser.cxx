//:
// \file
// \brief Parses the configuration file for bounding box tool.
//
#include "boxm2_bounding_box_parser.h"

#include <vcl_sstream.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>

// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
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
  if (vcl_strcmp(name,DATASET_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "name") == 0)
        convert(atts[i+1], dataset_);
    }
  }
  else if (vcl_strcmp(name,ENTITY_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "id") == 0)
      {
        convert(atts[i+1], entity_id_);
      }
    }
  }
  else if (vcl_strcmp(name,VOLUME_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "height") == 0)
        convert(atts[i+1], height_);
      else if (vcl_strcmp(atts[i], "id") == 0)
        convert(atts[i+1], volume_id_);
    }
    heights_[entity_id_][volume_id_] = height_;
  }
  else if (vcl_strcmp(name,VERT_TAG)== 0) {
    double x,y,z;
    int vertid;
    for (int i=0; atts[i]; i+=2) {
      if (vcl_strcmp(atts[i], "id") == 0)
        convert(atts[i+1], vertid);
      else if (vcl_strcmp(atts[i], "x") == 0)
        convert(atts[i+1], x);
      else if (vcl_strcmp(atts[i], "y") == 0)
        convert(atts[i+1], y);
      else if (vcl_strcmp(atts[i], "z") == 0)
        convert(atts[i+1], z);
    }
    verts_[entity_id_][volume_id_].push_back(vgl_point_3d<double>(x,y,z));
  }
}

