// This is brl/bbas/bhdfs/bhdfs_vil_load.h
#ifndef bhdfs_vil_load_h_
#define bhdfs_vil_load_h_
//:
// \file
// \brief read an image from an hdfs file
//
// bhdfs_vil_load returns a pointer to the base class of a vil_image_view object.
// You can then cast, or assign it to the appropriate type. You must not forget to
// delete the created view.
//
// bhdfs_vil_load_image_resource() returns a smart pointer to vil_image_resource which
// will "read an image from a file". In practice
// it may or may not read the image on creation, but a disk read will take place at
// some point before get_view() returns.
//
// \author Ozge C. Ozcanli, Dec 08, 2011
//
// \verbatim
//  Modifications
//
//\endverbatim

#include <vil/vil_fwd.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vxl_config.h>

//: Load an image resource object from a file.
// \relatesalso vil_image_resource
vil_image_resource_sptr bhdfs_vil_load_image_resource(char const* filename,
                                                      bool verbose = true);

//: Load an image resource object from a file.
// Won't use plugins.
// \relatesalso vil_image_resource
vil_image_resource_sptr bhdfs_vil_load_image_resource_raw(char const*,
                                                          bool verbose = true);

#if 0
//: Load from a filename with a plugin.
// \relatesalso vil_image_resource
vil_image_resource_sptr bhdfs_vil_load_image_resource_plugin(char const*);
#endif

#if 0
//: Load a pyramid image resource object from a file or directory.
// \relatesalso vil_pyramid_image_resource
vil_pyramid_image_resource_sptr
bhdfs_vil_load_pyramid_resource(char const* directory_or_file, bool verbose = true);
#endif

//: Convenience function for loading an image into an image view.
// \relatesalso vil_image_view
vil_image_view_base_sptr bhdfs_vil_load(const char *, bool verbose = true);

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#if 0
//: Load an image resource object from a file.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_load_image_resource(wchar_t const* filename, bool verbose = true);
#endif

#if 0
//: Load an image resource object from a file.
// Won't use plugins.
vil_image_resource_sptr vil_load_image_resource_raw(wchar_t const*, bool verbose = true);
#endif

#if 0
//: Convenience function for loading an image into an image view.
// \relatesalso vil_image_view
vil_image_view_base_sptr vil_load(const wchar_t *, bool verbose = true);
#endif

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T

#endif // bhdfs_vil_load_h_
