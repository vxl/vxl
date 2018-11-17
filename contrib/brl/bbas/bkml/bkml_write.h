// This is contrib/brl/bbas/bkml/bkml_write.h
#ifndef bkml_write_h
#define bkml_write_h

//:
// \file
// \brief A class with kml utilities
// \author Ozge C. Ozcanli ozge@visionsystemsinc.com
// \date  July 28, 2012
//
// \verbatim
//  Modifications
//   Yi Dong --- Feb, 2013  added method to write Photo overlay
//   Yi Dong --- Oct, 2014  added method to write Polygon with inner boundary
//   Yi Dong --- Oct, 2014  added method to write a point as a 2-d box
//   Yi Dong --- Dec, 2015  update box write method to have 'fill' option
// \endverbatim

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <functional>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_2.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>

class bkml_write
{
 public:

  //: Write KML header and open document tag
  static void open_document(std::ofstream& str);

  //: end document tag
  static void close_document(std::ofstream& str);

  //: Write a box
  static void write_box(std::ofstream &ofs, const std::string& name, const std::string& description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr);
  static void write_box(std::ofstream &ofs, std::string name, std::string description, vgl_box_2d<double> bbox);

  //: Write a box with color, color is in hexadecimale format: 0 - 255 --> 00 to ff, aabbggrr --> alpha alpha, blue blue, gree green , red red.. alpha is the opacity, ffffffff is white fully opaque
  static void write_box(std::ofstream &ofs, const std::string& name, const std::string& description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr, const std::string& hex_color,
                        unsigned const& fill = 0);
  static void write_box(std::ofstream &ofs, std::string name, std::string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr,
                        unsigned char const& r, unsigned char const& g, unsigned char const& b, unsigned char const&a = 85,
                        unsigned const& fill = 0);

  //: put a pin at the given location
  static void write_location(std::ofstream& ofs, const std::string& name, const std::string& description, double lat, double lon, double elev);
  static void write_location(std::ofstream& ofs, double lat, double lon, double elev,
                             std::string const& name = "location",
                             std::string const& description = "",
                             double const& scale = 1.0,
                             unsigned char const& r = 255,
                             unsigned char const& g = 131,
                             unsigned char const& b = 250);
  // write location as a small box
  static void write_location_as_box(std::ofstream& ofs, double lat, double lon, double elev,
                                    std::string const& name = "location",
                                    std::string const& description = "",
                                    double const& size = 1E-5,
                                    unsigned char const& r = 255,
                                    unsigned char const& g = 131,
                                    unsigned char const& b = 250);

  //: put a pin at the given location (x = lon and y = lat)
  static void write_location(std::ofstream& ofs, vgl_point_2d<double> const& loc,
                             std::string const& name = "location",
                             std::string const& description = "",
                             double const& scale = 1.0,
                             unsigned char const& r = 255,
                             unsigned char const& g = 131,
                             unsigned char const& b = 250);


  //: Write a photooverlay without img and correct near parameter though)
  static void write_photo_overlay(std::ofstream& ofs, const std::string& name,
                                  double lon, double lat, double alt,
                                  double head, double tilt, double roll,
                                  double t_fov, double r_fov,
                                  double value = 0.0);

  //: Write a polygon with color, line style
  static void write_polygon(std::ofstream& ofs, vgl_polygon<double> const& poly,
                            std::string const& name = "polygon",
                            std::string const& description = "",
                            double const& scale = 1.0,
                            double const& line_width = 3.0,
                            double const& alpha = 0.45,
                            unsigned char const& r = 0,
                            unsigned char const& g = 255,
                            unsigned char const& b = 0);

  //: Write a polygon with inner boundary
  // (first element in pair is the outer boundary (single sheet), second element in pair is the inner boundary)
  static void write_polygon(std::ofstream& ofs,
                            std::vector<std::pair<vgl_polygon<double>, vgl_polygon<double> > > const& polygon,
                            std::string const& name = "polygon",
                            std::string const& description = "",
                            double const& scale = 1.0,
                            double const& line_width = 3.0,
                            double const& alpha = 0.45,
                            unsigned char const& r = 0,
                            unsigned char const& g = 255,
                            unsigned char const& b = 0);

  //: Write a (path) with color and line style
  static void write_path(std::ofstream& ofs, std::vector<vgl_point_2d<double> > path,
                         std::string const& name = "paths",
                         std::string const& description = "",
                         double const& scale = 1.0,
                         double const& line_width = 3.0,
                         double const& alpha = 0.35,
                         unsigned char const& r = 255,
                         unsigned char const& g = 0,
                         unsigned char const& b = 0);


  //: Write a style include LineStyle and PolyStyle
  static void write_kml_style(std::ofstream& ofs,
                              const std::string& style_name = "kml_style",
                              double const& scale = 1.0,
                              double const& line_width = 3.0,
                              double const& alpha = 0.45,
                              unsigned char const& r = 0,
                              unsigned char const& g = 255,
                              unsigned char const& b = 0);

};

#endif
