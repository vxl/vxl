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
#include <vnl/vnl_double_2.h>

class bkml_write
{
 public:

  //: Write KML header and open document tag
  static void open_document(vcl_ofstream& str);

  //: end document tag
  static void close_document(vcl_ofstream& str);

  //: Write a box
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr);
  
  //: Write a box with color, color is in hexadecimale format: 0 - 255 --> 00 to ff, aabbggrr --> alpha alpha, blue blue, gree green , red red.. alpha is the opacity, ffffffff is white fully opaque
  static void write_box(vcl_ofstream &ofs, vcl_string name, vcl_string description, vnl_double_2 ul, vnl_double_2 ur, vnl_double_2 ll, vnl_double_2 lr, vcl_string hex_color);

  //: Write a photooverlay without img and correct near parameter though)
  static void write_photo_overlay(vcl_ofstream& ofs, vcl_string name,
                                  double lon, double lat, double alt,
                                  double head, double tilt, double roll,
                                  double t_fov, double r_fov);
};

#endif
