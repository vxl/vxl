#include <vil/vil_rgb.h>
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_y_gradient.txx>

template class vipl_y_gradient<vil_image, vil_image, vil_rgb<unsigned char>, vil_rgb<unsigned char>, vipl_trivial_pixeliter>;
