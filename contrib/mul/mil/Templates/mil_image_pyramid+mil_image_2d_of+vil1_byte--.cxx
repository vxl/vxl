#include <vil1/vil1_byte.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_image_pyramid.txx>

typedef mil_image_2d_of<vil1_byte> im;

MIL_IMAGE_PYRAMID_INSTANTIATE(im );

