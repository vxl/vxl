#ifndef INSTANTIATE_TEMPLATES
#include <vepl2/vepl2_convert.txx>
#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte
template vil2_image_view_base vepl2_convert(vil2_image_view_base const&, vil_rgb<vxl_byte>);
#endif
