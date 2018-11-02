//:
// \file
// \brief Parses the configuration file for bwm tool.
//
#include <sstream>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "bvgl_labelme_parser.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//Constructor from file
bvgl_labelme_parser::bvgl_labelme_parser(std::string& filename)
{
  std::FILE* xmlFile = std::fopen(filename.c_str(), "r");
  if (!xmlFile) {
    std::cerr << filename << " error on opening\n";
    throw -1;
  }
  if (!this->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(this->XML_GetErrorCode()) << " at line "
             << this->XML_GetCurrentLineNumber() << '\n';
    throw -1;
  }
}

//-----------------------------------------------------------------------------
//: Start Element needs to parse the following tags
//#define ANNOTATION "annotation"
//#define FILENAME_TAG "filename"
//#define FOLDER_TAG "folder"
//#define OBJECT_TAG "object"
//#define NAME_TAG "name"
//#define POLYTON_TAG "polygon"
//#define OBJECT_MINDIST_TAG "mindist"
//#define OBJECT_MAXDIST_TAG "maxdist"
//#define OBJECT_ORDER_TAG "order"
//#define TYPE_TAG "type"
//#define POINT_TAG "pt"
//#define X_TAG "x"
//#define Y_TAG "y"
//#define LAND_TAG "land"
//#define WEIGHT_TAG "weight"
void
bvgl_labelme_parser::startElement(const char* name, const char**  /*atts*/)
{
  //set active tag for charData
  active_tag_ = std::string(name);

  //parse object/polygon, start with a fresh set of points
  if (std::strcmp(name, POLYGON_TAG)==0)
    pts_.clear();

  if (std::strcmp(name, OBJECT_TAG)==0) {
    min_dist_ = -1;
    max_dist_ = -1;
    order_ = -1;
    weight_ = 0;
    frame_id_ = 0;
  }
}

//Creates and pushes polygon, creates/pushes point
void bvgl_labelme_parser::endElement(const XML_Char* name)
{
  if (std::strcmp(name, FILENAME_TAG)==0)
    image_name_ = temp_str_;

  if (std::strcmp(name, REGION_TAG)==0)
    region_tag_ = temp_str_;

  //Finish up polygon
  if (std::strcmp(name, POLYGON_TAG)==0) {
    vgl_polygon<double> poly(pts_);
    polygons_.push_back(poly);
  }

  if (std::strcmp(name, PIXEL_TAG)==0 || std::strcmp(name, SINGLE_PIONT_TAG)==0 ) {
    vgl_point_2d<double> pt(x_, y_);
    pixels_.push_back(pt);
  }

  //finish up a point
  if (std::strcmp(name, Y_TAG)==0) {
    vgl_point_2d<double> pt(x_, y_);
    pts_.push_back(pt);
  }

  //finish up an object
  if (std::strcmp(name, OBJECT_TAG)==0) {
    obj_min_dists_.push_back(min_dist_);
    obj_max_dists_.push_back(max_dist_);
    obj_depth_orders_.push_back(order_);
    obj_heights_.push_back(height_);
    obj_nlcd_ids_.push_back(nlcd_id_);
    obj_weights_.push_back(weight_);
    obj_frame_ids_.push_back(frame_id_);
    if (reference_ == 0)
      obj_references_.push_back(false);
    else
      obj_references_.push_back(true);
  }
}


//Grabs data from points
void bvgl_labelme_parser::charData(const XML_Char* s, int len)
{
#if 0
  if (active_tag_ == X_TAG || active_tag_ == Y_TAG) {
    int val;
    convert(std::string(s,len), val);
    if (active_tag_ == X_TAG)
      x_ = (double) val;
    if (active_tag_ == Y_TAG)
      y_ = (double) val;
  }
#endif
  if (active_tag_ == X_TAG )
    convert(std::string(s,len), x_);

  if (active_tag_ == Y_TAG )
    convert(std::string(s,len), y_);

  if (active_tag_ == FILENAME_TAG)
    temp_str_ = std::string(s, len);

  if (active_tag_ == IMG_CAT_TAG)
    image_category_ = std::string(s, len);

  if (active_tag_ == REGION_TAG)
    temp_str_ = std::string(s,len);

  if (active_tag_ == NAME_TAG) {
    std::string name = std::string(s,len);
    this->trim_string(name);
    if (name.length() != 0)
      obj_names_.push_back(name);
  }

  if (active_tag_ == TYPE_TAG) {
    std::string type = std::string(s,len);
    this->trim_string(type);
    if (type.length() != 0)
      obj_types_.push_back(type);
  }

  if (active_tag_ == ORIENT_TAG) {
    std::string orientation = std::string(s,len);
    this->trim_string(orientation);
    if (orientation.length() != 0)
      obj_orientations_.push_back(orientation);
  }

  if (active_tag_ == LAND_TAG) {
    std::string land = std::string(s,len);
    this->trim_string(land);
    if (land.length() != 0)
      obj_land_categories_.push_back(land);
  }
  if (active_tag_ == HEIGHT_TAG) {
    convert(std::string(s, len), height_);
  }

  if (active_tag_ == REFERENCE_TAG)
    convert(std::string(s, len), reference_);

  if (active_tag_ == OBJECT_MINDIST_TAG)
    convert(std::string(s,len), min_dist_);

  if (active_tag_ == OBJECT_MAXDIST_TAG)
    convert(std::string(s,len), max_dist_);

  if (active_tag_ == OBJECT_ORDER_TAG)
    convert(std::string(s,len), order_);

  if (active_tag_ == WEIGHT_TAG)
    convert(std::string(s,len), weight_);

  if (active_tag_ == IMG_NI_TAG)
    convert(std::string(s,len), image_ni_);

  if (active_tag_ == IMG_NJ_TAG)
    convert(std::string(s,len), image_nj_);

  if (active_tag_ == NLCD_TAG)
    convert(std::string(s,len), nlcd_id_);

  if (active_tag_ == FRAME_TAG)
    convert(std::string(s,len), frame_id_);
}

void bvgl_labelme_parser::trim_string(std::string& s)
{
  char trims[4];
  trims[0] = ' '; trims[1] = '\0'; trims[2] = '\n'; trims[3] = '\t';
#if 0
  int i = (int)s.find_first_not_of(' ');
  int j = (int)s.find_last_not_of(' ');
  std::string t = s.substr(i,j-i+1);
#endif
  std::string t = s;
  bool trimmed = true;
  while (trimmed) {
    trimmed = false;
    for (char trim : trims) {
      std::string current = t;
      auto i = (unsigned int)current.find_first_not_of(trim);
      auto j = (unsigned int)current.find_last_not_of(trim);
      if (i > j || j >= current.size()) {
        t = ""; break;
      }
      else {
        t = current.substr(i,j-i+1);
        if (t.size() != current.size())
          trimmed = true;
      }
    }
  }
  s = t;
}
