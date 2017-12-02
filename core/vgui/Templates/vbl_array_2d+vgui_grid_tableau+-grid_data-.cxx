#include <vbl/vbl_array_2d.hxx>
#include <vgui/vgui_grid_tableau.h>

std::ostream &operator<<(std::ostream &os, vgui_grid_tableau::grid_data const &)
{
  return os << "piglet";
}

VBL_ARRAY_2D_INSTANTIATE(vgui_grid_tableau::grid_data);
