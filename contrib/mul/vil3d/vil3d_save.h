// This is mul/vil3d/vil3d_save.h
#ifndef vil3d_save_h_
#define vil3d_save_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    Ian Scott

#include <vil3d/vil3d_fwd.h>

//: Send a vil3d_image_view to disk, deducing format from filename
// \relates vil3d_image_view
bool vil3d_save(const vil3d_image_view_base &, char const* filename);

//: Send a vil3d_image_view to disk, given filename
// \relates vil3d_image_view
bool vil3d_save(const vil3d_image_view_base &, char const* filename, char const* file_format);

//: Send a vil3d_image_view to disk, deducing format from filename
//  Utility function, allowing definition of voxel widths in header info.
// \relates vil3d_image_view
bool vil3d_save(const vil3d_image_view_base & im, 
                float voxel_width_i,
                float voxel_width_j,
                float voxel_width_k,
                char const* filename);


//: Send vil3d_image_resource to disk.
// \relates vil3d_image_resource
bool vil3d_save_image_resource(const vil3d_image_resource_sptr &ir, char const* filename,
                             char const* file_format);

//: Save vil3d_image_resource to file, deducing format from filename.
// \relates vil3d_image_resource
bool vil3d_save_image_resource(const vil3d_image_resource_sptr &ir, char const* filename);

//: Guess file format from filename.
char const *vil3d_save_guess_file_format(char const* filename);

#endif // vil3d_save_h_
