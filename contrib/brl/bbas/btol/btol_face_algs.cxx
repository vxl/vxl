//:
// \file
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vtol/vtol_vertex_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_face_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bsol/bsol_algs.h>
#include <btol/btol_vertex_algs.h>
#include <btol/btol_face_algs.h>

//:
//If the edge direction is minus then the vertices are flipped using
//the ::vertices() method. Thus they are out of order for eventual
//re-linking.  This method avoids the flip problem.
static bool properly_ordered_verts(vtol_one_chain_sptr const& cycle,
                                   vcl_vector<vtol_vertex_sptr>& verts)
{
  if (!cycle||!cycle->is_cycle())
    return false;
  int n = cycle->num_edges();
  for (int i = 0; i<n; i++)
  {
    vtol_edge_sptr e = cycle->edge(i);
    verts.push_back(e->v1());
  }
  return true;
}

//: convert a face to a vgl_polygon
bool btol_face_algs::vtol_to_vgl(vtol_face_2d_sptr const & face,
                                 vgl_polygon<double>& poly)
{
  if (!face)
    return false;
#if 0
  poly.clear();
  poly.new_sheet();
  vcl_vector<vtol_one_chain_sptr> one_chains;
  face->one_chains(one_chains);
  if (one_chains.size()!=1)
    return false;
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  if (!verts.size())
    return false;
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++)
  {
    vtol_vertex_2d* v = (*vit)->cast_to_vertex_2d();
    if (v)
      poly.push_back(v->x(), v->y());
  }
#endif
  poly.clear();
  poly.new_sheet();
  vcl_vector<vtol_vertex_sptr>* outside_verts =
    face->outside_boundary_vertices();
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = outside_verts->begin();
       vit != outside_verts->end(); ++vit)
    {
      vtol_vertex_2d* v = (*vit)->cast_to_vertex_2d();
      if (v)
        poly.push_back(v->x(), v->y());
    }
  delete outside_verts;
  //add the holes, if any
  vcl_vector<vtol_one_chain_sptr>* hole_chains = face->get_hole_cycles();
  vcl_vector<vcl_vector<vtol_vertex_sptr> > all_hole_verts;
  for (vcl_vector<vtol_one_chain_sptr>::iterator cit = hole_chains->begin();
       cit != hole_chains->end(); cit++)
    {
      vcl_vector<vtol_vertex_sptr> hole_verts;
      if (!properly_ordered_verts(*cit, hole_verts))
       return false;
      poly.new_sheet();
      for (vcl_vector<vtol_vertex_sptr>::iterator vit = hole_verts.begin();
           vit != hole_verts.end(); ++vit)
        {
          vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
          if (v)
            poly.push_back(v->x(), v->y());
        }
    }
  delete hole_chains;
  return true;
}

//: convert a vgl_polygon to a vtol_face_2d.  Works for multiply-connected polys
bool btol_face_algs::vgl_to_vtol(vgl_polygon<double>const & poly,
                                 vtol_face_2d_sptr& face)
{
  //convert the polygon sheets to one_chains
  vcl_vector<vtol_one_chain_sptr> chains;
  int n_sheets = poly.num_sheets();
  for (int i = 0; i<n_sheets; i++)
  {
    vtol_one_chain_sptr chain = new vtol_one_chain();
    vcl_vector<vgl_point_2d<double> > s = poly[i];
    vgl_point_2d<double> p0 = s[0];
    vtol_vertex_2d_sptr vs = new vtol_vertex_2d(p0.x(), p0.y()), v0 = vs, vi;
    vtol_edge_2d_sptr e;
    for (vcl_vector<vgl_point_2d<double> >::iterator pit = s.begin()+1;
         pit != s.end(); ++pit)
      {
        vgl_point_2d<double> pi = *pit;
        vi = new vtol_vertex_2d(pi.x(), pi.y());
        e  = new vtol_edge_2d(v0, vi);
        chain->add_edge(e, i==0);//outer cycle is positive
        v0 = vi;
      }
    //last edge closes the cycle
    e = new vtol_edge_2d(vi, vs);
    chain->add_edge(e, i==0);//outer cycle is positive
    chain->set_cycle(true);
    chains.push_back(chain);
  }
  //so now we have a set of nested one chains
  //and can construct the face
  face  = new vtol_face_2d(chains);
  return true;
}

//: currently only works for a digital edge (not a line segment)
bool btol_face_algs::edge_intersects(vtol_face_2d_sptr const & face,
                                     vtol_edge_2d_sptr const & edge)
{
  if (!face||!edge)
    return false;
  //quick check
  vsol_box_2d_sptr face_bounds = face->get_bounding_box();
  vsol_box_2d_sptr edge_bounds = edge->get_bounding_box();
  if (!bsol_algs::meet(face_bounds, edge_bounds))
    return false;
  vsol_curve_2d_sptr c = edge->curve();
  vdgl_digital_curve_sptr dc = c->cast_to_vdgl_digital_curve();
  if (!dc)
  {
    vcl_cout << "In btol_face_algs::edge_intersects(.) -"
             << " only digital curve geometry implemented\n";
    return false;
  }
  //convert the face to a polygon
  vgl_polygon<double> poly;
  btol_face_algs::vtol_to_vgl(face, poly);
  //iterate through the digital curve points
  vdgl_interpolator_sptr intp = dc->get_interpolator();
  vdgl_edgel_chain_sptr ec = intp->get_edgel_chain();
  int nedgl = ec->size();
  for (int i=0; i<nedgl; i++)
    if (poly.contains((*ec)[i].x(), (*ec)[i].y()))
      return true;
  return false;
}

//: takes an input set of edges and constructs the intersecting set.
//  Returns false if the intersecting set is empty.
bool btol_face_algs::
intersecting_edges(vtol_face_2d_sptr const & face,
                   vcl_vector<vtol_edge_2d_sptr> const & edges,
                   vcl_vector<vtol_edge_2d_sptr>& inter_edges)
{
  if (!face||!edges.size())
    return false;
  inter_edges.clear();
  bool empty = true;
  for (vcl_vector<vtol_edge_2d_sptr>::const_iterator eit = edges.begin();
       eit != edges.end(); eit++)
    if (btol_face_algs::edge_intersects(face, *eit))
    {
      inter_edges.push_back(*eit);
      empty = false;
    }
  return !empty;
}

//: only works if the edges are straight lines
vsol_point_2d_sptr btol_face_algs::centroid(vtol_face_2d_sptr const & face)
{
  if (!face)
    return (vsol_point_2d*)0;
  vcl_vector<vtol_vertex_sptr> verts;
  face->vertices(verts);
  int n = 0;
  double x0=0, y0=0;
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = verts.begin();
       vit != verts.end(); vit++, n++)
  {
    vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
    x0 += v->x();
    y0 += v->y();
  }
  if (!n)
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
  verts.push_back(v0); verts.push_back(v1);
  verts.push_back(v2); verts.push_back(v3);
  vtol_face_2d_sptr f = new vtol_face_2d(verts);
  return f;
}

//: create a simply-connected one_chain from a set of vertices.
// dir=true corresponds to +.
vtol_one_chain_sptr btol_face_algs::
one_chain(vcl_vector<vtol_vertex_sptr> const& verts)
{
  vtol_one_chain_sptr out;
  int n = verts.size();
  if (n<3)
    return out;
  vtol_vertex_2d_sptr v0 = verts[0]->cast_to_vertex_2d(), vs = v0, vi;
  vtol_edge_2d_sptr e;
  out = new vtol_one_chain();
  for (int i = 1; i<n; i++)
  {
    vi = verts[i]->cast_to_vertex_2d();
    e = new vtol_edge_2d(v0, vi);
    out->add_edge(e, true);
    v0=vi;
  }
  //add final edge
  e = new vtol_edge_2d(vi, vs);
  out->add_edge(e, true);
  out->set_cycle(true);
  return out;
}

//: create a new face by transforming the input face.
// Only valid for faces with linear (straight line) geometry
vtol_face_2d_sptr btol_face_algs::
transform(vtol_face_2d_sptr const& face,
          vnl_matrix_fixed<double, 3, 3> const& T)
{
  vtol_face_2d_sptr out;
  if (!face)
    return out;
  // transform the vertices of the outside boundary
  vcl_vector<vtol_vertex_sptr>* outside_verts =
    face->outside_boundary_vertices();

  vcl_vector<vtol_vertex_sptr> trans_outside_verts;
  for (vcl_vector<vtol_vertex_sptr>::iterator vit = outside_verts->begin();
       vit != outside_verts->end(); ++vit)
    {
      vtol_vertex_2d_sptr new_v =
        btol_vertex_algs::transform((*vit)->cast_to_vertex_2d(), T);
      if (!new_v)
        return out;
      trans_outside_verts.push_back(new_v->cast_to_vertex());
    }
  delete outside_verts;
  // transform the vertices of the interior holes
  vcl_vector<vtol_one_chain_sptr>* hole_chains = face->get_hole_cycles();
  vcl_vector<vcl_vector<vtol_vertex_sptr> > trans_hole_verts;
  for (vcl_vector<vtol_one_chain_sptr>::iterator cit = hole_chains->begin();
       cit != hole_chains->end(); cit++)
  {
    vcl_vector<vtol_vertex_sptr> hole_verts, t_hole_verts;
    if (!properly_ordered_verts(*cit, hole_verts))
      return out;
    for (vcl_vector<vtol_vertex_sptr>::iterator vit = hole_verts.begin();
         vit != hole_verts.end(); ++vit)
    {
      vtol_vertex_2d_sptr v = (*vit)->cast_to_vertex_2d();
      vtol_vertex_2d_sptr new_v =
        btol_vertex_algs::transform(v, T);
      if (!new_v)
        return out;
      t_hole_verts.push_back(new_v->cast_to_vertex());
    }
    trans_hole_verts.push_back(t_hole_verts);
  }
  delete hole_chains;
  //now reassemble the face
  //form the chains
  vcl_vector<vtol_one_chain_sptr> new_chains;
  new_chains.push_back(btol_face_algs::one_chain(trans_outside_verts));
  for (vcl_vector<vcl_vector<vtol_vertex_sptr> >::iterator vts =
       trans_hole_verts.begin(); vts != trans_hole_verts.end(); ++vts)
    new_chains.push_back(btol_face_algs::one_chain(*vts));

  //construct the face
  return new vtol_face_2d(new_chains);
}
