#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil1/vil1_colour_space.h>

#include <testlib/testlib_test.h>

static void test_colour_space()
{
  double r=10;
  double g=120;
  double b=240;

  vcl_cout<<"r= "<<r<<vcl_endl
          <<"g= "<<g<<vcl_endl
          <<"b= "<<b<<vcl_endl;

  double h, s, v;
  vil1_colour_space_RGB_to_HSV(r,g,b,&h,&s,&v);

  vcl_cout<<"h= "<<h<<vcl_endl
          <<"s= "<<s<<vcl_endl
          <<"v= "<<v<<vcl_endl;

  double nr, ng, nb;
  vil1_colour_space_HSV_to_RGB(h,s,v,&nr,&ng,&nb);

  vcl_cout<<"nr= "<<nr<<vcl_endl
          <<"ng= "<<ng<<vcl_endl
          <<"nb= "<<nb<<vcl_endl;

  double d = vcl_fabs (r-nr)
           + vcl_fabs (g-ng)
           + vcl_fabs (b-nb);

  vcl_cout<<"d= "<<d<<vcl_endl;

  TEST_NEAR("test r g b values", d, 0.0, 0.1);
}

TESTMAIN(test_colour_space);
