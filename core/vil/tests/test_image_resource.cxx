// This is core/vil/tests/test_image_resource.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vcl_complex.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_math.h>
#include <vil/vil_new.h>
#include <vil/vil_crop.h>
#include <vil/vil_clamp.h>
#include <vil/vil_transpose.h>
#include <vil/vil_flip.h>
#include <vil/vil_print.h>
#include <vil/vil_decimate.h>

// This is a "dummy" operator< for complex<T>.  This function is never
// actually called, but declaring it makes it possible to use a single
// template implementation for test_image_resource().  - PVr
static inline bool operator< (vcl_complex<float> const&, vcl_complex<float> const&) { return false; }
static inline bool operator< (vcl_complex<double>const&, vcl_complex<double>const&) { return false; }
static inline bool operator> (vcl_complex<float> const&, vcl_complex<float> const&) { return false; }
static inline bool operator> (vcl_complex<double>const&, vcl_complex<double>const&) { return false; }

template <class T>
void test_image_resource(vcl_string type, vil_pixel_format format, T /*dummy*/)
{
  vcl_cout << "*****************************************************************\n"
           << " Testing vil_image_resource objects with pixel type = " << type << '\n'
           << "*****************************************************************\n";

  vil_image_resource_sptr mem = vil_new_image_resource(10,8,1,format);

  TEST("vil_new_image_resource", mem?true:false, true);

  vil_image_view<T> view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  TEST("vil_memory_image::get_view()",
       view1 && view1.ni()==10 && view1.nj()==8 && view1.nplanes()==1, true );

  vil_image_resource_sptr mem2 = vil_new_image_resource_of_view(view1);

  TEST("vil_new_image_resource_of_view", mem2 && view1==(*mem2->get_view()), true);


  view1.fill(0);

  TEST("vil_memory_image::put_view()", mem->put_view(view1), true);

  vil_image_resource_sptr crop = vil_crop(mem, 2, 6, 2, 4);

  TEST("vil_crop(image_resource)", crop?true:false, true);

  vil_image_view<T> view2 = crop->get_copy_view(0, crop->ni(), 0, crop->nj());
  TEST("vil_memory_image::get_copy_view()",
       view2 && view2.ni()==6 && view2.nj()==4 && view2.nplanes()==1, true );

  view2.fill(10);

  TEST("vil_memory_image::put_view(copy)", crop->put_view(view2,0,0), true);

  view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  vil_print_all(vcl_cout, view1);

  if (format!=VIL_PIXEL_FORMAT_COMPLEX_FLOAT &&
      format!=VIL_PIXEL_FORMAT_COMPLEX_DOUBLE)
  { // Can't do clamping, or math_value_range on a complex<T>
    T v1, v2;

    vil_math_value_range(view1, v1, v2);
    TEST("Value range is 0,10", v1 == T(0) && v2 == T(10), true);

    vil_image_resource_sptr clamp = vil_clamp(mem, 1.0, 9.0); // arguments of type double, not T
    view1 = clamp->get_view(0, clamp->ni(), 0, clamp->nj());
    vil_print_all(vcl_cout, view1);
    vil_math_value_range(view1, v1, v2);
    TEST("Value range after clamping is 1,9", v1 == T(1) && v2 == T(9), true);


    view2.set_size(1,1,1);
    view2.fill(T(20));
    vil_image_resource_sptr trans = vil_transpose(mem);
    TEST("Transpose::put_view",trans->put_view(view2,3,0), true);
    view1 = mem->get_view(0, mem->ni(), 0, mem->nj());


    trans = vil_transpose(clamp);

    view1 = clamp->get_view(0, clamp->ni()/2, 0, clamp->nj()/2);
    TEST("Clamping", view1 && view1(3,0) == T(1) && view1(0,3)==T(9), true);
    view1 = trans->get_view(0, trans->ni()/2, 0, trans->nj()/2);
    TEST("Transpose, and clamping", view1 && view1(3,0) == T(9) && view1(0,3)==T(1), true);
  }
  else // Simulate effect of above code.
  {
    view1.set_size(1,1);
    view1(0,0)=T(20);
    mem->put_view(view1,0,3);
  }

  vil_image_resource_sptr flip1 = vil_flip_ud(mem);
  vil_image_resource_sptr flip2 = vil_flip_lr(flip1);
  vil_image_resource_sptr flip3 = vil_flip_ud(flip2);
  vil_image_resource_sptr flip4 = vil_flip_lr(flip3);
  view1 = flip4->get_view(0, flip4->ni(), 0, flip4->nj());
  view2 = mem->get_view(0, mem->ni(), 0, mem->nj());
  // Check that they are the same view of the same data
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) A", view1, view2);
  view1 = flip4->get_copy_view(0, flip4->ni(), 0, flip4->nj());
  // Check that they are not the same view of the same data
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) B", view1 != view2, true);
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) C",
       vil_image_view_deep_equality(view1, view2), true);
  flip4->put_view(view2, 0, 0);
  view1 = mem->get_copy_view(0, mem->ni(), 0, mem->nj());
  // Check that the put view works
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) D",
       vil_image_view_deep_equality(view1, view2), true);

  vil_image_resource_sptr dec = vil_decimate(flip4, 2, 3);
  view1 = dec->get_view(0,dec->ni(),0,dec->nj());
  TEST("vil_decimate get_view", view1?true:false, true);
  TEST("vil_decimate get_copy_view", dec->get_copy_view(0,dec->ni()/2,0,dec->nj()/2)?true:false, true);
  TEST("vil_decimate get_view sizes", view1.ni() == 5 && view1.nj() == 2, true);
  TEST("vil_decimate get_view pixels",
       view1(0,1) == T(20) && view1(1,1) == T(10) && view1(1,0) == T(0), true);
#if 0 // vil_decimate put_view has been disabled because it does't behave as we might expect.
  view2 = flip4->get_copy_view(0,10,0,8);
  view1(0,1) = 30;
  TEST("vil_decimate put_view", dec->put_view(view1, 0, 0), true);
  view1 = flip4->get_copy_view(0,10,0,8);
  view2(0,3) = 30;
  TEST("vil_decimate put_view pixels",
       vil_image_view_deep_equality(view1, view2), true);
#endif
}

void test_image_resource()
{
  test_image_resource("float", VIL_PIXEL_FORMAT_FLOAT, float());
  test_image_resource("double", VIL_PIXEL_FORMAT_DOUBLE, double());
  test_image_resource("vxl_byte", VIL_PIXEL_FORMAT_BYTE, vxl_byte());
  test_image_resource("vxl_sbyte", VIL_PIXEL_FORMAT_SBYTE, vxl_sbyte());
  test_image_resource("vxl_int_16", VIL_PIXEL_FORMAT_INT_16, vxl_int_16());
  test_image_resource("vxl_uint_16", VIL_PIXEL_FORMAT_UINT_16, vxl_uint_16());
  test_image_resource("vxl_int_32", VIL_PIXEL_FORMAT_INT_32, vxl_int_32());
  test_image_resource("vxl_uint_32", VIL_PIXEL_FORMAT_UINT_32, vxl_uint_32());
#if VXL_HAS_INT_64
  test_image_resource("vxl_int_64", VIL_PIXEL_FORMAT_INT_64, vxl_int_64());
  test_image_resource("vxl_uint_64", VIL_PIXEL_FORMAT_UINT_64, vxl_uint_64());
#endif
  test_image_resource("vcl_complex<float>", VIL_PIXEL_FORMAT_COMPLEX_FLOAT, vcl_complex<float>());
  test_image_resource("vcl_complex<double>", VIL_PIXEL_FORMAT_COMPLEX_DOUBLE, vcl_complex<double>());
}

TESTMAIN(test_image_resource);
