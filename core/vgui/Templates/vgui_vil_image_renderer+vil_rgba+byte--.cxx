#include <vgui/vgui_vil_image_renderer.txx>
#include <vxl_config.h>
#include <vil/vil_rgba.h>

typedef vil_rgba<vxl_byte> Type;

template class vgui_vil_image_renderer<Type>;
