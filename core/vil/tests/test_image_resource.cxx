// This is mul/vil2/tests/test_image_resource.cxx
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_view_functions.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_crop.h>
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

#if 0
  vcl_cout<<"image0: "<<image0<<vcl_endl;

  TEST("N.Planes",image0.nplanes(),1);
  TEST("resize x",image0.ni(),10);
  TEST("resize y",image0.nj(),8);

  for (int y=0;y<image0.nj();++y)
     for (int x=0;x<image0.ni();++x)
     {
       image0(x,y) = x+y;
     }

  image0.print_all(vcl_cout);

  {
    // Test the shallow copy
    vil2_image_view<vxl_byte> image1;
    image1 = image0;

    TEST("Shallow copy (size)",image0.ni()==image1.ni() && image0.nj()==image1.nj()
         && image0.nplanes()==image1.nplanes(), true);

    image0(4,6)=127;
    TEST("Shallow copy (values)",image1(4,6),image0(4,6));
  }


  vil2_image_view<vxl_byte> image2;
  {
    // Check data remains valid if a copy taken
    vil2_image_view<vxl_byte> image3;
    image3.resize(4,5,3);
    image3.fill(111);
    image2 = image3;
  }

  TEST("Shallow copy 2",image2.ni()==4
       && image2.nj()==5 && image2.nplanes()==3, true);

  image2(1,1)=17;
  TEST("Data still in scope",image2(3,3),111);
  TEST("Data still in scope",image2(1,1),17);

  vcl_cout<<image2<<vcl_endl;

  {
    // Test the deep copy
    vil2_image_view<vxl_byte> image4;
    image4.deep_copy(image0);
    TEST("Deep copy (size)",image0.ni()==image4.ni()
                         && image0.nj()==image4.nj()
                         && image0.nplanes()==image4.nplanes(), true);
    TEST("Deep copy (values)",image4(4,6),image0(4,6));

    vxl_byte v46 = image0(4,6);
    image0(4,6)=255;
    TEST("Deep copy (values really separate)",image4(4,6),v46);
  }

  vil2_image_view<vxl_byte> image_win;
  image_win.set_to_window(image0,2,4,3,4);
  TEST("set_to_window size",
       image_win.ni()==4 && image_win.nj()==4 &&
       image_win.nplanes()==image0.nplanes(),true);

  image0(2,3)=222;
  TEST("set_to_window is shallow copy",image_win(0,0),222);

  vcl_cout<<image0.is_a()<<vcl_endl;
  TEST("is_a() specialisation for vxl_byte",image0.is_a(),"vil2_image_view<vxl_byte>");

  vil2_image_view<vil_rgb<vxl_byte> > image5;
  image5.resize(5,4);
  image5.fill(vil_rgb<vxl_byte>(25,35,45));
  image5(2,2).b = 50;

  image2 = image5;
  TEST("Can assign rgb images to 3 plane view", image2, true);
  TEST("Pixels are correct", image2(2,2,1) == 35 && image2(2,2,2) == 50, true);

  image5 = image2;
  TEST("Can assign 3 planes suitable image to rgb view", image5, true);

  vil2_image_view<vil_rgba<vxl_byte> > image6 = image2;
  TEST("Can't assign a 3 plane images to rgba view", image6, false);

  vcl_cout << "***********************************\n";
  vcl_cout << " Testing vil2_image_view functions\n";
  vcl_cout << "***********************************\n";

  image2.fill(0);
  image_win = vil2_window(image2,2,1,1,2);
  image5.resize(1,2);
  image5(0,0) = vil_rgb<vxl_byte>(25,35,45);
  image5(0,1) = vil_rgb<vxl_byte>(25,35,45);
  image0 = image5;

  vil2_reformat_copy(image0, image_win);
  vil2_print_all(vcl_cout, image2);
  vil2_image_view<vxl_byte> test_image(5,4,3);
  test_image.fill(0);
  test_image(2,1,0) = test_image(2,2,0) = 25;
  test_image(2,1,1) = test_image(2,2,1) = 35;
  test_image(2,1,2) = test_image(2,2,2) = 45;
  TEST("vil2_reformat_copy, vil2_window and vil2_deep_equality", vil2_deep_equality(test_image,image2), true);
  test_image(2,2,2) = 44;
  TEST("!vil2_deep_equality", vil2_deep_equality(test_image,image2), false);
  test_image.resize(5,4,4);
  TEST("!vil2_deep_equality", vil2_deep_equality(test_image,image2), false);
#endif
}

MAIN( test_image_resource )
{
  START( "vil2_image_resource" );
  test_image_resource_1();

  SUMMARY();
}
