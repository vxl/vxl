//:
// \file
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/internals/vgui_draw_line.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#if 0 // not needed for now
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_group_2d.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_triangle_2d.h>
#endif // 0

#include "bgui_linked_vsol_soview2D.h"
#include <vgl/vgl_distance.h>
#include <vgui/vgui_soview2D.h>

//--------------------------------------------------------------------------
//: vsol_point_2d view
//--------------------------------------------------------------------------
/*vcl_ostream& bgui_linked_vsol_soview2D_point::print(vcl_ostream& s) const
{
  s << "[bgui_linked_vsol_soview2D_point " << x << "," << y << " ";
  return bgui_soview2D::print(s) << "]";
}

//--------------------------------------------------------------------------
//: vsol_line_2d view
//--------------------------------------------------------------------------

bgui_linked_vsol_soview2D_line_seg::
bgui_linked_vsol_soview2D_line_seg(vsol_line_2d_sptr const& seg)
{
  vsol_point_2d_sptr p0 = seg->p0();
  vsol_point_2d_sptr p1 = seg->p1();
  x0=p0->x(); y0 = p0->y();
  x1=p1->x(); y1 = p1->y();
}

//--------------------------------------------------------------------------
//: vsol_polyline_2d view
//--------------------------------------------------------------------------

vcl_ostream& bgui_linked_vsol_soview2D_polyline::print(vcl_ostream& s) const
{
  return bgui_soview2D_linestrip::print(s);
}

bgui_linked_vsol_soview2D_polyline::bgui_linked_vsol_soview2D_polyline(vsol_polyline_2d_sptr const& pline)
{
  if (!pline)
    {
      vcl_cout << "In bgui_linked_vsol_soview2D_polyline(..) - null input edge\n";
      return;
    }

   //n, x, and y are in the parent class bgui_soview2D_linestrip
   n = pline->size();

   //offset the coordinates for display (may not be needed)
   x = new float[n], y = new float[n];
   for (unsigned int i=0; i<n;i++)
   {
      x[i] = pline->vertex(i)->x();
      y[i] = pline->vertex(i)->y();
   }
   return;
}

//--------------------------------------------------------------------------
//: vdgl_digital_curve dotted view
//--------------------------------------------------------------------------
vcl_ostream& bgui_linked_vsol_soview2D_dotted_digital_curve::print(vcl_ostream& s) const
{
  return s;
}

bgui_linked_vsol_soview2D_dotted_digital_curve::
bgui_linked_vsol_soview2D_dotted_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  if (!dc)
    {
      vcl_cout << "In bgui_linked_vsol_soview2D_dotted_digital_curve(..) - null input dc\n";
      return;
    }

  //get the edgel chain
  vdgl_interpolator_sptr itrp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  //n, x, and y are in the parent class bgui_soview2D_linestrip
  unsigned int n = ech->size();

  float x = 0, y=0;
  for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      x=ed.get_x();
      y=ed.get_y();
      bgui_soview2D* p = new bgui_soview2D_point(x, y);
      ls.push_back(p);
    }
  return;
}
*/
//--------------------------------------------------------------------------
//: vdgl_digital_curve regular view
//--------------------------------------------------------------------------

vcl_ostream& bgui_linked_vsol_soview2D_digital_curve::print(vcl_ostream& s) const
{
  return s;
}

void bgui_linked_vsol_soview2D_digital_curve::draw() const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = sptr->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  int n = ech->size();

  glBegin( GL_LINE_STRIP );
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    glVertex2f( ed.get_x() , ed.get_y() );
  }
  glEnd();
}

float bgui_linked_vsol_soview2D_digital_curve::distance_squared( float x , float y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = sptr->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  int n = ech->size();

  float* xptr = new float[n]; 
  float* yptr = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    xptr[i]=ed.get_x();
    yptr[i]=ed.get_y();
  }

  double tmp = vgl_distance_to_non_closed_polygon( xptr , yptr , n , x , y );

  delete [] xptr;
  delete [] yptr;

  return tmp * tmp;
}

void bgui_linked_vsol_soview2D_digital_curve::get_centroid( float* x, float* y ) const
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = sptr->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  int n = ech->size();

  *x = 0;
  *y = 0;

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    *x += ed.get_x();
    *y += ed.get_y();
  }
  float s = 1.0f / float( n );
  *x *= s;
  *y *= s;
}

void bgui_linked_vsol_soview2D_digital_curve::translate( float x , float y ) 
{
  //get the edgel chain
  vdgl_interpolator_sptr itrp = sptr->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  int n = ech->size();

  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    ed.set_x( ed.get_x() + x );
    ed.set_y( ed.get_y() + y );
  }
}

/*
//--------------------------------------------------------------------------
//: vsol_polygon_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_linked_vsol_soview2D_polygon::print(vcl_ostream& poly) const
{
  return bgui_soview2D_polygon::print(poly);
}

bgui_linked_vsol_soview2D_polygon::bgui_linked_vsol_soview2D_polygon(vsol_polygon_2d_sptr const& poly)
{
  if (!poly)
    {
      vcl_cout << "In bgui_linked_vsol_soview2D_polygon(..) - null input poly\n";
      return;
    }
  //n, x, and y are in the parent class bgui_soview2D_polygon
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

vcl_ostream& bgui_linked_vsol_soview2D_line_group::print(vcl_ostream& s) const
{
  return bgui_soview2D_group::print(s);
}

bgui_linked_vsol_soview2D_line_group::
bgui_linked_vsol_soview2D_line_group(vcl_vector<vsol_line_2d_sptr>& lines)
{
  for (vcl_vector<vsol_line_2d_sptr>::iterator lit = lines.begin();
       lit != lines.end(); lit++)
    {
      bgui_soview2D* sov = new bgui_linked_vsol_soview2D_line_seg(*lit);
      ls.push_back(sov);
    }
}

//--------------------------------------------------------------------------
//: vsol_polygon_2d group view
//--------------------------------------------------------------------------

vcl_ostream& bgui_linked_vsol_soview2D_polygon_group::print(vcl_ostream& s) const
{
  return bgui_soview2D_group::print(s);
}

bgui_linked_vsol_soview2D_polygon_group::
bgui_linked_vsol_soview2D_polygon_group(vcl_vector<vsol_polygon_2d_sptr>& polys)
{
  for (vcl_vector<vsol_polygon_2d_sptr>::iterator pit = polys.begin();
       pit != polys.end(); pit++)
    {
      bgui_soview2D* sov = new bgui_linked_vsol_soview2D_polygon(*pit);
      ls.push_back(sov);
    }
}*/
