// This is core/vgui/vgui_style.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief  See vgui_style.h for a description of this file.
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   18 Oct 99
//
// \verbatim
//  Modifications
//   18-OCT-1999 P.Pritchett - Initial version.
// \endverbatim

#include "vgui_style.h"
#include "vgui_macro.h"
#include "vgui_gl.h"


//: Create a new style object
vgui_style_sptr
vgui_style::new_style()
{
  return vgui_style_sptr(new vgui_style());
}

//: Create a new style object
vgui_style_sptr
vgui_style::new_style(float r, float g, float b, float point_size, float line_width)
{
  return vgui_style_sptr(new vgui_style(r, g, b, point_size, line_width));
}

//: Create a new style object from an existing one;
vgui_style_sptr
vgui_style::new_style(const vgui_style_sptr& s)
{
  return vgui_style_sptr( new vgui_style( s->rgba[0],s->rgba[1],s->rgba[2],
                                          s->point_size,s->line_width ) );
}
  

vgui_style::vgui_style() {
  for (int i=0; i<2; ++i)
    rgba[i] = 1;

  rgba[3] = 1;
  line_width = 1;
  point_size = 1;
}


//: Constructor - creates a style and initializes the values
vgui_style::vgui_style(float r, float g, float b, float ps, float lw)
 : point_size(ps), line_width(lw)
{
  rgba[0] = r;
  rgba[1] = g;
  rgba[2] = b;
  rgba[3] = 1;
}


vgui_style::~vgui_style() {
  // to aid in debugging destroyed styles
  for (int i=0; i<2; ++i)
    rgba[i] = -1.0f;

  rgba[3] = -1.0f;
  line_width = -1.0f;
  point_size = -1.0f;
}

void
vgui_style::apply_color() const
{
  glColor3f( rgba[0], rgba[1], rgba[2] );
}

void
vgui_style::apply_line_width() const
{
  if( line_width > 0.0 ) 
    glLineWidth( line_width );
}

void
vgui_style::apply_point_size() const
{
  if( point_size > 0.0 ) 
    glPointSize( point_size );
}

void
vgui_style::apply_all() const
{
  apply_color();
  apply_point_size();
  apply_line_width();
}
