#ifndef vepl_test_driver_h_
#define vepl_test_driver_h_

#include <vil/vil_image.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

// create an 8 bit test image
vil_image CreateTest8bitImage(int wd, int ht);

// create a 16 bit test image
vil_image CreateTest16bitImage(int wd, int ht);

// create a 24 bit color test image
vil_image CreateTest24bitImage(int wd, int ht);

// create a 24 bit color test image, with 3 planes
vil_image CreateTest3planeImage(int wd, int ht);

// create a float-pixel test image
vil_image CreateTestfloatImage(int wd, int ht);

// Compare two images and return true if their difference is not v
bool difference(vil_image const& a, vil_image const& b, int v, vcl_string const& m);

#define ONE_TEST(x,i,r,T,v,m) { \
  vcl_cout << "Starting "<<m<<" test\n"; \
  i = x(r,5); \
  difference(i,r,v,m); \
  if (difference(i,r,0,"")) vcl_cout<<m<<" test FAILED: input image changed!\n"; }

#endif // vepl_test_driver_h_
