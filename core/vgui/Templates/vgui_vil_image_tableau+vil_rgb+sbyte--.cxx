#include <vgui/vgui_vil2_image_tableau.txx>
#include <vxl_config.h>
#include <vil2/vil2_rgb.h>

typedef vil2_rgb<vxl_sbyte> Type;

template class vgui_vil2_image_tableau<Type>;
