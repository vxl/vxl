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
//   <none yet>
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
};

#endif
