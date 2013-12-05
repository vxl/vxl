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
//   Yi Dong --- Feb, 2013  adding functions to write the Photooverlay
// \endverbatim

#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_functional.h>
#include <vnl/vnl_double_2.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>

class bkml_write
{
 public:

  //: Write KML header and open document tag
  static void open_document(vcl_ofstream& str);

  //: end document tag
  static void close_document(vcl_ofstream& str);

  //: Write a box
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr);
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vgl_box_2d<double> bbox);
  
  //: Write a box with color, color is in hexadecimale format: 0 - 255 --> 00 to ff, aabbggrr --> alpha alpha, blue blue, gree green , red red.. alpha is the opacity, ffffffff is white fully opaque
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr, vcl_string hex_color);
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr,
                        unsigned char const& r, unsigned char const& g, unsigned char const& b);

  //: put a pin at the given location
  static void write_location(vcl_ofstream& ofs, vcl_string name, vcl_string description, double lat, double lon, double elev);
  static void write_location(vcl_ofstream& ofs, double lat, double lon, double elev,
                             vcl_string const& name = "location",
                             vcl_string const& description = "",
                             double const& scale = 1.0,
                             unsigned char const& r = 255,
                             unsigned char const& g = 131,
                             unsigned char const& b = 250);

  //: put a pin at the given location (x = lon and y = lat)
  static void write_location(vcl_ofstream& ofs, vgl_point_2d<double> const& loc,
                             vcl_string const& name = "location",
                             vcl_string const& description = "",
                             double const& scale = 1.0,
                             unsigned char const& r = 255,
                             unsigned char const& g = 131,
                             unsigned char const& b = 250);
                             

  //: Write a photooverlay without img and correct near parameter though)
  static void write_photo_overlay(vcl_ofstream& ofs, vcl_string name,
                                  double lon, double lat, double alt,
                                  double head, double tilt, double roll,
                                  double t_fov, double r_fov,
                                  double value = 0.0);

  //: Write a polygon with color, line style
  static void write_polygon(vcl_ofstream& ofs, vgl_polygon<double> const& poly,
                            vcl_string const& name = "polygon",
                            vcl_string const& description = "",
                            double const& scale = 1.0,
                            double const& line_width = 3.0,
                            double const& alpha = 0.45,
                            unsigned char const& r = 0,
                            unsigned char const& g = 255,
                            unsigned char const& b = 0);

  //: Write a (path) with color and line style
  static void write_path(vcl_ofstream& ofs, vcl_vector<vgl_point_2d<double> > path,
                         vcl_string const& name = "paths",
                         vcl_string const& description = "",
                         double const& scale = 1.0,
                         double const& line_width = 3.0,
                         double const& alpha = 0.35,
                         unsigned char const& r = 255,
                         unsigned char const& g = 0,
                         unsigned char const& b = 0);


  //: Write a style include LineStyle and PolyStyle
  static void write_kml_style(vcl_ofstream& ofs, 
                              vcl_string style_name = "kml_style",
                              double const& scale = 1.0,
                              double const& line_width = 3.0,
                              double const& alpha = 0.45,
                              unsigned char const& r = 0,
                              unsigned char const& g = 255,
                              unsigned char const& b = 0);

};

#endif
