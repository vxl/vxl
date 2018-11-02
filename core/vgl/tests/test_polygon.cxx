// Some tests for vgl_polygon
// Amitha Perera, Sep 2001.
#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#include <vgl/vgl_polygon.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_simple_polygon()
{
  std::cout << "Simple polygon\n";

  // Simple triangle
  vgl_polygon<double> p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );
  p.print(std::cout);

  TEST("inside (1)",  p.contains( 0.0,  0.0 ), true );
  TEST("inside (2)",  p.contains( 5.0,  0.0 ), true );
  TEST("inside (3)",  p.contains( 5.0,  1.0 ), true );
  TEST("inside (4)",  p.contains( 2.5,  0.0 ), true );
  TEST("inside (5)",  p.contains( 2.5,  0.3 ), true );
  TEST("inside (6)",  p.contains( 2.5,  0.5 ), true );
  TEST("inside (7)",  p.contains( 5.0,  0.5 ), true );
  TEST("outside (1)", p.contains( 2.5,  0.6 ), false );
  TEST("outside (2)", p.contains( 5.1,  0.1 ), false );
  TEST("outside (3)", p.contains( 5.1,  0.0 ), false );
  TEST("outside (4)", p.contains( 5.0,  1.1 ), false );
  TEST("outside (5)", p.contains( 5.0, -0.1 ), false );
  TEST("outside (6)", p.contains( 2.0, -1.0 ), false );
  TEST("outside (7)", p.contains(-2.5, -0.5 ), false );
}

static void test_disjoint_polygon()
{
  std::cout << "Disjoint polygons\n";

  // Simple triangle
  vgl_polygon<float> p;
  p.new_sheet();
  p.push_back( 0.0f, 0.0f );
  p.push_back( 5.0f, 0.0f );
  p.push_back( 5.0f, 1.0f );
  // Another disjoint triangle
  p.new_sheet();
  p.push_back( 10.0f, 10.0f );
  p.push_back( 15.0f, 10.0f );
  p.push_back( 15.0f, 11.0f );
  p.print(std::cout);

  TEST("inside poly1",p.contains(  2.5f,  0.3f ), true );
  TEST("inside poly2",p.contains( 12.5f, 10.3f ), true );
  TEST("outside (1)", p.contains(  2.5f,  0.6f ), false );
  TEST("outside (2)", p.contains(  5.0f,  1.1f ), false );
  TEST("outside (3)", p.contains(  5.1f,  0.0f ), false );
  TEST("outside (4)", p.contains(  2.0f, -1.0f ), false );
  TEST("outside (5)", p.contains( -2.5f, -0.5f ), false );
  TEST("oustide (6)", p.contains( 12.5f, 10.6f ), false );
  TEST("outside (7)", p.contains( 15.0f,  9.0f ), false );
}

static void test_holey_polygon()
{
  std::cout << "Polygon with holes\n";

  // Simple triangle
  vgl_polygon<double> p;
  p.new_sheet();
  p.push_back( 0.0, 0.0 );
  p.push_back( 5.0, 0.0 );
  p.push_back( 5.0, 1.0 );
  // A hole
  p.new_sheet();
  p.push_back( 3.0, 0.5 );
  p.push_back( 4.0, 0.5 );
  p.push_back( 4.0, 0.1 );
  p.print(std::cout);

  TEST("inside",      p.contains( 2.5,  0.3 ), true );
  TEST("outside (1)", p.contains( 2.5,  0.6 ), false );
  TEST("outside (2)", p.contains( 5.0,  1.1 ), false );
  TEST("outside (3)", p.contains( 5.1,  0.0 ), false );
  TEST("outside (4)", p.contains( 2.0, -1.0 ), false );
  TEST("outside (5)", p.contains(-2.5, -0.5 ), false );
  TEST("outside (6)", p.contains( 3.9,  0.4 ), false );
  std::ofstream os("./temp");
  p.print(os);
  // read the poly
  os.close();
  std::ifstream is("./temp");
  vgl_polygon<double> pr;
  is >>pr;
  pr.print(std::cout);
  TEST("inside after read",      pr.contains( 2.5,  0.3 ), true );
}

static void test_self_intersection()
{
  std::cout << "compute polygon self intersections\n";
  std::vector<std::pair<unsigned,unsigned> > e1, e2;
  std::vector<vgl_point_2d<double> > ip;

  {
    vgl_polygon<double> p;
    // non-self-intersecting quad
    p.new_sheet();
    p.push_back( 0.0, 0.0 );
    p.push_back( 1.0, 0.0 );
    p.push_back( 1.0, 1.0 );
    p.push_back( 0.0, 1.0 );

    vgl_selfintersections(p, e1, e2, ip);
    TEST("non-self-intersecting quad", e1.empty() &&
         e2.empty() && ip.empty(), true);
  }

  {
    vgl_polygon<double> p;
    // simple self intersecting quad
    p.new_sheet();
    p.push_back( 0.0, 0.0 );
    p.push_back( 1.0, 1.0 );
    p.push_back( 0.0, 1.0 );
    p.push_back( 1.0, 0.0 );

    vgl_selfintersections(p, e1, e2, ip);
    TEST("simple self-intersecting quad", e1.size()==1 &&
         e1[0].first == 0 && e1[0].second == 0 &&
         e2[0].first == 0 && e2[0].second == 2 &&
         ip[0] == vgl_point_2d<double>(0.5,0.5), true);
  }
  {
    vgl_polygon<double> p;
    // non-self-intersecting polygon with collinear segments
    p.new_sheet();
    p.push_back( 0.0, 0.0 );
    p.push_back( 1.0, 0.0 );
    p.push_back( 1.0, 1.0 );
    p.push_back( 2.0, 1.0 );
    p.push_back( 2.0, 0.0 );
    p.push_back( 3.0, 0.0 );
    p.push_back( 3.0, 1.0 );
    p.push_back( 4.0, 1.0 );
    p.push_back( 4.0, 2.0 );
    p.push_back( 0.0, 2.0 );

    vgl_selfintersections(p, e1, e2, ip);
    TEST("collinear non-self-intersecting polygon ", e1.empty() &&
         e2.empty() && ip.empty(), true);
  }
  {
    vgl_polygon<double> p;
    // multisheet self-intersecting polygon
    p.new_sheet();
    p.push_back( 0.0, 0.0 );
    p.push_back( 1.0, 1.0 );
    p.push_back( 0.0, 1.0 );
    p.push_back( 1.0, 0.0 );
    p.new_sheet();
    p.push_back( -1.0, -1.0 );
    p.push_back( -1.0, 2.0 );
    p.push_back( 2.0, 2.0 );
    p.push_back( 2.0, -1.0 );
    p.new_sheet();
    p.push_back( 0.5, 0.75 );
    p.push_back( 2.5, 0.75 );
    p.push_back( 2.5, 2.5 );
    p.push_back( 0.5, 2.5 );

    // the correct solutions, but order may be incorrect
    typedef std::pair<unsigned,unsigned> upair;
    std::vector<upair> e1s(5), e2s(5);
    std::vector<vgl_point_2d<double> > ips(5);
    e1s[0]=upair(0,0);  e2s[0]=upair(0,2);  ips[0]=vgl_point_2d<double>(.5,.5);
    e1s[1]=upair(0,0);  e2s[1]=upair(2,0);  ips[1]=vgl_point_2d<double>(.75,.75);
    e1s[2]=upair(0,1);  e2s[2]=upair(2,3);  ips[2]=vgl_point_2d<double>(.5,1);
    e1s[3]=upair(1,1);  e2s[3]=upair(2,3);  ips[3]=vgl_point_2d<double>(.5,2);
    e1s[4]=upair(1,2);  e2s[4]=upair(2,0);  ips[4]=vgl_point_2d<double>(2,.75);

    vgl_selfintersections(p, e1, e2, ip);
    bool valid = e1.size()==5;
    for (unsigned int i=0; valid && i<5; ++i){
      bool match = false;
      for (unsigned int j=0; valid && j<e1s.size(); ++j){
        if (e1[i]==e1s[j] && e2[i]==e2s[j] && ip[i]==ips[j]){
          e1s.erase(e1s.begin()+j);
          e2s.erase(e2s.begin()+j);
          ips.erase(ips.begin()+j);
          match = true;
          break;
        }
      }
      if (!match)
        valid = false;
    }
    TEST("multisheet self-intersecting polygon", valid, true);
  }
  {
    vgl_polygon<double> p;
    // self-intersections at points
    p.new_sheet();
    p.push_back( -1.0, 0.0 );
    p.push_back( 0.0, 1.0 );
    p.push_back( 2.0, 1.0 );
    p.push_back( 2.0, 0.0 );
    p.push_back( 0.0, 1.0 );
    p.push_back( -1.0, 3.0 );
    p.new_sheet();
    p.push_back( -2.0, 3.0 );
    p.push_back( -2.0, 0.0 );
    p.push_back( 0.0, 0.0 );
    p.push_back( 0.0, 3.0 );

    // the correct solutions, but order may be incorrect
    typedef std::pair<unsigned,unsigned> upair;
    std::vector<upair> e1s(12), e2s(12);
    std::vector<vgl_point_2d<double> > ips(12);
    e1s[0]=upair(0,0);  e2s[0]=upair(0,3);  ips[0]=vgl_point_2d<double>(0,1);
    e1s[1]=upair(0,1);  e2s[1]=upair(0,3);  ips[1]=vgl_point_2d<double>(0,1);
    e1s[2]=upair(0,0);  e2s[2]=upair(0,4);  ips[2]=vgl_point_2d<double>(0,1);
    e1s[3]=upair(0,1);  e2s[3]=upair(0,4);  ips[3]=vgl_point_2d<double>(0,1);
    e1s[4]=upair(0,0);  e2s[4]=upair(1,2);  ips[4]=vgl_point_2d<double>(0,1);
    e1s[5]=upair(0,1);  e2s[5]=upair(1,2);  ips[5]=vgl_point_2d<double>(0,1);
    e1s[6]=upair(0,3);  e2s[6]=upair(1,2);  ips[6]=vgl_point_2d<double>(0,1);
    e1s[7]=upair(0,4);  e2s[7]=upair(1,2);  ips[7]=vgl_point_2d<double>(0,1);
    e1s[8]=upair(0,0);  e2s[8]=upair(1,1);  ips[8]=vgl_point_2d<double>(-1,0);
    e1s[9]=upair(0,5);  e2s[9]=upair(1,1);  ips[9]=vgl_point_2d<double>(-1,0);
    e1s[10]=upair(0,4); e2s[10]=upair(1,3); ips[10]=vgl_point_2d<double>(-1,3);
    e1s[11]=upair(0,5); e2s[11]=upair(1,3); ips[11]=vgl_point_2d<double>(-1,3);

    vgl_selfintersections(p, e1, e2, ip);
    bool valid = e1.size()==12;
    for (unsigned int i=0; valid && i<12; ++i){
      bool match = false;
      for (unsigned int j=0; valid && j<e1s.size(); ++j){
        if (e1[i]==e1s[j] && e2[i]==e2s[j] && ip[i]==ips[j]){
          e1s.erase(e1s.begin()+j);
          e2s.erase(e2s.begin()+j);
          ips.erase(ips.begin()+j);
          match = true;
          break;
        }
      }
      if (!match)
        valid = false;
    }
    TEST("self-intersections at points", valid, true);
  }
}

static void test_polygon()
{
  test_simple_polygon();
  test_disjoint_polygon();
  test_holey_polygon();
  test_self_intersection();
}

TESTMAIN(test_polygon);
