// This is brl/bbas/volm/volm_candidate_region_parser.h
#ifndef volm_candidate_region_parser_h_
#define volm_candidate_region_parser_h_
//:
// \file
// \brief A class to parse series of candidate region kml file
//
// \author Yi Dong
// \date October, 2014
// \verbatim
// Modifications
//  <none yet>
// \endverbatim

// Tages for KML
// general
#ifdef WIN32
  #define _LIB
#endif
#define KML_PLACEMARK_NAME_TAG "name"

// camera
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
#define KML_HEAD_DEV_TAG "heading_deviation"
#define KML_TILT_DEV_TAG "tilt_deviation"
#define KML_ROLL_DEV_TAG "roll_deviation"
#define KML_RFOV_DEV_TAG "rightFov_deviation"
#define KML_TFOV_DEV_TAG "topFov_deviation"

// geometry
#define KML_CORD_TAG "coordinates"
#define KML_POLYCORE_END_TAG "/coordinates"
#define KML_LINE_TAG "LineString"
#define KML_POLY_TAG "Polygon"
#define KML_POINT_TAG "Point"
#define KML_POLYOB_TAG "outerBoundaryIs"
#define KML_POLYIB_TAG "innerBoundaryIs"

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <map>
#include <expatpp.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class volm_candidate_region_parser : public expatpp
{
 public:
  volm_candidate_region_parser(void);
  // parser should not delete the site, it is used afterwards
  ~volm_candidate_region_parser(void) override = default;

  //: parse all the points that have given name (will return empty vector if kml file does not have points with given name)
  static std::vector<vgl_point_3d<double> > parse_points(std::string const& kml_file, std::string const& name);

  //: parse all lines that have given name
  static std::vector<std::vector<vgl_point_3d<double> > > parse_lines(std::string const& kml_file, std::string const& name);

  //: parse the exterior boundaries of polygons that have given name
  static vgl_polygon<double> parse_polygon(std::string const& kml_file, std::string const& name);

  //: parse polygons that have given name
  static vgl_polygon<double> parse_polygon_with_inner(std::string const& kml_file, std::string const& name,
                                                      vgl_polygon<double>& outer, vgl_polygon<double>& inner,
                                                      unsigned& n_out, unsigned& n_in);

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
  //: map of polygons based on the Placemark name
  std::map<std::string, std::vector<std::vector<vgl_point_3d<double> > > > polyouter_;
  std::map<std::string, std::vector<std::vector<vgl_point_3d<double> > > > polyinner_;
  //: map of lines based on the Placemark name
  std::map<std::string, std::vector<std::vector<vgl_point_3d<double> > > > linecords_;

  //: map of points based on the Placemark name
  std::map<std::string, std::vector<vgl_point_3d<double> > > points_;

 private:
  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* name) override;
  void charData(const XML_Char* s, int len) override;
  void handleAtts(const XML_Char** atts);
  void cdataHandler(const std::string& name, const std::string& data);
  void init_params();

  std::string current_name_;
  std::string region_name_;
  std::string last_tag;
  std::string cord_tag_;
};

#endif // volm_candidate_region_parser_h_
