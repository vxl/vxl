// This is core/vul/vul_file.h
#ifndef vul_file_h_
#define vul_file_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A collection of miscellaneous filesystem-type utilities
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   02 Nov 98
//
// \verbatim
// Modifications
// 981102 AWF Initial version.
// PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Jun.2003 - Ian Scott      - added support for '\' file separator to dos version
// \endverbatim

#include <vcl_string.h>

//: A collection of miscellaneous filesystem-type utilities
//
struct vul_file
{
  //: Return current working directory
  static vcl_string get_cwd();

  //: change current working directory
  static bool change_directory(char const* dirname);
  static bool change_directory(vcl_string const& dirname) {
    return change_directory(dirname.c_str());
  }
  
  //: Make a writable directory.
  // You might imagine mkdir would be a better name,
  // and then you might imagine a world w/out ms.
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

  //: Return size of vul_file
  static int size(char const* filename);
  static int size(vcl_string filename) { return size(filename.c_str()); }

  //: Return dirname
  static vcl_string dirname(char const* filename);
  static vcl_string dirname(vcl_string const& filename) {
    return dirname(filename.c_str());
  }

  //: Return extension (including the '.').
  static vcl_string extension(char const* filename);
  static vcl_string extension(vcl_string const& filename) {
    return extension( filename.c_str() );
  }

  //: Return basename
  static vcl_string basename(char const* filename, char const* suffix = 0);
  static vcl_string basename(vcl_string const& filename, char const* suffix = 0) {
    return basename(filename.c_str() );
  }

  //: Strips away directory of the filename
  static vcl_string strip_directory(char const* filename);
  static vcl_string strip_directory(vcl_string const &filename)
  { return strip_directory(filename.c_str()); }

  //: Strips away extension of the filename
  static vcl_string strip_extension(char const* filename);
  static vcl_string strip_extension(vcl_string const &filename)
  { return strip_extension(filename.c_str()); }

  //: Delete 1 or more files using the Local OS preferred globbing.
  // E.g. \c delete_file_glob("*"); will delete all the files in the
  // current directory on most operating systems.
  // \return true if successful.
  static bool delete_file_glob(char const* file_glob);
};

inline bool vul_file_exists(char const *f) { return vul_file::exists(f); }
inline bool vul_file_exists(vcl_string  f) { return vul_file::exists(f); }

inline bool vul_file_is_directory(char const *f) { return vul_file::is_directory(f); }
inline bool vul_file_is_directory(vcl_string  f) { return vul_file::is_directory(f); }

inline int vul_file_size(char const *f) { return vul_file::size(f); }
inline int vul_file_size(vcl_string  f) { return vul_file::size(f); }

inline vcl_string vul_file_extension(char const *f) { return vul_file::extension(f); }
inline vcl_string vul_file_extension(vcl_string  f) { return vul_file_extension(f.c_str()); }

#endif // vul_file_h_
