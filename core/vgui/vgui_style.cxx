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

vgui_style::vgui_style() {
  for (int i=0; i<2; ++i)
    rgba[i] = 1;

  rgba[3] = 1;
  line_width = 1;
  point_size = 1;
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
  vgui_macro_report_errors;
  glColor3f( rgba[0], rgba[1], rgba[2] );
  vgui_macro_report_errors;
}

void
vgui_style::apply_line_width() const
{
  if( line_width > 0.0 ) {
    vgui_macro_report_errors;
    glLineWidth( line_width );
    vgui_macro_report_errors;
  }
}

void
vgui_style::apply_point_size() const
{
  if( point_size > 0.0 ) {
    vgui_macro_report_errors;
    glPointSize( point_size );
    vgui_macro_report_errors;
  }
}

void
vgui_style::apply_all() const
{
  apply_color();
  apply_point_size();
  apply_line_width();
}
