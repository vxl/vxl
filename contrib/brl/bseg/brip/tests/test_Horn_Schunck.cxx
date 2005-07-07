// This is brl/bseg/brip/tests/test_Horn_Schunck.cxx
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>
#include <brip/brip_vil_float_ops.h>
#include <testlib/testlib_test.h>

static void test_Horn_Schunck()
{
  vil_image_view<float> cur, prev, vx, vy, ang;
  int w = 16, h =16;
  double D_R = 180*vnl_math::one_over_pi;
  cur.set_size(w,h);
  prev.set_size(w,h);
  vx.set_size(w,h);
  vy.set_size(w,h);
  ang.set_size(w,h);

  cur.fill(0.0);
  prev.fill(0.0);
  ang.fill(0.0);
  vx.fill(0.0);
  vy.fill(0.0);

  //add a single point that moves in eight different directions.
  //first set the point in the previous frame
  prev(5,5)=100.0; prev(5,10)=100.0; prev(10,10)=100.0; prev(10,5)=100.0;
  //0, 135, 90, 45 //1 pixel movement
  cur(4,5)=100.0; cur(6,11)=100.0; cur(10,11)=100.0; cur(9,4)=100.0;

  vil_image_view<float> gauss_cur = brip_vil_float_ops::gaussian(cur, 1.0);
  vil_image_view<float> gauss_prev = brip_vil_float_ops::gaussian(prev,1.0);
  
  brip_vil_float_ops::Horn_SchunckMotion(gauss_cur, gauss_prev,vx, vy,10000.0f,20 );
  
  for (int y =0; y<h; y++)
    for (int x = 0; x<w; x++)
      ang(x,y) = float(D_R*vcl_atan2(vy(x,y), vx(x,y)));
  
  vcl_cout << vx(5,5)<<' '<< vx(4,5)<<' '<< vx(5,10)<<' '<< vx(6,11)<< '\n'
           << vy(5,5)<<' '<< vy(4,5)<<' '<< vy(5,10)<<' '<< vy(6,11)<< '\n'
           << vx(10,10)<< ' '<< vx(10,11)<< ' '<< vx(10,5)<<' '<< vx(9,4)<<'\n'
           << vy(10,10)<< ' '<< vy(10,11)<< ' '<< vy(10,5)<<' '<< vy(9,4)<<'\n'
           << ang(5,5)<<' '<< ang(4,5)<<' '<< ang(5,10)<<' '<< ang(6,11) << '\n'
           << ang(10,10) <<' '<< ang(10,11) <<' '<< ang(10,5)<<' '<< ang(9,4)<< '\n';

  vcl_cout << "Angle Image:\n";
  for (int y =0; y<h; y++)
  {
    for (int x = 0; x<w; x++)
      vcl_cout << ' ' << vcl_setw(3) << int(ang(x,y));
    vcl_cout << '\n';
  }
  TEST_NEAR("ang(5,5)", ang(5,5), -175.95 , 0.005);
  TEST_NEAR("ang(4,5)", ang(4,5), -179.709, 0.005);
  TEST_NEAR("ang(5,10)", ang(5,10), 62.0676, 0.005);
  TEST_NEAR("ang(6,11)",  ang(6,11), 58.0239, 0.005);
  TEST_NEAR("ang(10,10)",  ang(10,10),93.1663, 0.005);
  TEST_NEAR("ang(10,11)",  ang(10,11),88.9808, 0.005);
  TEST_NEAR("ang(10,5)", ang(10,5), -143.654, 0.005);
  TEST_NEAR("ang(9,4)", ang(9,4), -133.667, 0.005);
}

TESTMAIN(test_Horn_Schunck);
