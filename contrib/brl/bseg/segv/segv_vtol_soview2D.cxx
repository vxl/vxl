#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <vgl/vgl_distance.h>
#include <vnl/vnl_math.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>
#include <segv/segv_vtol_soview2D.h>

//--------------------------------------------------------------------------//
//: vtol_vertex_2d view
vcl_ostream& segv_vtol_soview2D_vertex::print(vcl_ostream& s) const
{
  s << "[segv_vtol_soview2D_vertex " << x << "," << y << " ";
  s << " "; return vgui_soview2D::print(s) << "]";
}


// //--------------------------------------------------------------------------//

// void vgui_soview2D_group::set_style(vgui_style *s)
// {
//   for (unsigned int i=0; i< ls.size(); i++)
//     if (!ls[i]->get_style())
//       ls[i]->set_style(s);

//   vgui_soview::set_style( s);
// }

// vcl_ostream& vgui_soview2D_group::print(vcl_ostream& s) const
// {
//   s << "[vgui_soview2D_group ";

//   for (unsigned int i=0; i< ls.size(); i++)
//     ls[i]->print(s);

//   return vgui_soview2D::print(s) << "]";
// }

// void vgui_soview2D_group::draw()
// {
//   for (unsigned int i=0; i< ls.size(); i++)
//     ls[i]->draw();
// }

// float vgui_soview2D_group::distance_squared(float x, float y)
// {
//   if (ls.size() == 0)
//     return -1e30f;

//   float min= ls[0]->distance_squared( x, y);

//   for (unsigned int i=1; i< ls.size(); i++)
//   {
//     float d= ls[i]->distance_squared( x, y);
//     if ( d< min ) min= d;
//   }

//   return min;
// }

// void vgui_soview2D_group::get_centroid(float* x, float* y)
// {
//   *x = 0;
//   *y = 0;
//   int n = ls.size();

//   for (int i=0; i < n; i++)
//   {
//     float cx, cy;
//     ls[i]->get_centroid(&cx, &cy);
//     *x += cx;
//     *y += cy;
//   }

//   float s = 1.0f/n;
//   *x *= s;
//   *y *= s;
// }

// void vgui_soview2D_group::translate(float tx, float ty)
// {
//   for (unsigned int i=0; i < ls.size(); i++)
//     ls[i]->translate(tx, ty);
// }


