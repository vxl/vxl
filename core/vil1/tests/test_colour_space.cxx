#include <iostream>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_colour_space.h>

#include <testlib/testlib_test.h>

static void test_colour_space()
{
  double r=10;
  double g=120;
  double b=240;

  std::cout<<"r= "<<r<<std::endl
          <<"g= "<<g<<std::endl
          <<"b= "<<b<<std::endl;

  double h, s, v;
  vil1_colour_space_RGB_to_HSV(r,g,b,&h,&s,&v);

  std::cout<<"h= "<<h<<std::endl
          <<"s= "<<s<<std::endl
          <<"v= "<<v<<std::endl;

  double nr, ng, nb;
  vil1_colour_space_HSV_to_RGB(h,s,v,&nr,&ng,&nb);

  std::cout<<"nr= "<<nr<<std::endl
          <<"ng= "<<ng<<std::endl
          <<"nb= "<<nb<<std::endl;

  double d = std::fabs (r-nr)
           + std::fabs (g-ng)
           + std::fabs (b-nb);

  std::cout<<"d= "<<d<<std::endl;

  TEST_NEAR("test r g b values", d, 0.0, 0.1);
}

TESTMAIN(test_colour_space);
