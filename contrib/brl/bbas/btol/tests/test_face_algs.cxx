// This is brl/bbas/btol/tests/test_face_algs.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_polygon.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_face_2d_sptr.h>
#include <btol/btol_face_algs.h>

static void print_edges(vcl_string const& msg, vtol_face_2d_sptr const& f)
{
  if (!f)
    return;
  edge_list edges; f->edges(edges);
  for (edge_list::iterator eit = edges.begin(); eit != edges.end(); eit++)
    vcl_cout << msg << '('
             << (*eit)->v1()->cast_to_vertex_2d()->x() << ' '
             << (*eit)->v1()->cast_to_vertex_2d()->y() << "):("
             << (*eit)->v2()->cast_to_vertex_2d()->x() << ' '
             << (*eit)->v2()->cast_to_vertex_2d()->y()
             << ")\n";
}

static void print_verts(vcl_string const& msg, vtol_face_2d_sptr const& f)
{
  if (!f)
    return;
  vertex_list verts; f->vertices(verts);
  for (vertex_list::iterator vit = verts.begin(); vit != verts.end(); vit++)
    vcl_cout << msg << (*vit)->cast_to_vertex_2d()->x() << ' '
             << (*vit)->cast_to_vertex_2d()->y() << ")\n";
}

static void test_face_algs()
{
  vcl_cout << "testing one_chain constructor\n";

  //Outside boundary
  vtol_vertex_sptr v1 = new vtol_vertex_2d(0.0,0.0);
  vtol_vertex_sptr v2 = new vtol_vertex_2d(10.0,0.0);
  vtol_vertex_sptr v3 = new vtol_vertex_2d(10.0,20.0);
  vtol_vertex_sptr v4 = new vtol_vertex_2d(0.0,20.0);

  //Interior Hole
  vtol_vertex_sptr vh1 = new vtol_vertex_2d(5.0,5.0);
  vtol_vertex_sptr vh2 = new vtol_vertex_2d(7.0,5.0);
  vtol_vertex_sptr vh3 = new vtol_vertex_2d(7.0,10.0);
  vtol_vertex_sptr vh4 = new vtol_vertex_2d(5.0,10.0);

  vertex_list verts;
  verts.push_back(v1);
  verts.push_back(v2);
  verts.push_back(v3);
  verts.push_back(v4);

  vertex_list hole_verts;
  hole_verts.push_back(vh1);
  hole_verts.push_back(vh2);
  hole_verts.push_back(vh3);
  hole_verts.push_back(vh4);

  vtol_one_chain_sptr outside_chain = btol_face_algs::one_chain(verts);
  outside_chain->describe(vcl_cout);
  vertex_list chain_verts; outside_chain->vertices(chain_verts);
  bool all_equal = true;
  int n = chain_verts.size();
  all_equal = all_equal && n==4;
  if (all_equal)
    for (int i = 0; i<4; i++)
      all_equal = all_equal && (*chain_verts[i])==(*verts[i]);

  TEST("vtol_face_algs::chain_constructor", all_equal, true);

  vcl_cout << "testing simply-connected face transformation\n";
  vnl_matrix_fixed<double, 3, 3> T;
  T[0][0] = 1.0;   T[0][1] = 0.0;   T[0][2] = 0.0;
  T[1][0] = 0.0;   T[1][1] = 1.0;   T[1][2] = 0.0;
  T[2][0] = 0.0;   T[2][1] = 0.0;   T[2][2] = 1.0;

  one_chain_list chains;
  chains.push_back(outside_chain);
  vtol_face_2d_sptr  simple_f = new vtol_face_2d(chains);
  print_verts("simple_f_vert ", simple_f);

  vtol_face_2d_sptr trans_simple_f = btol_face_algs::transform(simple_f, T);
  print_verts("trans_simple_f_vert ", trans_simple_f);

  TEST("vtol_face_algs::transform_simple_face", *simple_f, *trans_simple_f);

  vcl_cout << "testing multiply-connected face transformation\n";
  vtol_one_chain_sptr hole_chain = btol_face_algs::one_chain(hole_verts);

  chains.push_back(hole_chain);
  vtol_face_2d_sptr  multi_f = new vtol_face_2d(chains);
  print_edges("multi_f_edge ", multi_f);
  vcl_cout << "\n\n";
  vertex_list* mf_verts = multi_f->outside_boundary_vertices();
  for (vertex_list::iterator vit = mf_verts->begin();
       vit != mf_verts->end(); vit++)
    vcl_cout << "mf_bound_v (" << (*vit)->cast_to_vertex_2d()->x() << ' '
             << (*vit)->cast_to_vertex_2d()->y() << ")\n";
  vcl_cout << "\n\n";
  delete mf_verts;
  one_chain_list *holes = multi_f->get_hole_cycles();
  vertex_list mf_hole_verts; (*holes)[0]->vertices(mf_hole_verts);
  for (vertex_list::iterator vit = mf_hole_verts.begin();
       vit != mf_hole_verts.end(); vit++)
    vcl_cout << "mf_hole_v (" << (*vit)->cast_to_vertex_2d()->x() << ' '
             << (*vit)->cast_to_vertex_2d()->y() << ")\n";
  delete holes;

  vtol_face_2d_sptr trans_multi_f = btol_face_algs::transform(multi_f, T);
  print_edges("trans_multi_f_edge ", trans_multi_f);

  TEST("vtol_face_algs::transform_multi_face", *multi_f, *trans_multi_f);

  vcl_cout << "testing construction from vgl_polygons\n";
  vgl_polygon<double> poly;
  poly.clear();
  poly.new_sheet();
  for (vertex_list::iterator vit = verts.begin(); vit != verts.end(); ++vit)
  {
    vtol_vertex_2d* v = (*vit)->cast_to_vertex_2d();
    if (v)
      poly.push_back(v->x(), v->y());
  }
  poly.new_sheet();
  for (vertex_list::iterator vit = hole_verts.begin();
       vit != hole_verts.end(); ++vit)
  {
    vtol_vertex_2d* v = (*vit)->cast_to_vertex_2d();
    if (v)
      poly.push_back(v->x(), v->y());
  }
  vtol_face_2d_sptr poly_face;
  bool failed = !btol_face_algs::vgl_to_vtol(poly, poly_face);
  if (!failed)
  {
    print_edges("poly_edge ", poly_face);
  }

  TEST("vtol_face_algs::vgl_to_vtol", failed, false);
}

TESTMAIN(test_face_algs);
