// This is oxl/vgui/vgui_soview3D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Mar 99
// \brief  See vgui_soview3D.h for a description of this file.

#include "vgui_soview3D.h"
#include <vcl_iostream.h>
#include <vgui/vgui_gl.h>

vgui_soview3D::vgui_soview3D() {}

//--------------------------------------------------------------------------//

vcl_ostream& vgui_point3D::print(vcl_ostream& s) const {
  s << "[vgui_point3D " << x << "," << y << "," << z << " ";
  s << " "; return vgui_soview3D::print(s) << "]";
}

void vgui_point3D::draw()
{
  bool lighting = false;
  if (glIsEnabled(GL_LIGHTING)) {
    lighting = true;
    glDisable(GL_LIGHTING);
  }

  //glPointSize(style->point_size);
  glBegin(GL_POINTS);
  glVertex3f(x,y,z);
  glEnd();


  if (lighting)
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------//

vcl_ostream& vgui_lineseg3D::print(vcl_ostream& s) const {
  s << "[vgui_lineseg3D " << x0 << "," << y0 << " -- " << x1 << "," << y1;
  s << " "; return vgui_soview3D::print(s) << "]";
}

void vgui_lineseg3D::draw() {
  //vcl_cerr << "line id : " << id << vcl_endl;

  bool lighting = false;
  if (glIsEnabled(GL_LIGHTING)) {
    lighting = true;
    glDisable(GL_LIGHTING);
  }

  //glLineWidth(style->line_width);
  glBegin(GL_LINES);
  glVertex3f(x0,y0,z0);
  glVertex3f(x1,y1,z1);
  glEnd();

  if (lighting)
    glEnable(GL_LIGHTING);
}

