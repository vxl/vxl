// This is vxl/vil2/vil2_save.h
#ifndef vil2_save_h_
#define vil2_save_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    Ian Scott

#include <vil2/vil2_image_view_base.h>

//: Send vil_image to disk, deducing format from filename
// \relates vil2_image_view
bool vil2_save(const vil2_image_view_base &, char const* filename);

//: Send vil_image to disk, given filename
// \relates vil2_image_view
bool vil2_save(const vil2_image_view_base &, char const* filename, char const* file_format);


#endif // vil_save_h_
