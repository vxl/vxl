// This is brl/bseg/tests/brip_test_lucas_kanade.cxx
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_math.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <testlib/testlib_test.h>

static void brip_test_lucas_kanade()
{
  vil1_memory_image_of<float> cur, prev, vx, vy, ang;
  int w = 32, h =32;
  double D_R = 180*vnl_math::one_over_pi;
  cur.resize(w,h);
  prev.resize(w,h);
  vx.resize(w,h);
  vy.resize(w,h);
  ang.resize(w,h);
  cur.fill(0.0);
  prev.fill(0.0);
  ang.fill(0.0);
  //add a single point that moves in eight different directions.
  //first set the point in the previous frame
  prev(10,10)=100.0; prev(10,15)=100.0; prev(10,20)=100.0; prev(10,25)=100.0;
  prev(20,10)=100.0; prev(20,15)=100.0; prev(20,20)=100.0; prev(20,25)=100.0;
  //0, 45, 90, 135 //1 pixel movement
  cur(11,10)=100.0; cur(12,16)=100.0; cur(10,21)=100.0; cur(9,26)=100.0;
  //180, 225, 270, 315
  cur(9,10)=100.0; cur(9,14)=100.0; cur(10,19)=100.0; cur(11,24)=100.0;
  vil1_memory_image_of<float> gauss_cur = brip_vil1_float_ops::gaussian(cur, 1.0);
  vil1_memory_image_of<float> gauss_prev = brip_vil1_float_ops::gaussian(prev, 1.0);
  brip_vil1_float_ops::Lucas_KanadeMotion(gauss_cur, gauss_prev, 1, 500.0, vx, vy);
  for (int y =0; y<h; y++)
    for (int x = 0; x<w; x++)
      ang(x,y) = float(D_R*vcl_atan2(vy(x,y), vx(x,y)));
  vcl_cout << vx(11,10)<<' '<< vx(12,16)<<' '<< vx(10,21)<<' '<< vx(9,26)<< '\n'
           << vy(11,10)<<' '<< vy(12,16)<<' '<< vy(10,21)<<' '<< vy(9,26)<< '\n'
           << vx(9,10)<< ' '<< vx(9,14)<< ' '<< vx(10,19)<<' '<< vx(11,24)<<'\n'
           << vy(9,10)<< ' '<< vy(9,14)<< ' '<< vy(10,19)<<' '<< vy(11,24)<<'\n'
           << ang(11,10)<<' '<< ang(12,16)<<' '<< ang(10,21)<<' '<< ang(9,26) << '\n'
           << ang(9,10) <<' '<< ang(9,14) <<' '<< ang(10,19)<<' '<< ang(11,24)<< '\n';

  vcl_cout << "Angle Image:\n";
  for (int y =0; y<h; y++)
  {
    for (int x = 0; x<w; x++)
      vcl_cout << ' ' << vcl_setw(3) << int(ang(x,y));
    vcl_cout << '\n';
  }
  TEST_NEAR("ang(11,10)", ang(11,10), 170.91, 0.005);
  TEST_NEAR("ang(12,16)", ang(12,16),-158.65, 0.005);
  TEST_NEAR("ang(10,21)", ang(10,21), -94.01, 0.005);
  TEST_NEAR("ang(9,26)",  ang(9,26),  -54.39, 0.005);
  TEST_NEAR("ang(9,10)",  ang(9,10),    8.74, 0.005);
  TEST_NEAR("ang(9,14)",  ang(9,14),   44.09, 0.005);
  TEST_NEAR("ang(10,19)", ang(10,19),  91.87, 0.005);
  TEST_NEAR("ang(11,24)", ang(11,24), 130.53, 0.005);
}

TESTMAIN(brip_test_lucas_kanade);
