//:
// \file
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <bgui/bgui_vsol_soview2D.h>

//--------------------------------------------------------------------------
//: vsol_point_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vsol_soview2D_point::print(vcl_ostream& s) const
{
s << "[bgui_vsol_soview2D_point " << x << "," << y << " ";
s << " "; return vgui_soview2D::print(s) << "]";
}


bgui_vsol_soview2D_line_seg::
bgui_vsol_soview2D_line_seg(vsol_line_2d_sptr const& seg)
{
  vsol_point_2d_sptr p0 = seg->p0();
  vsol_point_2d_sptr p1 = seg->p1();
  x0=p0->x(); y0 = p0->y();
  x1=p1->x(); y1 = p1->y();
}

//--------------------------------------------------------------------------
//: vsol_polygon_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vsol_soview2D_polygon::print(vcl_ostream& poly) const
{
  return vgui_soview2D_polygon::print(poly);
}

bgui_vsol_soview2D_polygon::bgui_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& poly)
{
  if (!poly)
    {
      vcl_cout << "In bgui_vsol_soview2D_polygon(..) - null input poly\n";
      return;
    }
  //n, x, and y are in the parent class vgui_soview2D_polygon
  n = poly->size();
  x = new float[n], y = new float[n];
  for (unsigned int i=0; i<n;i++)
    {
      vsol_point_2d_sptr p = poly->vertex(i);
      x[i]=p->x();
      y[i]=p->y();
    }
}

//--------------------------------------------------------------------------
//: vsol_line_2d group view
//--------------------------------------------------------------------------

vcl_ostream& bgui_vsol_soview2D_line_group::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}

bgui_vsol_soview2D_line_group::
bgui_vsol_soview2D_line_group(vcl_vector<vsol_line_2d_sptr>& lines)
{
  for (vcl_vector<vsol_line_2d_sptr>::iterator lit = lines.begin();
       lit != lines.end(); lit++)
    {
      vgui_soview2D* sov = new bgui_vsol_soview2D_line_seg(*lit);
      ls.push_back(sov);
    }
}

//--------------------------------------------------------------------------
//: vsol_polygon_2d group view
//--------------------------------------------------------------------------

vcl_ostream& bgui_vsol_soview2D_polygon_group::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}

bgui_vsol_soview2D_polygon_group::
bgui_vsol_soview2D_polygon_group(vcl_vector<vsol_polygon_2d_sptr>& polys)
{
  for (vcl_vector<vsol_polygon_2d_sptr>::iterator pit = polys.begin();
       pit != polys.end(); pit++)
    {
      vgui_soview2D* sov = new bgui_vsol_soview2D_polygon(*pit);
      ls.push_back(sov);
    }
}
