// This is core/vil/vil_save.h
#ifndef vil_save_h_
#define vil_save_h_
//:
// \file
// \author    Ian Scott

#include <vil/vil_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

//: Send a vil_image_view to disk, deducing format from filename
// \relatesalso vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename);

//: Send a vil_image_view to disk, given filename
// \relatesalso vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename, char const* file_format);

//: Send vil_image_resource to disk.
// \relatesalso vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename,
                             char const* file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relatesalso vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename);

//: Given a filename, guess the file format tag
// The returned pointer may point into the filename string - so keep it valid.
char const *vil_save_guess_file_format(char const* filename);


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//: Send a vil_image_view to disk, deducing format from filename
// \relatesalso vil_image_view
bool vil_save(const vil_image_view_base &, wchar_t const* filename);

//: Send a vil_image_view to disk, given filename
// \relatesalso vil_image_view
bool vil_save(const vil_image_view_base &, wchar_t const* filename, wchar_t const* file_format);

//: Send vil_image_resource to disk.
// \relatesalso vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, wchar_t const* filename,
                             wchar_t const* file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relatesalso vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, wchar_t const* filename);

//: Given a filename, guess the file format tag
// The returned pointer may point into the filename string - so keep it valid.
wchar_t const *vil_save_guess_file_format(wchar_t const* filename);
#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#endif // vil_save_h_
