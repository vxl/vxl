#ifndef vepl1_test_driver_h_
#define vepl1_test_driver_h_

#include <vil1/vil1_image.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

// create an 8 bit test image
vil1_image CreateTest8bitImage(int wd, int ht);

// create a 16 bit test image
vil1_image CreateTest16bitImage(int wd, int ht);

// create a 24 bit color test image
vil1_image CreateTest24bitImage(int wd, int ht);

// create a 24 bit color test image, with 3 planes
vil1_image CreateTest3planeImage(int wd, int ht);

// create a float-pixel test image
vil1_image CreateTestfloatImage(int wd, int ht);

// Compare two images and return true if their difference is not v
bool difference(vil1_image const& a, vil1_image const& b, int v, vcl_string const& m);

#define ONE_TEST(x,i,r,T,v,m) { \
  vcl_cout << "Starting "<<m<<" test\n"; \
  i = x(r,5); \
  difference(i,r,v,m); \
  if (difference(i,r,0,"")) vcl_cout<<m<<" test FAILED: input image changed!\n"; }

#endif // vepl1_test_driver_h_
