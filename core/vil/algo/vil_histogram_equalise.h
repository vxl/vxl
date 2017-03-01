#ifndef vil_histogram_equalise_h_
#define vil_histogram_equalise_h_
//:
//  \file
//  \brief Apply histogram equalisation to images
//  \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vxl_config.h>

//: Apply histogram equalisation to given byte image
//  \relatesalso vil_image_view
void vil_histogram_equalise(vil_image_view<vxl_byte>& image);

//: Apply histogram equalisation to given float image
//  \relatesalso vil_image_view
void vil_histogram_equalise(vil_image_view<float>& image);

#endif // vil_histogram_equalise_h_
