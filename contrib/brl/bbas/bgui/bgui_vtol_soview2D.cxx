#include "bgui_vtol_soview2D.h"
//:
// \file
#include <vcl_iostream.h>
#include <vgui/vgui_gl.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vtol/vtol_face_2d.h>

//--------------------------------------------------------------------------
//: vsol_point_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vtol_soview2D_point::print(vcl_ostream& s) const
{
  s << "[bgui_vtol_soview2D_point " << x << ',' << y << ' ';
  return vgui_soview2D::print(s) << ']';
}


//--------------------------------------------------------------------------
//: vdgl_digital_curve view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vtol_soview2D_dotted_digital_curve::print(vcl_ostream& s) const
{
  return s;
}

bgui_vtol_soview2D_dotted_digital_curve::
bgui_vtol_soview2D_dotted_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  if (!dc)
  {
    vcl_cout << "In bgui_vtol_soview2D_dotted_digital_curve(..) - null input dc\n";
    return;
  }

  //get the edgel chain
  vdgl_interpolator_sptr itrp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  //n, x, and y are in the parent class vgui_soview2D_linestrip
  unsigned int n = ech->size();

  float x, y;
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    x=ed.get_x();
    y=ed.get_y();
    vgui_soview2D* p = new vgui_soview2D_point(x, y);
    ls.push_back(p);
  }
  return;
}

//: vdgl_digital_curve view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vtol_soview2D_digital_curve::print(vcl_ostream& s) const
{
  return s;
}

bgui_vtol_soview2D_digital_curve::
bgui_vtol_soview2D_digital_curve(vdgl_digital_curve_sptr const& dc)
{
  if (!dc)
  {
    vcl_cout << "In bgui_vtol_soview2D_digital_curve(..) - null input dc\n";
    return;
  }

  //get the edgel chain
  vdgl_interpolator_sptr itrp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

  //n, x, and y are in the parent class vgui_soview2D_linestrip
  n = ech->size();

  x = new float[n], y = new float[n];
  for (unsigned int i=0; i<n;i++)
  {
    vdgl_edgel ed = (*ech)[i];
    x[i]=ed.get_x();
    y[i]=ed.get_y();
  }

  return;
}


bgui_vtol_soview2D_line_seg::
bgui_vtol_soview2D_line_seg(vsol_line_2d_sptr const& seg)
{
  vsol_point_2d_sptr p0 = seg->p0();
  vsol_point_2d_sptr p1 = seg->p1();
  x0=p0->x(); y0 = p0->y();
  x1=p1->x(); y1 = p1->y();
}

//--------------------------------------------------------------------------
//: vtol_vertex_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vtol_soview2D_vertex::print(vcl_ostream& s) const
{
  s << "[bgui_vtol_soview2D_vertex " << x << ',' << y << ' ';
  return vgui_soview2D::print(s) << ']';
}


//--------------------------------------------------------------------------
//: vtol_edge_2d view
//--------------------------------------------------------------------------
vcl_ostream& bgui_vtol_soview2D_edge::print(vcl_ostream& s) const
{
  return vgui_soview2D_linestrip::print(s);
}

bgui_vtol_soview2D_edge::bgui_vtol_soview2D_edge(vtol_edge_2d_sptr const& e)
{
  if (!e)
  {
    vcl_cout << "In bgui_vtol_soview2D_edge(..) - null input edge\n";
    return;
  }

  //find out what kind of curve the edge has
  vsol_curve_2d_sptr c = e->curve();
  if (!c)
  {
    vcl_cout << "In bgui_vtol_soview2D_edge(..) - null curve\n";
    return;
  }
  if (c->cast_to_vdgl_digital_curve())
  {
    vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
    //get the edgel chain
    vdgl_interpolator_sptr itrp = dc->get_interpolator();
    vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

    //n, x, and y are in the parent class vgui_soview2D_linestrip
    n = ech->size();
    //offset the coordinates for display (may not be needed)
    x = new float[n], y = new float[n];
    for (unsigned int i=0; i<n;i++)
    {
      vdgl_edgel ed = (*ech)[i];
      x[i]=ed.get_x();
      y[i]=ed.get_y();
    }
    return;
  }
  if (c->cast_to_line())
  {
    n = 2;
    x = new float[n], y = new float[n];
    vsol_line_2d_sptr l = c->cast_to_line();
    vsol_point_2d_sptr p0 = l->p0();
    x[0]=p0->x();      y[0]=p0->y();
    vsol_point_2d_sptr p1 = l->p1();
    x[1]=p1->x();      y[1]=p1->y();
    return;
  }
  vcl_cout << "In bgui_vtol_soview2D_edge(vtol_edge_2d_sptr& e) -"
           << " attempt to draw an edge with unknown curve geometry\n";
}

//--------------------------------------------------------------------------
//: vtol_edge_2d group view
//--------------------------------------------------------------------------

vcl_ostream& bgui_vtol_soview2D_edge_group::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}


bgui_vtol_soview2D_edge_group::
bgui_vtol_soview2D_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vgui_soview2D* sov = new bgui_vtol_soview2D_edge(*eit);
    ls.push_back(sov);
  }
}
//--------------------------------------------------------------------------
//: vtol_face_2d view
//--------------------------------------------------------------------------

vcl_ostream& bgui_vtol_soview2D_face::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}


bgui_vtol_soview2D_face::bgui_vtol_soview2D_face(vtol_face_2d_sptr const& f)
{
  if (!f)
  {
    vcl_cout << "In bgui_vtol_soview2D_face(..) - null input face\n";
    return;
  }

  edge_list* edges = f->edges();

  for (edge_list::iterator eit = edges->begin(); eit != edges->end(); eit++)
  {
    vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();
    vgui_soview2D* sov = new bgui_vtol_soview2D_edge(e);
    ls.push_back(sov);
  }
  delete edges;
}
