// This is core/vil2/tests/test_image_resource.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_math.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_crop.h>
#include <vil2/vil2_clamp.h>
#include <vil2/vil2_transpose.h>
#include <vil2/vil2_flip.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_decimate.h>

template <class T>
void test_image_resource(vcl_string type, vil2_pixel_format format, T  /*dummy*/)
{
  vcl_cout << "******************************************************************\n"
           << " Testing vil2_image_resource objects with pixel type = " << type << "\n"
           << "******************************************************************\n";

  vil2_image_resource_sptr mem = vil2_new_image_resource(10,8,1,format);

  TEST("vil2_new_image_resource", mem, true);

  vil2_image_view<T> view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  TEST("vil2_memory_image::get_view()",
       view1 && view1.ni()==10 && view1.nj()==8 && view1.nplanes()==1, true );

  view1.fill(0);

  TEST("vil2_memory_image::put_view()", mem->put_view(view1,0,0), true);

  vil2_image_resource_sptr crop = vil2_crop(mem, 2, 6, 2, 4);

  TEST("vil2_crop(image_resource)", crop, true);

  vil2_image_view<T> view2 = crop->get_copy_view(0, crop->ni(), 0, crop->nj());
  TEST("vil2_memory_image::get_copy_view()",
       view2 && view2.ni()==6 && view2.nj()==4 && view2.nplanes()==1, true );

  view2.fill(10);

  TEST("vil2_memory_image::put_view(copy)", crop->put_view(view2,0,0), true);

  view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  vil2_print_all(vcl_cout, view1);


  T v1, v2;

  vil2_math_value_range(view1, v1, v2);
  TEST("Value range is 0,10", v1 == 0 && v2 == 10, true);

  vil2_image_resource_sptr clamp = vil2_clamp(mem, 1.0, 9.0); // arguments of type double, not T
  view1 = clamp->get_view(0, clamp->ni(), 0, clamp->nj());
  vil2_print_all(vcl_cout, view1);
  vil2_math_value_range(view1, v1, v2);
  TEST("Value range after clamping is 1,9", v1 == 1 && v2 == 9, true);


  view2.set_size(1,1,1);
  view2.fill(T(20));
  vil2_image_resource_sptr trans = vil2_transpose(mem);
  TEST("Transpose::put_view",trans->put_view(view2,3,0), true);
  view1 = mem->get_view(0, mem->ni(), 0, mem->nj());


  trans = vil2_transpose(clamp);

  view1 = clamp->get_view(0, clamp->ni()/2, 0, clamp->nj()/2);
  TEST("Clamping", view1 && view1(3,0) == 1 && view1(0,3)==9, true);
  view1 = trans->get_view(0, trans->ni()/2, 0, trans->nj()/2);
  TEST("Transpose, and clamping", view1 && view1(3,0) == 9 && view1(0,3)==1, true);

  vil2_image_resource_sptr flip1 = vil2_flip_ud(mem);
  vil2_image_resource_sptr flip2 = vil2_flip_lr(flip1);
  vil2_image_resource_sptr flip3 = vil2_flip_ud(flip2);
  vil2_image_resource_sptr flip4 = vil2_flip_lr(flip3);
  view1 = flip4->get_view(0, flip4->ni(), 0, flip4->nj());
  view2 = mem->get_view(0, mem->ni(), 0, mem->nj());
  // Check that they are the same view of the same data
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) A", view1, view2);
  view1 = flip4->get_copy_view(0, flip4->ni(), 0, flip4->nj());
  // Check that they are not the same view of the same data
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) B", view1 != view2, true);
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) C",
       vil2_image_view_deep_equality(view1, view2), true);
  flip4->put_view(view2, 0, 0);
  view1 = mem->get_copy_view(0, mem->ni(), 0, mem->nj());
  // Check that the put view works
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x)))) D",
       vil2_image_view_deep_equality(view1, view2), true);

  vil2_image_resource_sptr dec = vil2_decimate(flip4, 2, 3);
  view1 = dec->get_view(0,dec->ni(),0,dec->nj());
  TEST("vil2_decimate get_view", view1, true);
  TEST("vil2_decimate get_copy_view", dec->get_copy_view(0,dec->ni()/2,0,dec->nj()/2), true);
  TEST("vil2_decimate get_view sizes", view1.ni() == 5 && view1.nj() == 2, true);
  TEST("vil2_decimate get_view pixels", view1(0,1) == 20.0f && view1(1,1) == 10.0f && view1(1,0) == 0.0f, true);
  view2 = flip4->get_copy_view(0,10,0,8);
  view1(0,1) = 30;
  TEST("vil2_decimate put_view", dec->put_view(view1, 0, 0), true);
  view1 = flip4->get_copy_view(0,10,0,8);
  view2(0,3) = 30;
  TEST("vil2_decimate put_view pixels",
       vil2_image_view_deep_equality(view1, view2), true);
}

MAIN( test_image_resource )
{
  START( "vil2_image_resource" );
  test_image_resource("float", VIL2_PIXEL_FORMAT_FLOAT, float());
  test_image_resource("double", VIL2_PIXEL_FORMAT_DOUBLE, double());
  test_image_resource("vxl_byte", VIL2_PIXEL_FORMAT_BYTE, vxl_byte());
  test_image_resource("vxl_sbyte", VIL2_PIXEL_FORMAT_SBYTE, vxl_sbyte());
  test_image_resource("vxl_int_16", VIL2_PIXEL_FORMAT_INT_16, vxl_int_16());
  test_image_resource("vxl_uint_16", VIL2_PIXEL_FORMAT_UINT_16, vxl_uint_16());
  test_image_resource("vxl_int_32", VIL2_PIXEL_FORMAT_INT_32, vxl_int_32());
  test_image_resource("vxl_uint_32", VIL2_PIXEL_FORMAT_UINT_32, vxl_uint_32());

  SUMMARY();
}
