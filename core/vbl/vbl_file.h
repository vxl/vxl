#ifndef vbl_file_h_
#define vbl_file_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME	vbl_file
// .LIBRARY	vbl
// .HEADER	vxl package
// .INCLUDE	vbl/vbl_file.h
// .FILE	vbl_file.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 02 Nov 98
// .SECTION Modifications
//   981102 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_string.h>

//: A collection of miscellaneous filesystem-type utilities
//
struct vbl_file {

  // -- Return current working directory
  static vcl_string get_cwd();

  // -- Make a writable directory.  You might imagine mkdir would be a
  // better name, and then you might imagine a world w/out ms.
  static bool make_directory(char const* filename);
  static bool make_directory(vcl_string const& filename) {
    return make_directory(filename.c_str());
  }

  // -- Return true iff filename is a directory.
  static bool is_directory(char const* filename);
  static bool is_directory(const vcl_string& filename) {
    return is_directory(filename.c_str());
  }

  // -- Expand any leading ~ escapes in filename
  static vcl_string expand_tilde(char const* filename);
  static vcl_string expand_tilde(vcl_string const& filename) {
    return expand_tilde(filename.c_str());
  }

  // -- Return true iff filename exists.  It may be any sort of file.
  static int exists(char const* filename);

  // -- Return size of vbl_file
  static int size(char const* filename);

  // -- Return dirname
  static vcl_string dirname(char const* filename);
  static vcl_string dirname(vcl_string const& filename) {
    return dirname(filename.c_str());
  }

  // -- Return basename
  static vcl_string basename(char const* filename, char const* suffix = 0);
};

#endif // vbl_file_h_
