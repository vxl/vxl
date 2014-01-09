//This is brl/bbas/bvgl/bvgl_labelme_parser.h
#ifndef bvgl_labelme_parser_h_
#define bvgl_labelme_parser_h_

// \verbatim
//  Modifications
//     Yi Dong   OCT-2012   added parser for mindist, maxdist, name, polygon defined in xml
//     Yi Dong   NOV-2012   added parser for order, image size, type and ImageCategory(desert or coast, etc) defined in xml
//     Yi Dong   JAN-2013   added parser for object orientation and NLCD land classfication
//     Yi Dong   JAN-2014   added new tags
// \endverbatim

#include <expatpplib.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
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
#define PIXEL_TAG "pixels"
#define X_TAG "x"
#define Y_TAG "y"
#define OBJECT_MINDIST_TAG "mindist"
#define OBJECT_MAXDIST_TAG "maxdist"
#define OBJECT_ORDER_TAG "order"
#define TYPE_TAG "type"
#define IMG_CAT_TAG "ImageCategory"
#define IMG_NI_TAG "nrows"
#define IMG_NJ_TAG "ncols"
#define NLCD_TAG "class"
#define ORIENT_TAG "orientation"
#define LAND_TAG "land"
#define WEIGHT_TAG "weight"
#define REGION_TAG "region"
#define FRAME_TAG "frame"


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

  //image filename/path, category, and size
  vcl_string image_name() const { return image_name_; }
  vcl_string image_category() const { return image_category_; }
  vcl_string region() const { return region_tag_; }
  unsigned image_ni() const { return image_ni_; }
  unsigned image_nj() const { return image_nj_; }
  //object names (in the same order as polygons)
  vcl_vector<vcl_string>& obj_names()            { return obj_names_; }
  vcl_vector<vcl_string>& obj_types()            { return obj_types_; }
  vcl_vector<float>& obj_mindists()              { return obj_min_dists_; }
  vcl_vector<float>& obj_maxdists()              { return obj_max_dists_; }
  vcl_vector<int>& obj_depth_orders()            { return obj_depth_orders_; }
  vcl_vector<vcl_string>& obj_orientations()     { return obj_orientations_; }
  vcl_vector<unsigned>& obj_nlcd_ids()           { return obj_nlcd_ids_; }
  vcl_vector<vcl_string>& obj_land_categories()  { return obj_land_categories_; }
  vcl_vector<float>& obj_weights()               { return obj_weights_; }
  vcl_vector<unsigned>& obj_frame_ids()          { return obj_frame_ids_; }

  // ACCESSORS for parser info
  vcl_vector<vgl_polygon<double> >& polygons() { return polygons_; }
  vcl_vector<vgl_point_2d<double> >& points()  { return pts_; }
  vcl_vector<vgl_point_2d<double> >& pixels()  { return pixels_; }
  void trim_string(vcl_string& s);

 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  //lvcs temp values
  vcl_vector<vgl_polygon<double> > polygons_;
  vcl_vector<vgl_point_2d<double> > pts_;
  vcl_vector<vgl_point_2d<double> > pixels_;
  double x_, y_;

  vcl_vector<vcl_string>         obj_names_;
  vcl_vector<vcl_string>         obj_types_;
  vcl_vector<float>          obj_min_dists_;
  vcl_vector<float>          obj_max_dists_;
  vcl_vector<int>         obj_depth_orders_;
  vcl_vector<unsigned>        obj_nlcd_ids_;
  vcl_vector<vcl_string>  obj_orientations_;
  vcl_vector<vcl_string>  obj_land_categories_;
  vcl_vector<float>       obj_weights_;
  vcl_vector<unsigned>    obj_frame_ids_;

  float min_dist_;
  float max_dist_;
  float weight_;
  int order_;
  vcl_string image_category_;
  vcl_string image_name_;
  vcl_string obj_orient_;
  vcl_string region_tag_;
  vcl_string temp_str_;
  unsigned frame_id_;
  unsigned nlcd_id_;
  unsigned image_ni_;
  unsigned image_nj_;
  

  //set active tag for parsing char data from different tags
  vcl_string active_tag_;
};

//string converter
template <typename T>
void convert(const char* t, T& d)
{
  vcl_string s = t;
  vcl_stringstream strm(s);
  strm >> d;
}

template <typename T>
void convert(vcl_string s, T& d)
{
  convert(s.c_str(), d);
}

#endif
