// This is mul/vil3d/vil3d_load.h
#ifndef vil3d_load_h_
#define vil3d_load_h_

//:
// \file
// \brief Functions to read an image from a file
// \author Ian Scott

#include <vil3d/vil3d_image_resource.h>



//: Load image from named path
//  Attempts to load with each of available file format readers
//  Use vil3d_file_format::add_format() to add additional format options.
vil3d_image_resource_sptr vil3d_load_image_resource(char const* filename);


//: Load a 3D image off file fully into memory.
//  Returns empty image if unable to load from named path
//  Use vil3d_file_format::add_format() to add additional format options.
vil3d_image_view_base_sptr vil3d_load(const char *);

#endif
