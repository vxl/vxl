//This is brl/bbas/bvgl/bvgl_labelme_parser.h
#ifndef bvgl_labelme_parser_h_
#define bvgl_labelme_parser_h_

#include <expatpplib.h>
#include <vcl_string.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_2d.h>

//tag macros
#define ANNOTATION "annotation"
#define FILENAME_TAG "filename"
#define FOLDER_TAG "folder"
#define OBJECT_TAG "object"
#define NAME_TAG "name"
#define POLYGON_TAG "polygon"
#define POINT_TAG "pt"
#define X_TAG "x"
#define Y_TAG "y"

#if 0
<annotation>
filename>andy_eze.jpg</filename>
<folder>test</folder>
<source>
<sourceImage>The MIT-CSAIL database of objects and scenes</sourceImage>
<sourceAnnotation>LabelMe Webtool</sourceAnnotation>
</source>
<object>
<name>mouth</name>
<deleted>0</deleted>
<verified>0</verified>
<date>05-Mar-2012 22:14:49</date>
<id>0</id>
<polygon>
<username>Andy</username>
<pt>
<x>335</x>
<y>183</y>
</pt>
</polygon>
</object>
#endif

class bvgl_labelme_parser : public expatpp
{
 public:
  bvgl_labelme_parser() {}
  bvgl_labelme_parser(vcl_string& filename);
  ~bvgl_labelme_parser(void) {}

  // ACCESSORS for parser info
  vcl_vector<vgl_polygon<double> > polygons() { return polygons_; }

 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  //lvcs temp values
  vcl_vector<vgl_polygon<double> > polygons_;
  vcl_vector<vgl_point_2d<double> > pts_;
  double x_, y_;

  //helper bool
  bool save_x_;
  bool save_y_;
  void reset_bools() { save_x_ = false; save_y_ = false; }
};

//string converter
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
  strm >> d;
}

template <typename T>
void convert(vcl_string s, T& d)
{
  convert(s.c_str(), d);
}

#endif
