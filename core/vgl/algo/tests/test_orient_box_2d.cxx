#include <iostream>
#include <vector>
#include <vcl_compiler.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_orient_box_2d.h>

void test_orient_box_2d()
{
  std::cout << "***************************\n"
           << " Testing vgl_orient_box_2d\n"
           << "***************************\n\n";

  vgl_point_2d<float> pmaj1(0.0f, 100.0f), pmaj2(200.0f, 100.0f);
  
  vgl_orient_box_2d<float> ob(pmaj1,pmaj2, 150.0f);
  std::vector<vgl_point_2d<float> > corns = ob.corners();
  std::cout << "Corners" << std::endl;
  for(std::vector<vgl_point_2d<float> >::iterator cit = corns.begin();
      cit != corns.end(); ++cit)
    std::cout << *cit << std::endl;
  float width = ob.width();
  float height = ob.height();
  vgl_point_2d<float> p(25.0, 25.0);
  vgl_point_2d<float> uv = ob.transform_to_obox(p);
  std::cout << "width " << width << " height " << height << " uv " << uv << std::endl;
  vgl_box_2d<float> box = ob.enclosing_box();
  std::cout << "axis_aligned box " << box << std::endl;
  TEST("box size", width == 200.0f && height == 150.0f, true);
  TEST("corners ", corns[2]==vgl_point_2d<float>(200.0f, 175.0f), true);
  TEST("uv ",  uv == vgl_point_2d<float>(-75.0f/200.0f, -0.5f), true);
  // test contains 
  vgl_point_2d<float> pin(199.0, 140.0);
  vgl_point_2d<float> pout(175.0, 180.0);
  bool good = ob.contains(pin) && !ob.contains(pout);
  TEST("contains " , good , true); 
  vgl_point_2d<float> p0(0.0f, 25.0f), p1(200.0f, 25.0f), p2(0.0f, 175.0);
  vgl_orient_box_2d<float> ob1(p0, p1, p2);
  TEST("construct from 3 pts", ob1 == ob, true);
  vgl_box_2d<float> b, br;
  b.add(vgl_point_2d<float>(0.0f, 0.0f));
  b.add(vgl_point_2d<float>(100.0f, 150.0f));
  br.add(vgl_point_2d<float>(-25.0f, 25.0f));
  br.add(vgl_point_2d<float>(125.0f, 125.0f));
  vgl_orient_box_2d<float> ob2(b, -3.1415926535897932384626433832795f/2.0f);
  vgl_orient_box_2d<float> ob3(br, 0.0f);
  TEST("test rotation constructors", ob2==ob3, true);
}
TESTMAIN(test_orient_box_2d);
