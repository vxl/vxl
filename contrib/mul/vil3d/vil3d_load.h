// This is contrib/mul/vil3d/vil3d_load.h
#ifndef vil3d_load_h_
#define vil3d_load_h_

//:
// \file
// \brief Functions to read an image from a file
// \author Tim Cootes

#include <vil3d/vil3d_file_format.h>

//: Load image from named path
//  Attempts to load with each of available file format readers
//  Use vil3d_file_format::add_format() to add additional format options.
bool vil3d_load(vil3d_header_data_sptr& header,
                vil3d_image_view_base_sptr& image,
								const vcl_string& path);

//: Utility function to load image (and discard image header information)
//  Returns empty image if unable to load from named path
vil3d_image_view_base_sptr vil3d_load(const vcl_string& path);

#endif



