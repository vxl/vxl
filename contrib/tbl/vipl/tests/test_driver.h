#ifndef vipl_test_driver_h_
#define vipl_test_driver_h_

#include <vil1/vil1_image.h>
// #include <mil/mil_image_2d_of.h>
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

#if 0
// create an 8 bit test image
mil_image_2d_of<unsigned char> Create_mil8bitImage(int wd, int ht);

// create a 16 bit test image
mil_image_2d_of<short> Create_mil16bitImage(int wd, int ht);

// create a float-pixel test image
mil_image_2d_of<float> Create_milfloatImage(int wd, int ht);
#endif

// Compare two images and return true if their difference is not v
template <class T>
bool difference(vil1_image const& a, vil1_image const& b, double v, vcl_string const& m, T);
// template <class T>
// bool difference(mil_image_2d_of<T> const& a, mil_image_2d_of<T> const& b, double v, vcl_string const& m, T);

#define ONE_TEST(x,p,i,r,I,T,v,m) { \
  vcl_cout << "Starting "<<m<<" test\n"; \
  x<I,I,T,T> op(p); \
  op.put_in_data_ptr(&r); op.put_out_data_ptr(&i); op.filter(); \
  if (difference(i,r,double(v),m,(T)0)) \
    vcl_cout<<m<<" test FAILED: input image changed!\n"; }

#endif // vipl_test_driver_h_
