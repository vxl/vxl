#ifndef INSTANTIATE_TEMPLATES
#include <vepl/vepl_convert.txx>
#include <vil/vil_rgb.h>
typedef unsigned char ubyte;
template vil_image vepl_convert(vil_image const&, vil_rgb<ubyte>);
#endif
