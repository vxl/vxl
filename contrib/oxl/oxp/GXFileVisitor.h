// This is oxl/oxp/GXFileVisitor.h
#ifndef GXFileVisitor_h_
#define GXFileVisitor_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//
// .NAME    GXFileVisitor
// .LIBRARY oxp
// .HEADER  Oxford Package
// .INCLUDE oxp/GXFileVisitor.h
// .FILE    GXFileVisitor.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 May 99
//
//-----------------------------------------------------------------------------

#include <vcl_iosfwd.h>

class GXFileVisitor
{
 public:
  GXFileVisitor();

  virtual ~GXFileVisitor() {}

  bool visit(char const* filename);
  bool visit(vcl_istream&);

  virtual bool point(char const* type, float x, float y);
  virtual bool polyline(float const* x, float const* y, int n);
  virtual bool text(float x, float y, char const* text);

  virtual bool set_color(float r, float g, float b);
  virtual bool set_point_radius(float r);
  virtual bool set_line_width(float w);

  // State variables
  float point_radius;
  float line_width;
  float color[3];

  // Global flags
  static bool do_text;
  static bool do_antialias;
};

#endif // GXFileVisitor_h_
