//-*- c++ -*-------------------------------------------------------------------
#ifndef GXFileVisitor_h_
#define GXFileVisitor_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : GXFileVisitor
//
// .SECTION Description
//    GXFileVisitor is a class that awf hasn't documented properly. FIXME
//
// .NAME        GXFileVisitor - Undocumented class FIXME
// .LIBRARY     POX
// .HEADER	Oxford Package
// .INCLUDE     oxp/GXFileVisitor.h
// .FILE        GXFileVisitor.h
// .FILE        GXFileVisitor.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 May 99
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iosfwd.h>

class GXFileVisitor {
public:
  GXFileVisitor();

  virtual ~GXFileVisitor() {}

  bool visit(char const* filename);
  bool visit(istream&);

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

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS GXFileVisitor.

