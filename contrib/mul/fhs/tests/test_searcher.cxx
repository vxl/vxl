// This is mul/fhs/tests/test_searcher.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <fhs/fhs_searcher.h>


void test_searcher1()
{
  vcl_cout << "==== test fhs_searcher (1) =====\n"
           << " Simple pair test (one arc)"<<vcl_endl;

  // Generate arc list
  vcl_vector<fhs_arc> arcs(1);
  int dx = 3, dy = 4;
  arcs[0]=fhs_arc(0,1, dx,dy, 2, 2);

  vcl_vector<vimt_image_2d_of<float> > feature_response(2);
  for (unsigned i=0;i<2;++i)
  {
    feature_response[i].image().set_size(20,20);
    feature_response[i].image().fill(100.0f);
  }
  feature_response[0].image()(10,10)= 60.0f;
  feature_response[1].image()(10+dx,10+dy)= 60.0f;

  fhs_searcher searcher;
  searcher.set_tree(arcs,0);

  TEST("Root node index",searcher.root_node(),0);
  TEST("N. points",searcher.n_points(),2);

  searcher.search(feature_response);
  vcl_vector<vgl_point_2d<double> > pts;
  double min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST("Number of output points",pts.size(),2);
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Next point is (13,14)",(pts[1]-vgl_point_2d<double>(13,14)).length(),0,1e-6);


  vcl_cout<<"Tweak arc by (-1,0)"<<vcl_endl;
  arcs[0]=fhs_arc(0,1, dx-1,dy, 1, 1);
  pts.resize(0);
  searcher.set_tree(arcs,0);
  searcher.search(feature_response);
  min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Next point is (13,14)",(pts[1]-vgl_point_2d<double>(13,14)).length(),0,1e-6);

  vcl_cout<<"Tweak arc by (-5,0)"<<vcl_endl;
  arcs[0]=fhs_arc(0,1, dx-5,dy, 1, 1);
  pts.resize(0);
  searcher.set_tree(arcs,0);
  searcher.search(feature_response);
  min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Next point is (13,14)",(pts[1]-vgl_point_2d<double>(13,14)).length(),0,1e-6);
  vcl_cout<<"Solution is "<<pts[1]<<vcl_endl;

  vcl_cout<<"Tweak arc by (-7,0)"<<vcl_endl;
  arcs[0]=fhs_arc(0,1, dx-7,dy, 1, 1);
  pts.resize(0);
  searcher.set_tree(arcs,0);
  searcher.search(feature_response);
  min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Next point is (6,14)",(pts[1]-vgl_point_2d<double>(6,14)).length(),0,1e-6);
  vcl_cout<<"Solution is "<<pts[1]<<vcl_endl;
}

void test_searcher2()
{
  vcl_cout << "==== test fhs_searcher (2) =====\n"
           << " Root is at centre of a square"<<vcl_endl;

  // Generate arc list
  vcl_vector<fhs_arc> arcs(4);
  int dx = 3, dy = 4;
  arcs[0]=fhs_arc(0,1, dx,dy, 1, 1);
  arcs[1]=fhs_arc(0,2, -dx,dy, 1, 1);
  arcs[2]=fhs_arc(0,3, dx,-dy, 1, 1);
  arcs[3]=fhs_arc(0,4, -dx,-dy, 1, 1);

  vcl_vector<vimt_image_2d_of<float> > feature_response(5);
  for (unsigned i=0;i<5;++i)
  {
    feature_response[i].image().set_size(20,20);
    feature_response[i].image().fill(10.0f);
  }
  feature_response[1].image()(10+dx,10+dy)= 5.0f;
  feature_response[2].image()(10-dx,10+dy)= 5.0f;
  feature_response[3].image()(10+dx,10-dy)= 5.0f;
  feature_response[4].image()(10-dx,10-dy)= 5.0f;

  fhs_searcher searcher;
  searcher.set_tree(arcs,0);

  TEST("Root node index",searcher.root_node(),0);
  TEST("N. points",searcher.n_points(),5);

  searcher.search(feature_response);
  vcl_vector<vgl_point_2d<double> > pts;
  double min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST("Number of output points",pts.size(),5);
  TEST_NEAR("Min cost",min_cost,30,1e-4);
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Point 1 is (13,14)",(pts[1]-vgl_point_2d<double>(13,14)).length(),0,1e-6);
  TEST_NEAR("Point 2 is ( 7,14)",(pts[2]-vgl_point_2d<double>( 7,14)).length(),0,1e-6);
  TEST_NEAR("Point 3 is (13, 6)",(pts[3]-vgl_point_2d<double>(13, 6)).length(),0,1e-6);
  TEST_NEAR("Point 4 is ( 7, 6)",(pts[4]-vgl_point_2d<double>( 7, 6)).length(),0,1e-6);

  vcl_cout<<"Modify arcs.  Result should be unchanged."<<vcl_endl;
  dx = 4; dy = 5;
  arcs[0]=fhs_arc(0,1, dx,dy, 1, 1);
  arcs[1]=fhs_arc(0,2, -dx,dy, 1, 1);
  arcs[2]=fhs_arc(0,3, dx,-dy, 1, 1);
  arcs[3]=fhs_arc(0,4, -dx,-dy, 1, 1);

  searcher.set_tree(arcs,0);
  searcher.search(feature_response);
  min_cost = searcher.best_points(pts);
  vcl_cout<<"Min cost: "<<min_cost<<vcl_endl;
  TEST_NEAR("Min cost",min_cost,38,1e-4);
  TEST_NEAR("Root point is (10,10)",(pts[0]-vgl_point_2d<double>(10,10)).length(),0,1e-6);
  TEST_NEAR("Point 1 is (13,14)",(pts[1]-vgl_point_2d<double>(13,14)).length(),0,1e-6);
  TEST_NEAR("Point 2 is ( 7,14)",(pts[2]-vgl_point_2d<double>( 7,14)).length(),0,1e-6);
  TEST_NEAR("Point 3 is (13, 6)",(pts[3]-vgl_point_2d<double>(13, 6)).length(),0,1e-6);
  TEST_NEAR("Point 4 is ( 7, 6)",(pts[4]-vgl_point_2d<double>( 7, 6)).length(),0,1e-6);
}

void test_searcher()
{
  test_searcher1();
  test_searcher2();
}

TESTMAIN(test_searcher);
