#include "vxl_config.h"
#if VXL_HAS_INT_64
#  include "vil/vil_rgb.h"
#  include "vil/vil_image_view.hxx"
VIL_IMAGE_VIEW_INSTANTIATE(vil_rgb<vxl_uint_64>);
#endif
