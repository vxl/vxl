#ifndef vipl_test_driver_h_
#define vipl_test_driver_h_

#include <vil/vil_image_view.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vxl_config.h> // for vxl_byte

// create an 8 bit test image
vil_image_view<vxl_byte> CreateTest8bitImage (int wd, int ht);

// create a 16 bit test image
vil_image_view<short> CreateTest16bitImage(int wd, int ht);

// create a float-pixel test image
vil_image_view<float> CreateTestfloatImage(int wd, int ht);

// Compare two images and return true if their difference is not v
template <class T>
bool difference(vil_image_view<T> const& a, vil_image_view<T> const& b, double v, vcl_string const& m, T);

#define ONE_TEST(x,p,i,r,I,T,v,m) { \
  vcl_cout << "Starting "<<m<<" test\n"; \
  x<I,I,T,T> op(p); \
  op.put_in_data_ptr(&r); op.put_out_data_ptr(&i); op.filter(); \
  if (difference(i,r,double(v),m,(T)0)) \
    vcl_cout<<m<<" test FAILED: input image changed!\n"; }

#endif // vipl_test_driver_h_
