#include <vgui/vgui_vil2_image_tableau.txx>
#include <vxl_config.h>
#include <vil2/vil2_rgb.h>

typedef vil2_rgb<vxl_uint_32> Type;

template class vgui_vil2_image_tableau<Type>;
