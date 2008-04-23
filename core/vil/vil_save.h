// This is core/vil/vil_save.h
#ifndef vil_save_h_
#define vil_save_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    Ian Scott

#include <vil/vil_fwd.h>

//: Send a vil_image_view to disk, deducing format from filename
// \relates vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename);

//: Send a vil_image_view to disk, given filename
// \relates vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename, char const* file_format);

//: Send vil_image_resource to disk.
// \relates vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename,
                             char const* file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relates vil_image_resource
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename);

//: Given a filename, guess the file format tag
// The returned pointer may point into the filename string - so keep it valid.
char const *vil_save_guess_file_format(char const* filename);

#endif // vil_save_h_
