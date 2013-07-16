#ifndef bkml_parser_h_
#define bkml_parser_h_
//:
// \file
// \verbatim
//  Modifications
//   Yi Dong   SEP-2012   added parser for polygon (inner/outer) defined in kml
//   Yi Dong   NOV-2012   added parser for deviations of heading, tilt, roll, right_fov, top_fov of camera
// \endverbatim

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
#define KML_POINT_TAG "Point"
#define KML_POLYOB_TAG "outerBoundaryIs"
#define KML_POLYIB_TAG "innerBoundaryIs"
#define KML_CORD_TAG "coordinates"
#define KML_POLYCORE_END_TAG "/coordinates"
#define KML_LINE_TAG "LineString"
//#define KML_PLACEMARK_NAME_TAG "name"


#define KML_HEAD_DEV_TAG "heading_deviation"
#define KML_TILT_DEV_TAG "tilt_deviation"
#define KML_ROLL_DEV_TAG "roll_deviation"
#define KML_RFOV_DEV_TAG "rightFov_deviation"
#define KML_TFOV_DEV_TAG "topFov_deviation"
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


  //: parser to load the points defined in kml file
  static vcl_vector<vgl_point_3d<double> > parse_points(vcl_string kml_file);

  //: parser to load the outer boundary of all defined polygons in the kml file (only parse the lat(y) and lon(x), elev ignored)
  static vgl_polygon<double> parse_polygon(vcl_string poly_kml_file);

  //: parser to load the outer and inner boundary, the first n_out sheets are the outer boundary 
  //  and the following n_in sheets are the inner boundary
  static vgl_polygon<double> parse_polygon_with_inner(vcl_string poly_kml_file, unsigned& n_out, unsigned& n_in);

  static bool parse_location_from_kml(vcl_string kml_file, double& lat, double& lon);

  static void trim_string(vcl_string& s);

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
  double heading_dev_;
  double tilt_dev_;
  double roll_dev_;
  double right_fov_dev_;
  double top_fov_dev_;
  vcl_vector<vcl_vector<vgl_point_3d<double> > > polyouter_;
  vcl_vector<vcl_vector<vgl_point_3d<double> > > polyinner_;
  vcl_vector<vcl_vector<vgl_point_3d<double> > > linecord_;
  vcl_vector<vgl_point_3d<double> > points_;
  
  vcl_string current_name_;
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
  
};


#endif
