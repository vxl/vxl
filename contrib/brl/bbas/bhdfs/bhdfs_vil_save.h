// This is contrib/brl/bbas/bhdfs/bhdfs_vil_save.h
#ifndef bhdfs_vil_save_h_
#define bhdfs_vil_save_h_
//:
// \file
// \author    Ozge C. Ozcanli, Dec 08, 2011

#include <vil/vil_save.h>

//: Send a vil_image_view to disk, deducing format from filename
// \relatesalso vil_image_view
bool bhdfs_vil_save(const vil_image_view_base &, char const* filename);

//: Send a vil_image_view to disk, given filename
// \relatesalso vil_image_view
bool bhdfs_vil_save(const vil_image_view_base &, char const* filename, char const* file_format);

//: Send vil_image_resource to disk.
// \relatesalso vil_image_resource
bool bhdfs_vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename,
                             char const* file_format);

//: Save vil_image_resource to file, deducing format from filename.
// \relatesalso vil_image_resource
bool bhdfs_vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename);

#endif // bhdfs_vil_save_h_
