#ifndef vepl2_test_driver_h_
#define vepl2_test_driver_h_

#include <vil2/vil2_image_view_base.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

// create a 1 bit test image
vil2_image_view_base_sptr CreateTest1bitImage(int wd, int ht);

// create an 8 bit test image
vil2_image_view_base_sptr CreateTest8bitImage(int wd, int ht);

// create a 16 bit test image
vil2_image_view_base_sptr CreateTest16bitImage(int wd, int ht);

// create a 32 bit test image
vil2_image_view_base_sptr CreateTest32bitImage(int wd, int ht);

// create a 24 bit color test image
vil2_image_view_base_sptr CreateTest24bitImage(int wd, int ht);

// create a 24 bit color test image, with 3 planes
vil2_image_view_base_sptr CreateTest3planeImage(int wd, int ht);

// create a float-pixel test image
vil2_image_view_base_sptr CreateTestfloatImage(int wd, int ht);

// create a oublei-pixel test image
vil2_image_view_base_sptr CreateTestdoubleImage(int wd, int ht);

// Compare two images and return true if their difference is not v
bool difference(vil2_image_view_base_sptr const& a,
                vil2_image_view_base_sptr const& b,
                vxl_int_32 v, vcl_string const& m, bool exact=true);

#define ONE_TEST(x,i,r,T,v,m,a) { vcl_cout << "Starting "<<m<<" test\n"; i = x(*r a); difference(i,r,v,m); }
#define FUZ_TEST(x,i,r,T,v,m,a) { vcl_cout << "Starting "<<m<<" test\n"; i = x(*r a); difference(i,r,v,m,false); }

#endif // vepl2_test_driver_h_
