#ifndef vepl_test_driver_h_
#define vepl_test_driver_h_

#include <string>
#include <iostream>
#include <vil/vil_image_resource.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>

// create a 1 bit test image
vil_image_resource_sptr CreateTest1bitImage(int wd, int ht);

// create an 8 bit test image
vil_image_resource_sptr CreateTest8bitImage(int wd, int ht);

// create a 16 bit test image
vil_image_resource_sptr CreateTest16bitImage(int wd, int ht);

// create a 32 bit test image
vil_image_resource_sptr CreateTest32bitImage(int wd, int ht);

// create a 24 bit color test image
vil_image_resource_sptr CreateTest24bitImage(int wd, int ht);

// create a 24 bit color test image, with 3 planes
vil_image_resource_sptr CreateTest3planeImage(int wd, int ht);

// create a float-pixel test image
vil_image_resource_sptr CreateTestfloatImage(int wd, int ht);

// create a double-pixel test image
vil_image_resource_sptr CreateTestdoubleImage(int wd, int ht);

// Compare two images and return true if their difference is not v
bool difference(const vil_image_resource_sptr& a,
                const vil_image_resource_sptr& b,
                vxl_uint_32 v, std::string const& m, bool exact=true);

#define ONE_TEST(x,i,r,T,v,m,a) { std::cout << "Starting "<<m<<" test\n"; i = x(r a); difference(i,r,v,m); }
#define FUZ_TEST(x,i,r,T,v,m,a) { std::cout << "Starting "<<m<<" test\n"; i = x(r a); difference(i,r,v,m,false); }

#endif // vepl_test_driver_h_
