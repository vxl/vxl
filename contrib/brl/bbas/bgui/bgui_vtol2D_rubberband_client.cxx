#include "bgui_vtol2D_rubberband_client.h"
//:
// \file

#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_vertex_2d.h>
#include <bgui/bgui_vtol2D_tableau.h>

void
bgui_vtol2D_rubberband_client::
add_point(float x, float y)
{
  vtol2D_->add_point(x,y);
}

void
bgui_vtol2D_rubberband_client::
add_line(float x0, float y0, float x1, float y1)
{
  vtol2D_->add_line(x0, y0, x1, y1);
}

void
bgui_vtol2D_rubberband_client::
add_infinite_line(float a, float b, float c)
{
  vtol2D_->add_infinite_line(a, b, c);
}

void
bgui_vtol2D_rubberband_client::
add_circle(float x, float y, float r)
{
  vtol2D_->add_circle(x, y, r);
}

void
bgui_vtol2D_rubberband_client::
add_linestrip(int n, float const* x, float const* y)
{
  vtol2D_->add_linestrip(n, x, y);
}

void
bgui_vtol2D_rubberband_client::
add_polygon(int n, float const* x, float const* y)
{
  vcl_vector<vtol_vertex_sptr> verts;
  for (int i=0; i+1<n; ++i)
    verts.push_back(new vtol_vertex_2d(x[i], y[i]));
  vtol_face_2d_sptr f2d = new vtol_face_2d(verts);
  vtol2D_->add_face(f2d);
  vtol2D_->set_temp(f2d->cast_to_face());
}

void
bgui_vtol2D_rubberband_client::
add_box(float x0, float y0, float x1, float y1)
{
  vtol_vertex_sptr v0 = new vtol_vertex_2d(x0, y0);
  vtol_vertex_sptr v1 = new vtol_vertex_2d(x1, y0);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(x1, y1);
  vtol_vertex_sptr v3 = new vtol_vertex_2d(x0, y1);
  vcl_vector<vtol_vertex_sptr> verts;
  verts.push_back(v0);   verts.push_back(v1);
  verts.push_back(v2);   verts.push_back(v3);
  vtol_face_2d_sptr box = new vtol_face_2d(verts);
  vtol2D_->add_face(box);
  vtol2D_->set_temp(box->cast_to_face());
}

void
bgui_vtol2D_rubberband_client::
clear_highlight()
{
  vtol2D_->highlight(0);
}
