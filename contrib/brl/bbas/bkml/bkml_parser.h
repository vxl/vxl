#ifndef bkml_parser_h_
#define bkml_parser_h_

// Tags for KML
#define PHOTO_OVL_TAG "PhotoOverlay"
#define KML_CAMERA_TAG "Camera"
#define KML_LON_TAG "longitude"
#define KML_LAT_TAG "latitude"
#define KML_ALT_TAG "altitude"
#define KML_HEAD_TAG "heading"
#define KML_TILT_TAG "tilt"
#define KML_ROLL_TAG "roll"
#define KML_RFOV_TAG "rightFov"
#define KML_TFOV_TAG "topFov"
#define KML_NEAR_TAG "near"
#define KML_POLY_TAG "Polygon"
#define KML_POLYOB_TAG "outerBoundaryIs"
#define KML_POLYIB_TAG "innerBoundaryIs"
#define KML_CORD_TAG "coordinates"
#define KML_POLYCORE_END_TAG "/coordinates"
#define KML_LINE_TAG "LineString"


#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>


class bkml_parser : public expatpp
{
 public:
  bkml_parser(void);
  // parser should not delete the site, it is used afterwards
  ~bkml_parser(void) {}
  
  //: the first sheet contains the outer polygon, and the second sheet contains the inner polygon if any, saves 2d points, only lat, lon
  // in this poly lon is x, lat is y
  static vgl_polygon<double> parse_polygon(vcl_string poly_kml_file);

  // results of parse
  double longitude_;
  double latitude_;
  double altitude_;
  double heading_;
  double tilt_;
  double roll_;
  double right_fov_;
  double top_fov_;
  double near_;
  vcl_vector<vgl_point_3d<double> > polyouter_;
  vcl_vector<vgl_point_3d<double> > polyinner_;
  vcl_vector<vgl_point_3d<double> > linecord_;
 private:

  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);
  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void init_params();

  //element parser
  vcl_string last_tag;
  vcl_string cord_tag_;
  void trim_string(vcl_string& s);
};

#endif
