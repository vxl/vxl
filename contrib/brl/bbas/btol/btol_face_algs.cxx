//:
// \file
#include <vcl_algorithm.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>
#include <btol/btol_face_algs.h>


//: Destructor
btol_face_algs::~btol_face_algs()
{
}

//:convert a face to a vgl_polygon currently only works for singlely-connected
// faces.

bool btol_face_algs::vtol_to_vgl(vtol_face_2d_sptr const & face,
                                 vgl_polygon& poly)
{
  if(!face)
    return false;
  poly.clear();
  poly.new_sheet();
  vcl_vector<vtol_one_chain_sptr> one_chains;
  face->one_chains(one_chains);
  if(one_chains.size()!=1)
    return false;
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  if(!verts.size())
    return false;
  for(vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
      vit != verts.end(); vit++)
    {
      vtol_vertex_2d* v = (*vit)->cast_to_vertex_2d();
      if(v)
        poly.push_back(v->x(), v->y());
    }
  return true;
}
//: currently only works for a digital edge (not a line segment)
bool btol_face_algs::edge_intersects(vtol_face_2d_sptr const & face,
                                     vtol_edge_2d_sptr const & edge)
{
  if(!face||!edge)
    return false;
  //quick check
  vsol_box_2d_sptr face_bounds = face->get_bounding_box();
  vsol_box_2d_sptr edge_bounds = edge->get_bounding_box();
  if(!bsol_algs::meet(face_bounds, edge_bounds))
    return false;
  vsol_curve_2d_sptr c = edge->curve();
  vdgl_digital_curve_sptr dc = c->cast_to_digital_curve();
  if (!dc)
    {
      vcl_cout << "In btol_face_algs::edge_intersects(.) - only digital"
               << " curve geometry implemented\n";
      return false;
    }
  //convert the face to a polygon
  vgl_polygon poly;
  btol_face_algs::vtol_to_vgl(face, poly);
  //iterate through the digital curve  points
  vdgl_interpolator_sptr intp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
  int nedgl = ec->size();
  for (int i=0; i<nedgl; i++)
    if(poly.contains((*ec)[i].x(), (*ec)[i].y()))
      return true;
  return false;
}
//: takes an input set of edges and constructs the intersecting set
//  returns false if the intersecting set is empty.
bool btol_face_algs::
intersecting_edges(vtol_face_2d_sptr const & face,
                   vcl_vector<vtol_edge_2d_sptr> const & edges,
                   vcl_vector<vtol_edge_2d_sptr>& inter_edges)
{
  if(!face||!edges.size())
    return false;
  inter_edges.clear();
  bool empty = true;
  for(vcl_vector<vtol_edge_2d_sptr>::const_iterator eit = edges.begin();
      eit != edges.end(); eit++)
    if(btol_face_algs::edge_intersects(face, *eit))
      {
        inter_edges.push_back(*eit);
        empty = false;
      }
  return !empty;
}
//:only works if the edges are straight lines
vsol_point_2d_sptr btol_face_algs::centroid(vtol_face_2d_sptr const & face)
{
  if(!face)
    return (vsol_point_2d*)0;
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  int n = 0;
  double x0=0, y0=0;
  for(vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
      vit != verts.end(); vit++, n++)
    {
      vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
      x0 += v->x();
      y0 += v->y();
    }
  if(!n)
    return (vsol_point_2d*)0;
  x0 /=n;
  y0 /=n;
  return new vsol_point_2d(x0, y0);
}

vtol_face_2d_sptr btol_face_algs::box(const double x0, const double y0, 
                                      const double width, const double height)
{
  double w = width/2, h = height/2;
  vcl_vector<vtol_vertex_sptr> verts;
  vtol_vertex_2d* v0 = new vtol_vertex_2d(x0-w, y0-h);
  vtol_vertex_2d* v1 = new vtol_vertex_2d(x0+w, y0-h);
  vtol_vertex_2d* v2 = new vtol_vertex_2d(x0+w, y0+h);
  vtol_vertex_2d* v3 = new vtol_vertex_2d(x0-w, y0+h);
  verts.push_back(v0);  verts.push_back(v1);
  verts.push_back(v2);  verts.push_back(v3);
  vtol_face_2d_sptr f = new vtol_face_2d(verts);
  return f;
}
