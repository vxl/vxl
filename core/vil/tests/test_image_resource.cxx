// This is mul/vil2/tests/test_image_resource.cxx
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_crop.h>
#include <vil2/vil2_clamp.h>
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

  vil2_value_range(v1, v2, view1);
  TEST("Value range is 0,10", v1 == 0.0f && v2 == 10.0f, true);

  vil2_image_resource_sptr clamp = vil2_clamp(mem, 1.0, 9.0);
  view1 = clamp->get_view(0, clamp->ni(), 0, clamp->nj());
  vil2_print_all(vcl_cout, view1);
  vil2_value_range(v1, v2, view1);
  TEST("Value range is 1,9", v1 == 1.0f && v2 == 9.0f, true);
  
}

MAIN( test_image_resource )
{
  START( "vil2_image_resource" );
  test_image_resource_1();

  SUMMARY();
}
