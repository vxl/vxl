#include <vgui/vgui_vil_image_renderer.txx>
#include <vil/vil_rgb.h>

typedef vil_rgb<double> Type;

template class vgui_vil_image_renderer<Type>;
