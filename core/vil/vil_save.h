// This is core/vil/vil_save.h
#ifndef vil_save_h_
#define vil_save_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    Ian Scott

#include <vil/vil_image_view_base.h>

//: Send a vil_image_view to disk, deducing format from filename
// \relates vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename);

//: Send a vil_image_view to disk, given filename
// \relates vil_image_view
bool vil_save(const vil_image_view_base &, char const* filename, char const* file_format);

#endif // vil_save_h_
