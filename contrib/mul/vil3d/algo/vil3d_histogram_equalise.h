#ifndef vil3d_histogram_equalise_h_
#define vil3d_histogram_equalise_h_
//:
//  \file
//  \brief Apply histogram equalisation to images
//  \author Tim Cootes

#include <vil3d/vil3d_image_view.h>
#include <vxl_config.h>

//: Apply histogram equalisation to given image
void vil3d_histogram_equalise(vil3d_image_view<vxl_byte>& image);


#endif // vil3d_histogram_equalise_h_
