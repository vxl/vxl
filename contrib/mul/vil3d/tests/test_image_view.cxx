// This is mul/vil3d/tests/test_image_view.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_functional.h>
#include <vxl_config.h> // for vxl_int_32
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_plane.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_crop.h>

bool Equal(const vil3d_image_view<vxl_int_32>& im0,
           const vil3d_image_view<vxl_int_32>& im1)
{
  return im0.nplanes()==im1.nplanes()
      && im0.ni() == im1.ni()
      && im0.nj() == im1.nj()
      && im0.nk() == im1.nk()
      && im0(0,0,0,0) == im1(0,0,0,0)
      && im0(1,1,1,1) == im1(1,1,1,1);
}

static void test_image_view_int()
{
  vcl_cout << "*******************************\n"
           << " Testing vil3d_image_view<int>\n"
           << "*******************************\n";

  vil3d_image_view<vxl_int_32> image0;
  image0.set_size(10,8,2);
  vcl_cout<<"image0: "<<image0<<vcl_endl;

  TEST("N.Planes",image0.nplanes(),1);
  TEST("set_size i",image0.ni(),10);
  TEST("set_size j",image0.nj(),8);
  TEST("set_size k",image0.nk(),2);

  for (unsigned int k=0;k<image0.nk();++k)
    for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j,k) = i+j*10+100*k;

  vil3d_print_all(vcl_cout, image0);

  {
    // Test the shallow copy
    vil3d_image_view<vxl_int_32> image1;
    image1 = image0;

    vcl_cout<<"Shallow copy: "<<image1<<vcl_endl;

    TEST("Shallow copy (size)",
         image0.ni()==image1.ni() &&
         image0.nj()==image1.nj() &&
         image0.nk()==image1.nk() &&
         image0.nplanes()==image1.nplanes(), true);

    image0(4,6,1)=127;
    TEST("Shallow copy (values)",image1(4,6,1),image0(4,6,1));
  }


  {
    // Test the shallow copy by smart pointer
    vil3d_image_view<vxl_int_32> *image_ptr = new vil3d_image_view<vxl_int_32>;
    image_ptr->deep_copy(image0);

    vil3d_image_view_base_sptr image_sptr = image_ptr;
    vil3d_image_view<vxl_int_32> image1;
    image1 = image_sptr;

    vcl_cout<<"Shallow copy by smart pointer: "<<image1<<vcl_endl;

    TEST("Shallow copy (size)",
         image0.ni()==image1.ni() &&
         image0.nj()==image1.nj() &&
         image0.nk()==image1.nk() &&
         image0.nplanes()==image1.nplanes(), true);

    image0(4,6,1)=127;
    TEST("Shallow copy (values)",image1(4,6,1),image0(4,6,1));
  }


   vil3d_image_view<vxl_int_32> image2;
  {
    // Check data remains valid if a copy taken
    vil3d_image_view<vxl_int_32> image3;
    image3.set_size(4,5,3,2);
    image3.fill(111);
    image2 = image3;
  }

  TEST("Shallow copy 2",image2.ni()==4
       && image2.nj()==5 && image2.nk()==3 && image2.nplanes()==2, true);

  image2(1,1,1)=17;
  TEST("Data still in scope",image2(3,3,0),111);
  TEST("Data still in scope",image2(1,1,1),17);

  vcl_cout<<image2<<vcl_endl;

  {
    // Test the deep copy
    vil3d_image_view<vxl_int_32> image4;
    image4.deep_copy(image0);
    TEST("Deep copy (size)",image0.ni()==image4.ni() &&
                            image0.nj()==image4.nj() &&
                            image0.nk()==image4.nk() &&
                            image0.nplanes()==image4.nplanes(), true);
    TEST("Deep copy (values)",image4(4,6,0),image0(4,6,0));

    vxl_int_32 v46 = image0(4,6,0);
    image0(4,6,0)=255;
    TEST("Deep copy (values really separate)",image4(4,6,0),v46);
  }

  vil3d_image_view<vxl_int_32> image_win;
  image_win = vil3d_crop(image0,2,4,3,4,0,1);
  TEST("vil3d_crop size",
       image_win.ni()==4 && image_win.nj()==4 &&
       image_win.nk()==1 && image_win.nplanes()==image0.nplanes(),
       true);

  image0(2,3,0)=222;
  TEST("vil3d_crop is shallow copy",image_win(0,0,0),222);

  vcl_cout<<image0.is_a()<<vcl_endl;
  TEST("is_a() specialisation for vxl_int_32",image0.is_a(),"vil3d_image_view<vxl_int_32>");

  vil3d_image_view<vxl_int_32> image5(7,8,9,3);
  image5.fill(14);
  image5(1,2,3,1)=123;
  vil3d_image_view<vxl_int_32> image_plane = vil3d_plane(image5,1);
  TEST("Plane size",image_plane.ni()==image5.ni() &&
                    image_plane.nj()==image5.nj() &&
                    image_plane.nk()==image5.nk() &&
                    image_plane.nplanes()==1, true);
  TEST("Plane view",image_plane(1,2,3),image5(1,2,3,1));
  TEST("Plane view",image_plane(0,0,0),image5(0,0,0,0));

  vil_image_view<vxl_int_32> image_ij = vil3d_slice_ij(image5,3);
  TEST("slice_ij size",image_ij.ni()==image5.ni() &&
                       image_ij.nj()==image5.nj() &&
                       image_ij.nplanes()==image5.nplanes(), true);
  TEST("slice_ij value",image_ij(1,2,1),123);

  vil_image_view<vxl_int_32> image_ji = vil3d_slice_ji(image5,3);
  TEST("slice_ji size",image_ji.nj()==image5.ni() &&
                       image_ji.ni()==image5.nj() &&
                       image_ji.nplanes()==image5.nplanes(), true);
  TEST("slice_ji value",image_ji(2,1,1),123);

  vil_image_view<vxl_int_32> image_ik = vil3d_slice_ik(image5,2);
  TEST("image_ik size",image_ik.ni()==image5.ni() &&
                       image_ik.nj()==image5.nk() &&
                       image_ik.nplanes()==image5.nplanes(), true);
  TEST("image_ik value",image_ik(1,3,1),123);

  vil_image_view<vxl_int_32> image_ki = vil3d_slice_ki(image5,2);
  TEST("image_ki size",image_ki.ni()==image5.nk() &&
                       image_ki.nj()==image5.ni() &&
                       image_ki.nplanes()==image5.nplanes(), true);
  TEST("image_ki value",image_ki(3,1,1),123);

  vil_image_view<vxl_int_32> image_jk = vil3d_slice_jk(image5,1);
  TEST("image_jk size",image_jk.ni()==image5.nj() &&
                       image_jk.nj()==image5.nk() &&
                       image_jk.nplanes()==image5.nplanes(), true);
  TEST("image_jk value",image_jk(2,3,1),123);

  vil_image_view<vxl_int_32> image_kj = vil3d_slice_kj(image5,1);
  TEST("image_kj size",image_kj.ni()==image5.nk() &&
                       image_kj.nj()==image5.nj() &&
                       image_kj.nplanes()==image5.nplanes(), true);
  TEST("image_kj value",image_kj(3,2,1),123);


#if 0
  vcl_cout << "************************************\n"
           << " Testing vil3d_image_view functions\n"
           << "************************************\n";

  image2.fill(0);
  image_win = vil_crop(image2,2,1,1,2);
  image5.set_size(1,2);
  image5(0,0) = vil_rgb<vxl_int_32>(25,35,45);
  image5(0,1) = vil_rgb<vxl_int_32>(25,35,45);
  image0 = image5;

  vil_copy_reformat(image0, image_win);
  vil_print_all(vcl_cout, image2);
  vil3d_image_view<vxl_int_32> test_image(5,4,3);
  test_image.fill(0);
  test_image(2,1,0) = test_image(2,2,0) = 25;
  test_image(2,1,1) = test_image(2,2,1) = 35;
  test_image(2,1,2) = test_image(2,2,2) = 45;
  TEST("vil_reformat_copy, vil_window and vil_deep_equality",
       vil3d_image_view_deep_equality(test_image,image2), true);
  test_image(2,2,2) = 44;
  TEST("!vil_deep_equality", vil3d_image_view_deep_equality(test_image,image2), false);
  test_image.set_size(5,4,4);
  TEST("!vil_deep_equality", vil3d_image_view_deep_equality(test_image,image2), false);
  vil_print_all(vcl_cout, image2);

  vil3d_image_view<float> image7;
  vil_convert_cast(image2, image7);
  vil_print_all(vcl_cout, image7);
  vil_transform(image7, image7, vcl_bind2nd(vcl_plus<float>(),0.6f));
  vil_convert_cast(image7, image2);
  vil_print_all(vcl_cout, image2);
  TEST("Rounding up", image2(0,0,0) == 1 && image2(2,2,1) == 36, true);

  image7.clear();
  vil_convert_rgb_to_grey(vil_view_as_rgb(image2), image7);
  vil_print_all(vcl_cout, image7);
  TEST("vil_convert_rgb_to_grey(vil_rgba)", image7, true);
  TEST_NEAR("Conversion rgb to grey", image7(0,0), 1.0, 1e-5);
  TEST_NEAR("Conversion rgb to grey", image7(2,1), 34.5960, 1e-5);
  vil_convert_grey_to_rgb(image7, image5);
  TEST("vil_convert_grey_to_rgb", image5, true);
  vil_print_all(vcl_cout, image5);

  vil_convert_grey_to_rgba(image7, image6);
  TEST("vil_convert_grey_to_rgba", image6, true);

  image2 = vil_plane(vil_view_as_planes(image6),1);
  vil_transform(vil_plane(vil_view_as_planes(image6),1), image2,
                 vcl_bind2nd(vcl_plus<vxl_int_32>(),1));

  vil_print_all(vcl_cout, image6);
  image7.clear();
  vil_convert_rgb_to_grey(image6, image7);
  TEST("vil_convert_rgb_to_grey(vil_rgba)", image7, true);
  TEST_NEAR("Conversion rgba to grey", image7(0,0),  1.71540, 1e-5);
  TEST_NEAR("Conversion rgba to grey", image7(2,1), 35.71540, 1e-5);
  vil_print_all(vcl_cout, image7);
#endif // 0
}

static void test_image_view_is_a()
{
  vil3d_image_view<vxl_int_16> image0;
  TEST("is_a() specialisation for vxl_int_16",image0.is_a(),"vil3d_image_view<vxl_int_16>");
  vil3d_image_view<vxl_int_32> image1;
  TEST("is_a() specialisation for vxl_int_32",image1.is_a(),"vil3d_image_view<vxl_int_32>");
  vil3d_image_view<vxl_byte> image2;
  TEST("is_a() specialisation for vxl_byte",image2.is_a(),"vil3d_image_view<vxl_byte>");
  vil3d_image_view<float> image3;
  TEST("is_a() specialisation for float",image3.is_a(),"vil3d_image_view<float>");
  vil3d_image_view<double> image4;
  TEST("is_a() specialisation for double",image4.is_a(),"vil3d_image_view<double>");
}

static void test_image_view()
{
  test_image_view_int();
  test_image_view_is_a();
}

TESTMAIN(test_image_view);
