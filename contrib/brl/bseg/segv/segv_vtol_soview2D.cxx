#include "segv_vtol_soview2D.h"
//:
// \file
#include <vcl_iostream.h>
#include <vgui/vgui_gl.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vtol/vtol_face_2d.h>

//--------------------------------------------------------------------------
//: vtol_vertex_2d view
//--------------------------------------------------------------------------
vcl_ostream& segv_vtol_soview2D_vertex::print(vcl_ostream& s) const
{
  s << "[segv_vtol_soview2D_vertex " << x << ',' << y << ' ';
  return vgui_soview2D::print(s) << ']';
}

//--------------------------------------------------------------------------
//: vtol_edge_2d view
//--------------------------------------------------------------------------
vcl_ostream& segv_vtol_soview2D_edge::print(vcl_ostream& s) const
{
  return vgui_soview2D_linestrip::print(s);
}

segv_vtol_soview2D_edge::segv_vtol_soview2D_edge(vtol_edge_2d_sptr& e)
{
  if (!e)
  {
    vcl_cout << "In segv_vtol_soview2D_edge(..) - null input edge\n";
    return;
  }

  //find out what kind of curve the edge has
  vsol_curve_2d_sptr c = e->curve();
  if (!c)
  {
    vcl_cout << "In segv_vtol_soview2D_edge(..) - null curve\n";
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
  vcl_cout << "In segv_vtol_soview2D_edge(vtol_edge_2d_sptr& e) -"
           << " attempt to draw an edge with unknown curve geometry\n";
}

//--------------------------------------------------------------------------
//: vtol_edge_2d group view
//--------------------------------------------------------------------------

vcl_ostream& segv_vtol_soview2D_edge_group::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}

segv_vtol_soview2D_edge_group::
segv_vtol_soview2D_edge_group(vcl_vector<vtol_edge_2d_sptr>& edges)
{
  for (vcl_vector<vtol_edge_2d_sptr>::iterator eit = edges.begin();
       eit != edges.end(); eit++)
  {
    vgui_soview2D* sov = new segv_vtol_soview2D_edge(*eit);
    ls.push_back(sov);
  }
}
//--------------------------------------------------------------------------
//: vtol_face_2d view
//--------------------------------------------------------------------------

vcl_ostream& segv_vtol_soview2D_face::print(vcl_ostream& s) const
{
  return vgui_soview2D_group::print(s);
}

segv_vtol_soview2D_face::segv_vtol_soview2D_face(vtol_face_2d_sptr& f)
{
  if (!f)
  {
    vcl_cout << "In segv_vtol_soview2D_face(..) - null input face\n";
    return;
  }
  edge_list* edges = f->edges();

  for (edge_list::iterator eit = edges->begin(); eit != edges->end(); eit++)
  {
    vtol_edge_2d_sptr e = (*eit)->cast_to_edge_2d();
    vgui_soview2D* sov = new segv_vtol_soview2D_edge(e);
    ls.push_back(sov);
  }
  delete edges;
}
