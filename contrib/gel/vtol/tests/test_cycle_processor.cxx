// This is gel/vtol/tests/test_cycle_processor.cxx
#include <testlib/testlib_test.h>
#include <vcl_algorithm.h> // vcl_find()
#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_vertex_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_one_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vtol/vtol_cycle_processor.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_curve_2d_sptr.h>

static void test_cycle_processor()
{
  bool basic_ops = true, cycle_ops = true, set_ops = true, merge_ops = true;

  vcl_cout<< "Testing basic ops\n";
  if (basic_ops)
  {
    vtol_vertex_2d_sptr v0 = new vtol_vertex_2d(0.0, 0.0);
    vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(5.0, 0.0);
    vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(2.5, 2.5);
    vtol_edge_sptr e0 = new vtol_edge_2d(v0, v1);
    vtol_edge_sptr e1 = new vtol_edge_2d(v1, v2);
    vtol_edge_sptr e2 = new vtol_edge_2d(v2, v0);
    vcl_cout << "edges:" << e0 << ' ' << e1 << ' ' << e2 << vcl_endl;
    edge_list edges;
    edges.push_back(e0);     edges.push_back(e1);
    bool found = vcl_find(edges.begin(), edges.end(), e0) != edges.end();
    vcl_cout << "Found e0 " << found << vcl_endl;
    found = vcl_find(edges.begin(), edges.end(), e2) != edges.end();
    vcl_cout << "Found e2 " << found << vcl_endl;

    edges.push_back(e2);
    edge_list::iterator eit = vcl_find(edges.begin(), edges.end(), e1);
    vcl_cout << "Edge found looking for e1 = " << *eit << vcl_endl;
    bool eequal = (*eit)==e1;
    vcl_cout << "Found edge == e1 " << eequal << vcl_endl;

    vcl_cout << "edges before erase\n";
    for (edge_list::iterator jit = edges.begin(); jit != edges.end(); jit++)
      vcl_cout << *jit  << ' ';
    vcl_cout << vcl_endl;

    edges.erase(eit);
    vcl_cout << "the erased edge " << *eit << **eit << vcl_endl
             << "edges after erase:\n";
    for (edge_list::iterator jit = edges.begin(); jit != edges.end(); jit++)
      vcl_cout << ' ' << *jit;
    vcl_cout << vcl_endl;
    TEST("edges content", edges.size()==2 && edges[0]==e0 && edges[1]==e2, true);
    //Check angle between edges
    vsol_point_2d_sptr pa = new vsol_point_2d(10.0, 5.0);
    vsol_point_2d_sptr pb = new vsol_point_2d(9.33, 2.5);
    vsol_point_2d_sptr pc = new vsol_point_2d(9.33, -2.5);
    vsol_point_2d_sptr pd = new vsol_point_2d(10.0, -5.0);
    vsol_point_2d_sptr pe = new vsol_point_2d(0.0, 0.0);
    vsol_point_2d_sptr pf = new vsol_point_2d(5.0, 0.0);
    vsol_curve_2d_sptr cef = new vdgl_digital_curve(pe, pf);
    vsol_curve_2d_sptr cfa = new vdgl_digital_curve(pf, pa);
    vsol_curve_2d_sptr cfb = new vdgl_digital_curve(pf, pb);
    vsol_curve_2d_sptr cfc = new vdgl_digital_curve(pf, pc);
    vsol_curve_2d_sptr cfd = new vdgl_digital_curve(pf, pd);
    vdgl_interpolator_sptr intp = cfd->cast_to_vdgl_digital_curve()->get_interpolator();
    vdgl_edgel_chain_sptr ec =intp->get_edgel_chain();
    for (unsigned int i=0; i<ec->size(); i++)
      vcl_cout << ec->edgel(i) << vcl_endl;
    vtol_vertex_sptr va = new vtol_vertex_2d(*pa);
    vtol_vertex_sptr vb = new vtol_vertex_2d(*pb);
    vtol_vertex_sptr vc = new vtol_vertex_2d(*pc);
    vtol_vertex_sptr vd = new vtol_vertex_2d(*pd);
    vtol_vertex_sptr ve = new vtol_vertex_2d(*pe);
    vtol_vertex_sptr vf = new vtol_vertex_2d(*pf);
    vtol_edge_2d_sptr eef = new vtol_edge_2d(ve,vf, cef);
    vtol_edge_2d_sptr efa = new vtol_edge_2d(vf,va, cfa);
    vtol_edge_2d_sptr efb = new vtol_edge_2d(vf,vb, cfb);
    vtol_edge_2d_sptr efc = new vtol_edge_2d(vf,vc, cfc);
    vtol_edge_2d_sptr efd = new vtol_edge_2d(vf,vd, cfd);

    double ang_ef_fa = vtol_cycle_processor::angle_between_edges(eef,efa,vf);
    double flip = ang_ef_fa < 0 ? 0 : 180;
    vcl_cout << "Angle ef-fa = " << ang_ef_fa <<vcl_endl;
    TEST_NEAR("angle_between_edges()", ang_ef_fa, flip-45, 1e-6);

    double ang_ef_fb = vtol_cycle_processor::angle_between_edges(eef,efb,vf);
    vcl_cout << "Angle ef-fb = "  << ang_ef_fb << vcl_endl;
    TEST_NEAR("angle_between_edges()", ang_ef_fb, flip-30, 0.001);

    double ang_ef_fc = vtol_cycle_processor::angle_between_edges(eef,efc,vf);
    vcl_cout << "Angle ef-fc = " << ang_ef_fc << vcl_endl;
    TEST_NEAR("angle_between_edges()", ang_ef_fc, 30-flip, 0.001);

    double ang_ef_fd = vtol_cycle_processor::angle_between_edges(eef,efd,vf);
    vcl_cout << "Angle ef-fd = " << ang_ef_fd << vcl_endl;
    TEST_NEAR("angle_between_edges()", ang_ef_fd, 45-flip, 1e-6);
  }

  //
  // test cycle construction operations
  //

  vcl_cout<< "Testing cycle_processor\n";
  if (cycle_ops)
  {
    vsol_point_2d_sptr p0 = new vsol_point_2d(0.0, 0.0);
    vsol_point_2d_sptr p1 = new vsol_point_2d(10.0, 0.0);
    vsol_point_2d_sptr p2 = new vsol_point_2d(5.0, 5.0);
    vsol_curve_2d_sptr c0 = new vdgl_digital_curve(p0, p1);
    vsol_curve_2d_sptr c1 = new vdgl_digital_curve(p1, p2);
    vsol_curve_2d_sptr c2 = new vdgl_digital_curve(p2, p0);
    vtol_vertex_2d_sptr v0 = new vtol_vertex_2d(*p0);
    vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(*p1);
    vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(*p2);
    vtol_edge_2d_sptr e0 = new vtol_edge_2d(v0, v1, c0);
    vtol_edge_2d_sptr e1 = new vtol_edge_2d(v1, v2, c1);
    vtol_edge_2d_sptr e2 = new vtol_edge_2d(v2, v0, c2);
    vcl_cout << "outside edges:" << e0 << ' ' << e1 << ' ' << e2 << vcl_endl;
    vcl_vector<vtol_edge_2d_sptr> edges;
    edges.push_back(e0);     edges.push_back(e1);     edges.push_back(e2);
    vtol_cycle_processor cp(edges, true);
    one_chain_list nested_chains;
    cp.nested_one_cycles(nested_chains, 0.5);
    if (nested_chains.size())
      for (one_chain_list::iterator cit = nested_chains.begin();
           cit != nested_chains.end(); cit++)
      {
        edge_list cedges; (*cit)->edges(cedges);
        vcl_cout << "chain edges\n";
        for (edge_list::iterator ceit = cedges.begin();
             ceit != cedges.end(); ceit++)
          vcl_cout << (*ceit)->cast_to_edge_2d() << vcl_endl;
        vcl_cout << vcl_endl;
      }
    else
      vcl_cout << "No cycles were formed\n";
    TEST("nested_chains", nested_chains.size(), 1);
    //Add an interior hole
    vsol_point_2d_sptr pi0 = new vsol_point_2d(1.0, 1.0);
    vsol_point_2d_sptr pi1 = new vsol_point_2d(7.0, 1.0);
    vsol_point_2d_sptr pi2 = new vsol_point_2d(4.0, 4.0);
    vsol_curve_2d_sptr ci0 = new vdgl_digital_curve(pi0, pi1);
    vsol_curve_2d_sptr ci1 = new vdgl_digital_curve(pi1, pi2);
    vsol_curve_2d_sptr ci2 = new vdgl_digital_curve(pi2, pi0);
    vtol_vertex_2d_sptr vi0 = new vtol_vertex_2d(*pi0);
    vtol_vertex_2d_sptr vi1 = new vtol_vertex_2d(*pi1);
    vtol_vertex_2d_sptr vi2 = new vtol_vertex_2d(*pi2);
    vtol_edge_2d_sptr ei0 = new vtol_edge_2d(vi0, vi1, ci0);
    vtol_edge_2d_sptr ei1 = new vtol_edge_2d(vi1, vi2, ci1);
    vtol_edge_2d_sptr ei2 = new vtol_edge_2d(vi2, vi0, ci2);
    vcl_cout << "hole edges:" << ei0 << ' ' << ei1 << ' ' << ei2 << vcl_endl;
    vcl_vector<vtol_edge_2d_sptr> edges2;
    edges2.push_back(e0);     edges2.push_back(e1);     edges2.push_back(e2);
    edges2.push_back(ei0);    edges2.push_back(ei1);    edges2.push_back(ei2);
    nested_chains.clear();
    vtol_cycle_processor cp2(edges2, true);
    cp2.nested_one_cycles(nested_chains, 0.5);
    if (nested_chains.size())
      for (one_chain_list::iterator cit = nested_chains.begin();
           cit != nested_chains.end(); cit++)
      {
        edge_list cedges; (*cit)->edges(cedges);
        vcl_cout << "chain edges\n";
        for (edge_list::iterator ceit = cedges.begin();
             ceit != cedges.end(); ceit++)
          vcl_cout << (*ceit)->cast_to_edge_2d() << vcl_endl;
        vcl_cout << vcl_endl;
      }
        TEST("nested_chains", nested_chains.size(), 2);
  }

  //
  // test cycle processor set operations
  //
  if (set_ops)
  {
    vcl_cout << "Starting set operation tests\n";
    vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
    vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(10.0,0.0);
    vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(5.0,10.0);
    vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(5.0,-10.0);
    vtol_edge_sptr e1= new vtol_edge_2d(v1,v2);
    vtol_edge_sptr e2= new vtol_edge_2d(v2,v3);
    vtol_edge_sptr e3= new vtol_edge_2d(v3,v1);
    vtol_edge_sptr e4= new vtol_edge_2d(v1,v4);
    vtol_edge_sptr e5= new vtol_edge_2d(v4,v2);
    edge_list s1, s2;
    s1.push_back(e1);   s1.push_back(e2);   s1.push_back(e3);
    s2.push_back(e4);   s2.push_back(e5);   s2.push_back(e1);
    vcl_cout<< "e1 =" << e1 << vcl_endl
            << "e2 =" << e2 << vcl_endl
            << "e3 =" << e3 << vcl_endl
            << "e4 =" << e4 << vcl_endl
            << "e5 =" << e5 << vcl_endl;
    //Test set intersection
    edge_list s1_and_s2;
    vtol_cycle_processor::intersect_edges(s1, s2, s1_and_s2);
    vcl_cout << "\nvtol_edge_2ds in s1\n";
    for (edge_list::iterator eit = s1.begin(); eit != s1.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    vcl_cout << "\nvtol_edge_2ds in s2\n";
    for (edge_list::iterator eit = s2.begin(); eit != s2.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    vcl_cout << "\nvtol_edge_2ds in s1_and_s2\n";
    for (edge_list::iterator eit = s1_and_s2.begin(); eit != s1_and_s2.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    TEST("vtol_cycle_processor::intersect_edges()", s1_and_s2[0], e1);
    //Test set difference
    edge_list s1_diff_s2;
    vtol_cycle_processor::difference_edges(s1, s2, s1_diff_s2);
    vcl_cout<< "\nvtol_edge_2ds in s1\n";
    for (edge_list::iterator eit = s1.begin(); eit != s1.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    vcl_cout<< "\nvtol_edge_2ds in s2\n";
    for (edge_list::iterator eit = s2.begin(); eit != s2.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    vcl_cout<< "\nvtol_edge_2ds in s1_minus_s2\n";
    for (edge_list::iterator eit = s1_diff_s2.begin(); eit != s1_diff_s2.end(); eit++)
      vcl_cout << ' ' << *eit << vcl_endl;
    TEST("vtol_cycle_processor::difference_edges() [0]", s1_diff_s2[0], e2);
    TEST("vtol_cycle_processor::difference_edges() [1]", s1_diff_s2[1], e3);
    vcl_cout << "Ending set operation tests\n\n";
  }
  //
  // test cycle merge operations
  //

  if (merge_ops)
  {
    vcl_cout << "Beginning merge operation tests\n";
    vtol_vertex_2d_sptr v1 = new vtol_vertex_2d(0.0,0.0);
    vtol_vertex_2d_sptr v2 = new vtol_vertex_2d(10.0,0.0);
    vtol_vertex_2d_sptr v3 = new vtol_vertex_2d(5.0,10.0);
    vtol_vertex_2d_sptr v4 = new vtol_vertex_2d(5.0,-10.0);
    vtol_edge_sptr e1= new vtol_edge_2d(v1,v2);
    vtol_edge_sptr e2= new vtol_edge_2d(v2,v3);
    vtol_edge_sptr e3= new vtol_edge_2d(v3,v1);
    vtol_edge_sptr e4= new vtol_edge_2d(v1,v4);
    vtol_edge_sptr e5= new vtol_edge_2d(v4,v2);
    vcl_cout<< "e1 =" << e1 << vcl_endl
            << "e2 =" << e2 << vcl_endl
            << "e3 =" << e3 << vcl_endl
            << "e4 =" << e4 << vcl_endl
            << "e5 =" << e5 << vcl_endl;
    edge_list f1_edges, f2_edges;
    f1_edges.push_back(e1);   f1_edges.push_back(e2);   f1_edges.push_back(e3);
    f2_edges.push_back(e4);   f2_edges.push_back(e5);   f2_edges.push_back(e1);
    vtol_one_chain_sptr onc_1 = new vtol_one_chain(f1_edges, true);
    vtol_one_chain_sptr onc_2 = new vtol_one_chain(f2_edges, true);
    one_chain_list merged_cycles;
#if 0 // not yet implemented; change this to "1" when merge_one_cycles() exists
    vtol_cycle_processor::merge_one_cycles(onc_1, onc_2, merged_cycles);
#else
    edge_list merged_edges;
    merged_edges.push_back(onc_1->edge(1));
    merged_edges.push_back(onc_1->edge(2));
    merged_edges.push_back(onc_2->edge(0));
    merged_edges.push_back(onc_2->edge(1));
    vtol_one_chain_sptr merged_cycle = new vtol_one_chain(merged_edges, true);
    merged_cycles.push_back(merged_cycle);
#endif
    TEST("vtol_cycle_processor::merge_one_cycles()", merged_cycles.size(), 1);
    vcl_cout<< "number of one_cycles = " << merged_cycles.size() << vcl_endl;
    if (merged_cycles.size() > 0)
    {
      edge_list outer_edges; merged_cycles[0]->edges(outer_edges);
      vcl_cout<< "edges in merged cycle\n";
      for (edge_list::iterator eit = outer_edges.begin();
           eit != outer_edges.end(); eit++)
        vcl_cout<< *eit << vcl_endl;
      TEST("vtol_cycle_processor::merge_one_cycles()",
           outer_edges[0]==e2 && outer_edges[1]==e3 &&
           outer_edges[2]==e4 && outer_edges[3]==e5, true);
    }
    vcl_cout << "Ending merge operation tests\n\n";
  }
}

TESTMAIN(test_cycle_processor);
