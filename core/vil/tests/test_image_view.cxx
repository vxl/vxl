// This is mul/vil2/tests/test_image_view.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_functional.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_crop.h>
#include <vil2/vil2_copy.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_plane.h>
#include <vil2/vil2_convert.h>
#include <vil2/vil2_view_as.h>
#include <vil2/vil2_image_view.h>

bool Equal(const vil2_image_view<vxl_byte>& im0,
           const vil2_image_view<vxl_byte>& im1)
{
  return im0.nplanes()==im1.nplanes()
      && im0.ni() == im1.ni()
      && im0.nj() == im1.nj()
      && im0(0,0,0) == im1(0,0,0)
      && im0(1,1,1) == im1(1,1,1);
}


void test_image_view_byte()
{
  vcl_cout << "*******************************\n"
           << " Testing vil2_image_view<byte>\n"
           << "*******************************\n";

  vil2_image_view<vxl_byte> image0;
  image0.resize(10,8);
  vcl_cout<<"image0: "<<image0<<vcl_endl;

  TEST("N.Planes",image0.nplanes(),1);
  TEST("resize x",image0.ni(),10);
  TEST("resize y",image0.nj(),8);

  for (unsigned int y=0;y<image0.nj();++y)
     for (unsigned int x=0;x<image0.ni();++x)
       image0(x,y) = x+y;

  vil2_print_all(vcl_cout, image0);

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
       image_win.ni()==4 && image_win.nj()==4 && image_win.nplanes()==image0.nplanes(),
       true);

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

  vcl_cout << "***********************************\n"
           << " Testing vil2_image_view functions\n"
           << "***********************************\n";

  image2.fill(0);
  image_win = vil2_crop(image2,2,1,1,2);
  image5.resize(1,2);
  image5(0,0) = vil_rgb<vxl_byte>(25,35,45);
  image5(0,1) = vil_rgb<vxl_byte>(25,35,45);
  image0 = image5;

  vil2_copy_reformat(image0, image_win);
  vil2_print_all(vcl_cout, image2);
  vil2_image_view<vxl_byte> test_image(5,4,3);
  test_image.fill(0);
  test_image(2,1,0) = test_image(2,2,0) = 25;
  test_image(2,1,1) = test_image(2,2,1) = 35;
  test_image(2,1,2) = test_image(2,2,2) = 45;
  TEST("vil2_reformat_copy, vil2_window and vil2_deep_equality",
       vil2_image_view_deep_equality(test_image,image2), true);
  test_image(2,2,2) = 44;
  TEST("!vil2_deep_equality", vil2_image_view_deep_equality(test_image,image2), false);
  test_image.resize(5,4,4);
  TEST("!vil2_deep_equality", vil2_image_view_deep_equality(test_image,image2), false);
  vil2_print_all(vcl_cout, image2);

  vil2_image_view<float> image7;
  vil2_convert_cast(image2, image7);
  vil2_print_all(vcl_cout, image7);
  vil2_transform(image7, image7, vcl_bind2nd(vcl_plus<float>(),0.6f));
  vil2_convert_cast(image7, image2);
  vil2_print_all(vcl_cout, image2);
  TEST("Rounding up", image2(0,0,0) == 1 && image2(2,2,1) == 36, true);

  image7.clear();
  vil2_convert_rgb_to_grey(vil2_view_as_rgb(image2), image7);
  vil2_print_all(vcl_cout, image7);
  TEST("vil2_convert_rgb_to_grey(vil2_rgba)", image7, true);
  TEST_NEAR("Conversion rgb to grey", image7(0,0), 1.0, 1e-5);
  TEST_NEAR("Conversion rgb to grey", image7(2,1), 34.5960, 1e-5);
  vil2_convert_grey_to_rgb(image7, image5);
  TEST("vil2_convert_grey_to_rgb", image5, true);
  vil2_print_all(vcl_cout, image5);

  vil2_convert_grey_to_rgba(image7, image6);
  TEST("vil2_convert_grey_to_rgba", image6, true);

  image2 = vil2_plane(vil2_view_as_planes(image6),1);
  vil2_transform(vil2_plane(vil2_view_as_planes(image6),1), image2,
                 vcl_bind2nd(vcl_plus<vxl_byte>(),1));

  vil2_print_all(vcl_cout, image6);
  image7.clear();
  vil2_convert_rgb_to_grey(image6, image7);
  TEST("vil2_convert_rgb_to_grey(vil2_rgba)", image7, true);
  TEST_NEAR("Conversion rgba to grey", image7(0,0),  1.71540, 1e-5);
  TEST_NEAR("Conversion rgba to grey", image7(2,1), 35.71540, 1e-5);
  vil2_print_all(vcl_cout, image7);
}

#if 0 // commented out
void test_image_2d_byte_io()
{
  // -------- Test the binary I/O --------
  vil2_image_view<vxl_byte> image_out0;
  vil2_image_view<vxl_byte> image_out1;
  image_out0.set_nplanes(2);
  image_out0.resize(5,6);
  for (int i=0;i<2;++i)
    for (int y=0;y<6;++y)
      for (int x=0;x<5;++x)
        image_out0(x,y,i)=x+10*y+100*i;

  image_out1 = image_out0;

  vsl_b_ofstream bfs_out("test_image_view.bvl.tmp");
  TEST ("Created test_image_view.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image_out0);
  vsl_b_write(bfs_out, image_out1);
  bfs_out.close();

  vil2_image_view<vxl_byte> image_in0,image_in1;

  vsl_b_ifstream bfs_in("test_image_view.bvl.tmp");
  TEST ("Opened test_image_view.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, image_in0);
  vsl_b_read(bfs_in, image_in1);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("Binary IO", Equal(image_in0,image_out0),true);
  TEST("Binary IO (2)", Equal(image_in1,image_out1),true);

  // Check that image_in0 and 1 refer to same data block
  image_in0(3,4,1)=17;
  TEST("Data connected correctly",image_in1(3,4,1),17);
}
#endif // 0

MAIN( test_image_view )
{
  START( "vil2_image_view" );
  test_image_view_byte();
#if 0
  test_image_2d_byte_io();
#endif

  SUMMARY();
}
