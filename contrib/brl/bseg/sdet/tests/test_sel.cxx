// This is brl/bseg/sdet/tests/test_sel.cxx

#include <testlib/testlib_test.h>

#include <sdet/sdet_curve_model.h>
#include <sdet/sdet_sel.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>

//: Test the symbolic edge linker methods
MAIN( test_sel )
{ 
  double tolerance = 1e-3;
  bool test_passed = true;

  //*******************************************************
  START (" Test dbset_edgel class");
  //sdet_edgel constructors
  sdet_edgel* eA = new sdet_edgel(vgl_point_2d<double>(0.0,0.0), 0.0);
  sdet_edgel* eB = new sdet_edgel(vgl_point_2d<double>(1.0,0.0), 0.0);
  sdet_edgel* eC = new sdet_edgel(vgl_point_2d<double>(2.0,0.0), 0.0);
 
  TEST("Constructor", &eA!=0, true);

  //*******************************************************
  START (" Test sdet_ES_curve_model class");

  //sdet_ES_curve_model constructors
  sdet_ES_curve_model es1(eA, eB, eA, 0.1, 0.1, 1.0, 0.5, 0.5, false);
  sdet_ES_curve_model es2(eA, eB, eB, 0.1, 0.1, 1.0, 0.5, 0.5, false);
  sdet_ES_curve_model es3(eA, eC, eA, 0.1, 0.1, 1.0, 0.5, 0.5, false);

  //ground truth curve bundle eA-eB at eA
  double k_range_gt[] = {-0.0008, -0.7983, 0.0008, 0.7983 };
  double gamma_range_gt[] = {-0.5942, 1.7910, 0.5942, -1.7910};

  for (unsigned i=0; i<es1.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es1.cv_bundle[0][i].x() - k_range_gt[i])<tolerance 
                              && vcl_fabs(es1.cv_bundle[0][i].y() - gamma_range_gt[i])<tolerance;
  }
  TEST("Compute curve bundle from(eA-eB) at eA", test_passed, true);

  double k_range_gt2[] = {-0.0008, 0.7984, 0.0008, -0.7984 };
  double gamma_range_gt2[] = {0.5941, 1.7910, -0.5941, -1.7910};

  for (unsigned i=0; i<es2.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es2.cv_bundle[0][i].x() - k_range_gt2[i])<tolerance 
                              && vcl_fabs(es2.cv_bundle[0][i].y() - gamma_range_gt2[i])<tolerance;
  }
  TEST("Compute curve bundle from(eA-eB) at eB", test_passed, true);

  //ground truth eA-eC
  double k_range_gt3[] = { 0.0498, -0.2496, -0.0498, 0.2496 };
  double gamma_range_gt3[] = {-0.1492, 0.2990, 0.1492, -0.2990};

  for (unsigned i=0; i<es3.cv_bundle[0].size(); i++){
    test_passed = test_passed && vcl_fabs(es3.cv_bundle[0][i].x() - k_range_gt3[i])<tolerance 
                              && vcl_fabs(es3.cv_bundle[0][i].y() - gamma_range_gt3[i])<tolerance;
  }
  TEST("Compute curve bundle from a pair of edgels(eA-eC)", test_passed, true);

  
  START (" Test sdet_sel class");

  //construct an edgemap first
  sdet_edgemap_sptr edgemap = new sdet_edgemap(100, 100);
  edgemap->insert(eA);
  edgemap->insert(eB);
  edgemap->insert(eC);

  //construct other required classes
  sdet_curvelet_map cvlet_map; 
  sdet_edgel_link_graph edge_link_graph; 
  sdet_curve_fragment_graph curve_frag_graph;

  //sdet_sel constructors
  sdet_sel<sdet_ES_curve_model> edge_linker(edgemap, cvlet_map, edge_link_graph, curve_frag_graph);

  TEST("Constructor", &edge_linker != 0, true);

  //*******************************************************
 

  SUMMARY();
}
