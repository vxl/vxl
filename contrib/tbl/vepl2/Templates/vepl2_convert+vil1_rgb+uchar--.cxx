#ifndef INSTANTIATE_TEMPLATES
#include <vepl2/vepl2_convert.txx>
#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte
template vil_image_resource vepl2_convert(vil_image_resource_sptr, vil_rgb<vxl_byte>);
#endif
