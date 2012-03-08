//:
// \file
// \brief Parses the configuration file for bwm tool.
//
#include "bvgl_labelme_parser.h"

#include <vcl_sstream.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>


//Constructor from file
bvgl_labelme_parser::bvgl_labelme_parser(vcl_string& filename)
{
  vcl_FILE* xmlFile = vcl_fopen(filename.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << filename << " error on opening\n";
    throw -1;
  }
  if (!this->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(this->XML_GetErrorCode()) << " at line "
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
//#define POINT_TAG "pt"
//#define X_TAG "x"
//#define Y_TAG "y"
void
bvgl_labelme_parser::startElement(const char* name, const char** atts)
{
  //set active tag for charData
  active_tag_ = vcl_string(name); 

  //parse object/polygon, start with a fresh set of points
  if(vcl_strcmp(name, POLYGON_TAG)==0) 
    pts_.clear();
}

//Creates and pushes polygon, creates/pushes point
void bvgl_labelme_parser::endElement(const XML_Char* name)
{
  //Finish up polygon
  if(vcl_strcmp(name, POLYGON_TAG)==0) {
    vgl_polygon<double> poly(pts_); 
    polygons_.push_back(poly);
  }

  //finish up a point
  if(vcl_strcmp(name, Y_TAG)==0) {
    vgl_point_2d<double> pt(x_, y_);
    pts_.push_back(pt);
  }
}


//Grabs data from points
void bvgl_labelme_parser::charData(const XML_Char* s, int len)
{
  if(active_tag_ == X_TAG || active_tag_ == Y_TAG) {
    int val; 
    convert(vcl_string(s,len), val);
    if(active_tag_ == X_TAG)
      x_ = (double) val;
    if(active_tag_ == Y_TAG) 
      y_ = (double) val;
  }

  if(active_tag_ == FILENAME_TAG)
    image_name_ = vcl_string(s, len);

  if(active_tag_ == NAME_TAG) {
    vcl_string name = vcl_string(s,len);
    obj_names_.push_back(name);
  }
}

