#include <vcl_iostream.h>
#include <vil/vil_colour_space.h>
#include <vil/vil_byte.h>
#include <vcl_cmath.h>

#include <testlib/testlib_test.h>

MAIN( test_colour_space )
{
  START( "colour space" );

  double r, g, b, h ,s ,v, nr, ng, nb;
  r=10;
  g=120;
  b=240;

  vcl_cout<<"r= "<<r<<vcl_endl;
  vcl_cout<<"g= "<<g<<vcl_endl;
  vcl_cout<<"b= "<<b<<vcl_endl;

  vil_colour_space_RGB_to_HSV(r,g,b,&h,&s,&v);

  vcl_cout<<"h= "<<h<<vcl_endl;
  vcl_cout<<"s= "<<s<<vcl_endl;
  vcl_cout<<"v= "<<v<<vcl_endl;

  vil_colour_space_HSV_to_RGB(h,s,v,&nr,&ng,&nb);

  vcl_cout<<"nr= "<<nr<<vcl_endl;
  vcl_cout<<"ng= "<<ng<<vcl_endl;
  vcl_cout<<"nb= "<<nb<<vcl_endl;

  double d;
  d= vcl_fabs (r-nr);
  d+= vcl_fabs (g-ng);
  d+= vcl_fabs (b-nb);

  vcl_cout<<"d= "<<d<<vcl_endl;

  TEST ("test r g b values", d<0.1, true);

  SUMMARY();
}
