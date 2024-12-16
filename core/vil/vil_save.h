// This is core/vil/vil_save.h
#ifndef vil_save_h_
#define vil_save_h_
//:
// \file
// \author    Ian Scott

#include "vil_fwd.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h>

//: Send a vil_image_view to disk, deducing format from filename
// \relatesalso vil_image_view
bool
vil_save(const vil_image_view_base &, const char * filename);

//: Send a vil_image_view to disk, given filename
// \relatesalso vil_image_view
bool
vil_save(const vil_image_view_base &, const char * filename, const char * file_format);

//: Send vil_image_resource to disk.
// \relatesalso vil_image_resource
bool
vil_save_image_resource(const vil_image_resource_sptr & ir, const char * filename, const char * file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relatesalso vil_image_resource
bool
vil_save_image_resource(const vil_image_resource_sptr & ir, const char * filename);

//: Given a filename, guess the file format tag
// The returned pointer may point into the filename string - so keep it valid.
const char *
vil_save_guess_file_format(const char * filename);


#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//: Send a vil_image_view to disk, deducing format from filename
// \relatesalso vil_image_view
bool
vil_save(const vil_image_view_base &, const wchar_t * filename);

//: Send a vil_image_view to disk, given filename
// \relatesalso vil_image_view
bool
vil_save(const vil_image_view_base &, const wchar_t * filename, const wchar_t * file_format);

//: Send vil_image_resource to disk.
// \relatesalso vil_image_resource
bool
vil_save_image_resource(const vil_image_resource_sptr & ir, const wchar_t * filename, const wchar_t * file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relatesalso vil_image_resource
bool
vil_save_image_resource(const vil_image_resource_sptr & ir, const wchar_t * filename);

//: Given a filename, guess the file format tag
// The returned pointer may point into the filename string - so keep it valid.
const wchar_t *
vil_save_guess_file_format(const wchar_t * filename);
#endif // defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#endif // vil_save_h_
