// This is tbl/vepl2/tests/test_driver.cxx
#include <testlib/testlib_register.h>
#include "test_driver.h"
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vxl_config.h> // for vxl_byte etc.
#include <vcl_string.h>

DECLARE(vepl2_test_dilate_disk);
DECLARE(vepl2_test_dyadic);
DECLARE(vepl2_test_erode_disk);
DECLARE(vepl2_test_gaussian_convolution);
DECLARE(vepl2_test_gradient_dir);
DECLARE(vepl2_test_gradient_mag);
DECLARE(vepl2_test_median);
DECLARE(vepl2_test_moment);
DECLARE(vepl2_test_monadic);
DECLARE(vepl2_test_sobel);
DECLARE(vepl2_test_threshold);
DECLARE(vepl2_test_x_gradient);
DECLARE(vepl2_test_y_gradient);

void
register_tests()
{
  REGISTER(vepl2_test_dilate_disk);
  REGISTER(vepl2_test_dyadic);
  REGISTER(vepl2_test_erode_disk);
  REGISTER(vepl2_test_gaussian_convolution);
  REGISTER(vepl2_test_gradient_dir);
  REGISTER(vepl2_test_gradient_mag);
  REGISTER(vepl2_test_median);
  REGISTER(vepl2_test_moment);
  REGISTER(vepl2_test_monadic);
  REGISTER(vepl2_test_sobel);
  REGISTER(vepl2_test_threshold);
  REGISTER(vepl2_test_x_gradient);
  REGISTER(vepl2_test_y_gradient);
}

DEFINE_MAIN;

// create a 1 bit test image
vil_image_resource_sptr CreateTest1bitImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_BOOL);
  vil_image_view<bool> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = (((x-wd/2)*(y-ht/2)/16) & 1) == 1;
  return im;
}

// create an 8 bit test image
vil_image_resource_sptr CreateTest8bitImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<vxl_byte> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = vxl_byte(((x-wd/2)*(y-ht/2)/16) & 0xff);
  return im;
}

// create a 16 bit test image
vil_image_resource_sptr CreateTest16bitImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_UINT_16);
  vil_image_view<vxl_uint_16> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = vxl_uint_16 (((x-wd/2)*(y-ht/2)/16) & 0xffff);
  return im;
}

// create a 32 bit test image
vil_image_resource_sptr CreateTest32bitImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_UINT_32);
  vil_image_view<vxl_uint_32> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = vxl_uint_32 (((x-wd/2)*(y-ht/2)/16) & 0xffff);
  return im;
}

// create a 24 bit color test image, i.e., ubyte with 3 interleaved planes
vil_image_resource_sptr CreateTest24bitImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_RGB_BYTE);
  vil_image_view<vxl_byte> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
    {
      image(x, y, 0) = vxl_byte(x&0xff);
      image(x, y, 1) = vxl_byte(((x-wd/2)*(y-ht/2)/16)&0xff);
      image(x, y, 2) = vxl_byte((y/3)&0xff);
    }
  return im;
}

// create an 8 bit color test image, with 3 ubyte planes
vil_image_resource_sptr CreateTest3planeImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 3, VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<vxl_byte> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
    {
      image(x, y, 0) = vxl_byte(x&0xff);
      image(x, y, 1) = vxl_byte(((x-wd/2)*(y-ht/2)/16)&0xff);
      image(x, y, 2) = vxl_byte((y/3)&0xff);
    }
  return im;
}

// create a float-pixel test image
vil_image_resource_sptr CreateTestfloatImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_FLOAT);
  vil_image_view<float> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = 0.1f * ((x-wd/2)*(y-ht/2)/16);
  return im;
}

// create a double-pixel test image
vil_image_resource_sptr CreateTestdoubleImage(int wd, int ht)
{
  vil_image_resource_sptr im = vil_new_image_resource(wd, ht, 1, VIL_PIXEL_FORMAT_DOUBLE);
  vil_image_view<double> image = im->get_view();
  for (int x = 0; x < wd; x++)
    for (int y = 0; y < ht; y++)
      image(x, y) = 0.1 * ((x-wd/2)*(y-ht/2)/16);
  return im;
}

// Compare two images and return true if their difference is not v
bool difference(vil_image_resource_sptr a,
                vil_image_resource_sptr b,
                vxl_int_32 v, vcl_string const& m, bool exact)
{
  unsigned int sx = a->ni(),  sy = a->nj(), sp = a->nplanes();
  TEST("# rows match", sx, b->ni());
  TEST("# columns match", sy, b->nj());
  TEST("# planes match", sp, b->nplanes());
  TEST("Image formats match", a->pixel_format(), b->pixel_format());

  if (!exact) return false;

  vxl_int_32 ret = 0;
  // run over all pixels except for an outer border of 1 pixel:
  // The ABSX parameter is used to suppress compiler warnings by not computing
  // the absolute value of unsigned types.
#define DIFF(T) /* for non-integral types like float and double */ {\
  T r = (T)0; \
  vil_image_view<T > v1 = a->get_view(); \
  vil_image_view<T > v2 = b->get_view(); \
  vil_image_view<T >::const_iterator it1 = v1.begin(); \
  vil_image_view<T >::const_iterator it2 = v2.begin(); \
  for (unsigned int p=0; p<sp; ++p) \
    for (unsigned int j=1; j+1<sy; ++j) \
      for (unsigned int i=1; i+1<sx; ++i) { \
        T x = (*(it1+i*v1.istep()+j*v1.jstep()+p*v1.planestep())) \
             -(*(it2+i*v2.istep()+j*v2.jstep()+p*v2.planestep())); \
        r += x<0?-x:x; \
      } \
  ret = (vxl_int_32)(r+0.5); \
}
#define DIFI(T) /* for integral and for very short types like e.g. bool */ {\
  ret = 0; \
  vil_image_view<T > v1 = a->get_view(); \
  vil_image_view<T > v2 = b->get_view(); \
  vil_image_view<T >::const_iterator it1 = v1.begin(); \
  vil_image_view<T >::const_iterator it2 = v2.begin(); \
  for (unsigned int p=0; p<sp; ++p) \
    for (unsigned int j=1; j+1<sy; ++j) \
      for (unsigned int i=1; i+1<sx; ++i) { \
        vxl_int_32 x = (vxl_int_32)(*(it1+i*v1.istep()+j*v1.jstep()+p*v1.planestep())) \
                      -(vxl_int_32)(*(it2+i*v2.istep()+j*v2.jstep()+p*v2.planestep())); \
        ret += x<0?-x:x; \
      } \
}
  if (a->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) { DIFF(float); }
  else if (a->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) { DIFF(double); }
  else if (a->pixel_format() == VIL_PIXEL_FORMAT_BOOL) { DIFI(bool); }
  else if (a->pixel_format() == VIL_PIXEL_FORMAT_BYTE) { DIFI(vxl_byte); }
  else if (a->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) { DIFI(vxl_uint_16); }
  else if (a->pixel_format() == VIL_PIXEL_FORMAT_UINT_32) { DIFI(vxl_uint_32); }
#undef DIFF
#undef DIFI
  vcl_cout<<m<<": expected "<<v<<", found "<<ret<<'\n';
  TEST(m.c_str(), ret, v);
  return v!=ret;
}
