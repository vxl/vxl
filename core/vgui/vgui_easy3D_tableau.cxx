// This is oxl/vgui/vgui_easy3D_tableau.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Philip C. Pritchett, RRG, University of Oxford
// \date   24 Sep 99
// \brief  See vgui_easy3D_tableau.h for a description of this file.

#include "vgui_easy3D_tableau.h"

#include "vgui_soview3D.h"
#include "vgui_displaylist3D_tableau.h"
#include "vgui_style_factory.h"

vgui_easy3D_tableau::
vgui_easy3D_tableau()
{
  fg[0] = 0.0f;
  fg[1] = 1.0f;
  fg[2] = 0.0f;

  line_width = 1;
  point_size = 3;
}


void
vgui_easy3D_tableau::
add(vgui_soview3D* object)
{
  vgui_style *style =
    vgui_style_factory::instance()->get_style(fg[0], fg[1], fg[2],
                                              point_size, line_width);

  object->set_style(style);
  vgui_displaylist3D_tableau::add(object);
}


vgui_point3D*
vgui_easy3D_tableau::
add_point(float x, float y, float z)
{
  vgui_point3D *obj
    = new vgui_point3D( x,y,z );

  add(obj);
  return obj;
}


vgui_lineseg3D*
vgui_easy3D_tableau::
add_line( float x0, float y0, float z0,
          float x1, float y1, float z1 )
{
  vgui_lineseg3D *obj
    = new vgui_lineseg3D( x0,y0,z0, x1,y1,z1 );

  add(obj);
  return obj;
}

vgui_triangle3D*
vgui_easy3D_tableau::
add_triangle( float x0, float y0, float z0,
              float x1, float y1, float z1,
              float x2, float y2, float z2 )
{
  vgui_triangle3D *obj = 
    new vgui_triangle3D( x0,y0,z0, x1,y1,z1, x2,y2,z2 );

  add(obj);
  return obj;
}


void
vgui_easy3D_tableau::
set_foreground(float r, float g, float b)
{
  fg[0] = r;
  fg[1] = g;
  fg[2] = b;
}

void
vgui_easy3D_tableau::
set_line_width(float w)
{
  line_width = w;
}

void
vgui_easy3D_tableau::
set_point_radius(float r)
{
  point_size = r;
}

