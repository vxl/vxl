#ifndef INSTANTIATE_TEMPLATES
#include <vepl2/vepl2_convert.txx>
#include <vil2/vil2_rgb.h>
#include <vxl_config.h> // for vxl_byte
template vil2_image_view_base vepl2_convert(vil2_image_view_base const&, vil2_rgb<vxl_byte>);
#endif
