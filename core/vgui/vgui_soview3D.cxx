// This is core/vgui/vgui_soview3D.cxx
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Mar 99
// \brief  See vgui_soview3D.h for a description of this file.

#include <iostream>
#include "vgui_soview3D.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vgui/vgui_gl.h"

//--------------------------------------------------------------------------//

std::ostream &
vgui_point3D::print(std::ostream & s) const
{
  s << "[vgui_point3D " << x << ',' << y << ',' << z << ' ';
  return vgui_soview3D::print(s) << ']';
}

void
vgui_point3D::draw() const
{
  bool lighting = false;
  if (glIsEnabled(GL_LIGHTING))
  {
    lighting = true;
    glDisable(GL_LIGHTING);
  }

  // glPointSize(style->point_size);
  glBegin(GL_POINTS);
  glVertex3f(x, y, z);
  glEnd();


  if (lighting)
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------//

std::ostream &
vgui_lineseg3D::print(std::ostream & s) const
{
  s << "[vgui_lineseg3D " << x0 << ',' << y0 << ',' << z0 << " - " << x1 << ',' << y1 << ',' << z1 << ' ';
  return vgui_soview3D::print(s) << ']';
}

void
vgui_lineseg3D::draw() const
{
#ifdef DEBUG
  std::cerr << "vgui_lineseg3D::draw() line id=" << id << '\n';
#endif

  bool lighting = false;
  if (glIsEnabled(GL_LIGHTING))
  {
    lighting = true;
    glDisable(GL_LIGHTING);
  }

  // glLineWidth(style->line_width);
  glBegin(GL_LINES);
  glVertex3f(x0, y0, z0);
  glVertex3f(x1, y1, z1);
  glEnd();

  if (lighting)
    glEnable(GL_LIGHTING);
}

//--------------------------------------------------------------------------//

std::ostream &
vgui_triangle3D::print(std::ostream & s) const
{
  s << "[vgui_triangle3D " << x0 << ',' << y0 << ',' << z0 << " - " << x1 << ',' << y1 << ',' << z1 << " - " << x2
    << ',' << y2 << ',' << z2 << ' ';
  return vgui_soview3D::print(s) << ']';
}

void
vgui_triangle3D::draw() const
{
  bool lighting = false;
  if (glIsEnabled(GL_LIGHTING))
  {
    lighting = true;
    glDisable(GL_LIGHTING);
  }

  glBegin(GL_TRIANGLES);
  glVertex3f(x0, y0, z0);
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y2, z2);
  glEnd();

  if (lighting)
    glEnable(GL_LIGHTING);
}
