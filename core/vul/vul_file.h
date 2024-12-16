// This is core/vul/vul_file.h
#ifndef vul_file_h_
#define vul_file_h_
//:
// \file
// \brief A collection of miscellaneous filesystem-type utilities
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   02 Nov 98
//
// \verbatim
//  Modifications
//   981102 AWF Initial version.
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Jun.2003 - Ian Scott      - added support for '\' file separator to dos version
// \endverbatim

#include <string>
#include <ctime>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>
//: A collection of miscellaneous filesystem-type utilities
//
struct vul_file
{
  //: Return current working directory
  static std::string
  get_cwd();

  //: change current working directory
  static bool
  change_directory(const char * dirname);
  static bool
  change_directory(const std::string & dirname)
  {
    return change_directory(dirname.c_str());
  }

  //: Make a writable directory.
  // You might imagine mkdir would be a better name,
  // and then you might imagine a world w/out ms.
  static bool
  make_directory(const char * filename);
  static bool
  make_directory(const std::string & filename)
  {
    return make_directory(filename.c_str());
  }

  //: Make a writable directory, including any necessary parents.
  // Returns true if successful, or if the directory already exists.
  static bool
  make_directory_path(const char * filename);
  static bool
  make_directory_path(const std::string & filename)
  {
    return make_directory_path(filename.c_str());
  }

  //: Return true iff filename is a directory.
  static bool
  is_directory(const char * filename);
  static bool
  is_directory(const std::string & filename)
  {
    return is_directory(filename.c_str());
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  //: Return true iff filename is a drive, e.g., "c:" or "Z:".
  static bool
  is_drive(const char * filename);
  static bool
  is_drive(const std::string & filename)
  {
    return is_drive(filename.c_str());
  }
#endif

  //: Expand any leading ~ escapes in filename
  static std::string
  expand_tilde(const char * filename);
  static std::string
  expand_tilde(const std::string & filename)
  {
    return expand_tilde(filename.c_str());
  }

  //: Return true iff filename exists.  It may be any sort of file.
  static bool
  exists(const char * filename);
  static bool
  exists(const std::string & filename)
  {
    return exists(filename.c_str());
  }

  //: Return size of vul_file
  static unsigned long
  size(const char * filename);
  static unsigned long
  size(std::string filename)
  {
    return size(filename.c_str());
  }

  //: Return dirname
  static std::string
  dirname(const char * filename);
  static std::string
  dirname(const std::string & filename)
  {
    return dirname(filename.c_str());
  }

  //: Return extension (including the '.').
  static std::string
  extension(const char * filename);
  static std::string
  extension(const std::string & filename)
  {
    return extension(filename.c_str());
  }

  //: Return basename
  // Only strip specified suffix.
  static std::string
  basename(const char * filename, const char * suffix = nullptr);
  static std::string
  basename(const std::string & filename, const char * suffix = nullptr)
  {
    return basename(filename.c_str(), suffix);
  }

  //: Strips away directory of the filename
  static std::string
  strip_directory(const char * filename);
  static std::string
  strip_directory(const std::string & filename)
  {
    return strip_directory(filename.c_str());
  }

  //: Strips away extension of the filename
  static std::string
  strip_extension(const char * filename);
  static std::string
  strip_extension(const std::string & filename)
  {
    return strip_extension(filename.c_str());
  }

  //: Delete 1 or more files using the Local OS preferred globbing.
  // E.g. \c delete_file_glob("*"); will delete all the files in the
  // current directory on most operating systems.
  // \return true if successful.
  static bool
  delete_file_glob(const std::string & file_glob);
  static bool
  delete_file_glob(const char * file_glob)
  {
    return delete_file_glob(std::string(file_glob));
  }


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T

  //: Return current working directory
  //  This function is provided as an overloading
  static std::string
  get_cwd(char * /*dummy*/)
  {
    return get_cwd();
  }

  //: Return current working directory
  static std::wstring
  get_cwd(wchar_t * dummy);

  //: change current working directory
  static bool
  change_directory(const wchar_t * dirname);
  static bool
  change_directory(const std::wstring & dirname)
  {
    return change_directory(dirname.c_str());
  }

  //: Make a writable directory.
  // You might imagine mkdir would be a better name,
  // and then you might imagine a world w/out ms.
  static bool
  make_directory(const wchar_t * filename);
  static bool
  make_directory(const std::wstring & filename)
  {
    return make_directory(filename.c_str());
  }

  //: Make a writable directory, including any necessary parents.
  // Returns true if successful, or if the directory already exists.
  static bool
  make_directory_path(const wchar_t * filename);
  static bool
  make_directory_path(const std::wstring & filename)
  {
    return make_directory_path(filename.c_str());
  }

  //: Return true iff filename is a directory.
  static bool
  is_directory(const wchar_t * filename);
  static bool
  is_directory(const std::wstring & filename)
  {
    return is_directory(filename.c_str());
  }

  //: Return true iff filename is a drive, e.g., "c:" or "Z:".
  static bool
  is_drive(const wchar_t * filename);
  static bool
  is_drive(const std::wstring & filename)
  {
    return is_drive(filename.c_str());
  }

  ////: Expand any leading ~ escapes in filename
  static std::wstring
  expand_tilde(const wchar_t * filename)
  {
    // ~ meaningless on win32
    return std::wstring(filename);
  }
  static std::wstring
  expand_tilde(const std::wstring & filename)
  {
    // ~ meaningless on win32
    return filename;
  }

  //: Return true iff filename exists.  It may be any sort of file.
  static bool
  exists(const wchar_t * filename);
  static bool
  exists(const std::wstring & filename)
  {
    return exists(filename.c_str());
  }

  ////: Return size of vul_file
  // static unsigned long size(wchar_t const* filename);
  // static unsigned long size(std::wstring filename) { return size(filename.c_str()); }

  //: Return dirname
  static std::wstring
  dirname(const wchar_t * filename);
  static std::wstring
  dirname(const std::wstring & filename)
  {
    return dirname(filename.c_str());
  }

  //: Return extension (including the '.').
  static std::wstring
  extension(const wchar_t * filename);
  static std::wstring
  extension(const std::wstring & filename)
  {
    return extension(filename.c_str());
  }

  //: Return basename
  static std::wstring
  basename(const wchar_t * filename, const wchar_t * suffix = 0);
  static std::wstring
  basename(const std::wstring & filename, const wchar_t * suffix = 0)
  {
    return basename(filename.c_str(), suffix);
  }

  //: Strips away directory of the filename
  static std::wstring
  strip_directory(const wchar_t * filename);
  static std::wstring
  strip_directory(const std::wstring & filename)
  {
    return strip_directory(filename.c_str());
  }

  //: Strips away extension of the filename
  static std::wstring
  strip_extension(const wchar_t * filename);
  static std::wstring
  strip_extension(const std::wstring & filename)
  {
    return strip_extension(filename.c_str());
  }

#endif

  static std::time_t
  time_modified(char const * filename);
  static std::time_t
  time_modified(const std::string & filename)
  {
    return time_modified(filename.c_str());
  }
};

inline bool
vul_file_exists(const char * f)
{
  return vul_file::exists(f);
}
inline bool
vul_file_exists(std::string f)
{
  return vul_file::exists(f);
}

inline bool
vul_file_is_directory(const char * f)
{
  return vul_file::is_directory(f);
}
inline bool
vul_file_is_directory(std::string f)
{
  return vul_file::is_directory(f);
}

inline unsigned long
vul_file_size(const char * f)
{
  return vul_file::size(f);
}
inline unsigned long
vul_file_size(std::string f)
{
  return vul_file::size(f);
}

inline std::string
vul_file_extension(const char * f)
{
  return vul_file::extension(f);
}
inline std::string
vul_file_extension(std::string f)
{
  return vul_file_extension(f.c_str());
}

#endif // vul_file_h_
