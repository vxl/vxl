#ifndef vbl_file_h_
#define vbl_file_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_file.h

//:
// \file
// \brief A collection of miscellaneous filesystem-type utilities
// \author Andrew W. Fitzgibbon, Oxford RRG, 02 Nov 98
//
// \verbatim
// Modifications
// 981102 AWF Initial version.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim




#include <vcl_string.h>

//: A collection of miscellaneous filesystem-type utilities
//
struct vbl_file {

  //: Return current working directory
  static vcl_string get_cwd();

  //: Make a writable directory.  You might imagine mkdir would be a
  // better name, and then you might imagine a world w/out ms.
  static bool make_directory(char const* filename);
  static bool make_directory(vcl_string const& filename) {
    return make_directory(filename.c_str());
  }

  //: Return true iff filename is a directory.
  static bool is_directory(char const* filename);
  static bool is_directory(const vcl_string& filename) {
    return is_directory(filename.c_str());
  }

  //: Expand any leading ~ escapes in filename
  static vcl_string expand_tilde(char const* filename);
  static vcl_string expand_tilde(vcl_string const& filename) {
    return expand_tilde(filename.c_str());
  }

  //: Return true iff filename exists.  It may be any sort of file.
  static bool exists(char const* filename);
  static bool exists(vcl_string const& filename) {
    return exists(filename.c_str());
  }

  //: Return size of vbl_file
  static int size(char const* filename);

  //: Return dirname
  static vcl_string dirname(char const* filename);
  static vcl_string dirname(vcl_string const& filename) {
    return dirname(filename.c_str());
  }

  //: Return extension (including the '.').
  static vcl_string extension(char const* filename);

  //: Return basename
  static vcl_string basename(char const* filename, char const* suffix = 0);

  //: Strip aways parts of the filename
  static vcl_string strip_directory(char const* filename);
  static vcl_string strip_extension(char const* filename);
};

#endif // vbl_file_h_
