#ifndef mil_invert_image_2d_h_
#define mil_invert_image_2d_h_
//:
//  \file
//  \brief Functions to convert images from one type to another
//  \author Tim Cootes

#include <mil/mil_image_2d_of.h>


//: creates an inverted version of an input image  (i.e. white->black, black->white) 
void mil_invert_image_2d(mil_image_2d_of<float>& dest,
                         const mil_image_2d_of<float>& src);

#endif // mil_invert_image_2d_h_
