// This is mul/vil2/tests/test_image_resource.cxx
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_math.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_crop.h>
#include <vil2/vil2_clamp.h>
#include <vil2/vil2_transpose.h>
#include <vil2/vil2_flip.h>
#include <vil2/vil2_print.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>


void test_image_resource_1()
{
  vcl_cout << "*********************************\n";
  vcl_cout << " Testing vil2_image_resource objects\n";
  vcl_cout << "*********************************\n";

  vil2_image_resource_sptr mem = vil2_new_image_resource(10,8,1,VIL2_PIXEL_FORMAT_FLOAT);
//  vil2_memory_image mem(10,8,1,VIL2_PIXEL_FORMAT_FLOAT);

  TEST("vil2_new_image_resource", mem, true);

  vil2_image_view<float> view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  TEST("vil2_memory_image::get_view()", view1 && view1.ni()==10 && view1.nj()==8
    && view1.nplanes()==1, true );

  view1.fill(0);

  TEST("vil2_memory_image::put_view()", mem->put_view(view1,0,0), true);

  vil2_image_resource_sptr crop = vil2_crop(mem, 2, 6, 2, 4);

  TEST("vil2_crop(image_resource)", crop, true);

  vil2_image_view<float> view2 = crop->get_copy_view(0, crop->ni(), 0, crop->nj());
  TEST("vil2_memory_image::get_copy_view()",
       view2 && view2.ni()==6 && view2.nj()==4 && view2.nplanes()==1, true );

  view2.fill(10.0);

  TEST("vil2_memory_image::put_view(copy)", crop->put_view(view2,0,0), true);

  view1 = mem->get_view(0, mem->ni(), 0, mem->nj());
  vil2_print_all(vcl_cout, view1);


  float v1, v2;

  vil2_math_value_range(v1, v2, view1);
  TEST("Value range is 0,10", v1 == 0.0f && v2 == 10.0f, true);

  vil2_image_resource_sptr clamp = vil2_clamp(mem, 1.0, 9.0);
  view1 = clamp->get_view(0, clamp->ni(), 0, clamp->nj());
  vil2_print_all(vcl_cout, view1);
  vil2_math_value_range(v1, v2, view1);
  TEST("Value range after clamping is 1,9", v1 == 1.0f && v2 == 9.0f, true);


  view2.resize(1,1,1);
  view2.fill(20.0);
  vil2_image_resource_sptr trans = vil2_transpose(mem);
  TEST("Transpose::put_view",trans->put_view(view2,3,0), true);
  view1 = mem->get_view(0, mem->ni(), 0, mem->nj());


  trans = vil2_transpose(clamp);

  view1 = clamp->get_view(0, clamp->ni()/2, 0, clamp->nj()/2);
  TEST("Clamping", view1 && view1(3,0) == 1.0f && view1(0,3)==9.0f, true);
  view1 = trans->get_view(0, trans->ni()/2, 0, trans->nj()/2);
  TEST("Transpose, and clamping", view1 && view1(3,0) == 9.0f && view1(0,3)==1.0f, true);


  vil2_image_resource_sptr flip1 = vil2_flip_lr(clamp);
  vil2_image_resource_sptr flip2 = vil2_flip_ud(flip1);
  vil2_image_resource_sptr flip3 = vil2_flip_lr(flip2);
  vil2_image_resource_sptr flip4 = vil2_flip_ud(flip3);
  view1 = flip4->get_view(0, flip4->ni(), 0, flip4->nj());
  view2 = clamp->get_view(0, clamp->ni(), 0, clamp->nj());
  TEST("x == flip_lr(flip_ud(flip_lr(flip_ud(x))))",
    vil2_image_view_deep_equality(view1, view2), true);

}

MAIN( test_image_resource )
{
  START( "vil2_image_resource" );
  test_image_resource_1();

  SUMMARY();
}
