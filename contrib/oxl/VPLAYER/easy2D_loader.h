#ifndef easy2D_loader_h_
#define easy2D_loader_h_
//--------------------------------------------------------------------------------
// .NAME easy2D_loader - Easy 2D loader
// .SECTION Description:
//   Loads geometry from GX file 
// .SECTION Author
//   Marko Bacic, Oxford RRG
// .SECTION Modifications:
//   Marko Bacic 07-SEP-2000    Initial version.
//--------------------------------------------------------------------------------

#include <vgui/vgui_easy2D.h>
#include <oxp/GXFileVisitor.h>

class easy2D_loader : public GXFileVisitor {
  vgui_easy2D_ref easy_;
public:
  easy2D_loader(vgui_easy2D_ref const& easy);
  bool point(char const* type, float x, float y);
  bool polyline(float const* x, float const* y, int n);
  bool set_color(float r, float g, float b);
  bool set_point_radius(float r);
  bool set_line_width(float w);
};
#endif // easy2D_loader_h_
