// This is mul/vil2/tests/test_image_view.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_functional.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_crop.h>
#include <vil2/vil2_copy.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_plane.h>
#include <vil2/vil2_convert.h>
#include <vil2/vil2_view_as.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_new.h>

template <class S, class T>
void test_image_view_rgba(vil2_image_view<S> &image2, vil2_image_view<T> &image7)
{
// do nothing in general case
}

VCL_DEFINE_SPECIALIZATION
void test_image_view_rgba(vil2_image_view<vxl_byte> &image2, vil2_image_view<float> &image7)
{
  image2.set_size(10,10,2);
  vil2_image_view<vil_rgba<vxl_byte> > image6 = image2;
  TEST("Can't assign a 3 plane images to rgba view", image6, false);

  vil2_convert_cast(image7, image6);
  TEST("vil2_convert_cast<float, rgba<byte> >", image6, true);

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

VCL_DEFINE_SPECIALIZATION
void test_image_view_rgba(vil2_image_view<float> &image2, vil2_image_view<double> &image7)
{
  image2.set_size(10,10,2);
  vil2_image_view<vil_rgba<float> > image6 = image2;
  TEST("Can't assign a 3 plane images to rgba view", image6, false);

  vil2_convert_cast(image7, image6);
  TEST("vil2_convert_cast<double,rgba<float> >", image6, true);

  image2 = vil2_plane(vil2_view_as_planes(image6),1);
  vil2_transform(vil2_plane(vil2_view_as_planes(image6),1), image2,
                 vcl_bind2nd(vcl_plus<float>(),1));

  vil2_print_all(vcl_cout, image6);
  image7.clear();
  vil2_convert_rgb_to_grey(image6, image7);
  vil2_print_all(vcl_cout, image7);
  TEST("vil2_convert_rgb_to_grey(vil2_rgba)", image7, true);

  TEST_NEAR("Conversion rgba to grey wibble", image7(0,0),  1.3154, 1e-5);

  TEST_NEAR("Conversion rgba to grey", image7(2,1), 34.9114, 1e-5);

}


template <class S, class T>
void test_image_view(S d1, vcl_string s_name, T d2)
{

  vil2_image_view<S> image0;
  image0.set_size(10,8);
  vcl_cout<<"image0: "<<image0<<vcl_endl;

  TEST("N.Planes",image0.nplanes(),1);
  TEST("set_size i",image0.ni(),10);
  TEST("set_size j",image0.nj(),8);

  for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j) = i+j;

  vil2_print_all(vcl_cout, image0);

  {
    // Test the shallow copy
    vil2_image_view<S> image1;
    image1 = image0;

    TEST("Shallow copy (size)",
         image0.ni()==image1.ni() &&
         image0.nj()==image1.nj() &&
         image0.nplanes()==image1.nplanes(), true);

    image0(4,6)=127;
    TEST("Shallow copy (values)",image1(4,6),image0(4,6));
  }


  vil2_image_view<S> image2;
  {
    // Check data remains valid if a copy taken
    vil2_image_view<S> image3;
    image3.set_size(4,5,3);
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
    vil2_image_view<S> image4;
    image4.deep_copy(image0);
    TEST("Deep copy (size)",image0.ni()==image4.ni()
                         && image0.nj()==image4.nj()
                         && image0.nplanes()==image4.nplanes(), true);
    TEST("Deep copy (values)",image4(4,6),image0(4,6));

    S v46 = image0(4,6);
    image0(4,6)=255;
    TEST("Deep copy (values really separate)",image4(4,6),v46);
  }

  vil2_image_view<S> image_win;
  image_win = vil2_crop(image0,2,4,3,4);
  TEST("vil2_crop size",
       image_win.ni()==4 && image_win.nj()==4 && image_win.nplanes()==image0.nplanes(),
       true);

  image0(2,3)=222;
  TEST("vil2_crop is shallow copy",image_win(0,0),222);

  vcl_cout<<image0.is_a()<<vcl_endl;
  TEST("is_a() specialisation for S",image0.is_a(),"vil2_image_view<"+s_name+">");

  vil2_image_view<vil_rgb<S> > image5;
  image5.set_size(5,4);
  image5.fill(vil_rgb<S>(25,35,45));
  image5(2,2).b = 50;

  image2 = image5;
  TEST("Can assign rgb images to 3 plane view", image2, true);
  TEST("Pixels are correct", image2(2,2,1) == 35 && image2(2,2,2) == 50, true);

  image5 = image2;
  TEST("Can assign 3 planes suitable image to rgb view", image5, true);


  TEST("Equality", image2, image2);
  TEST("comparison", image2 < image2, false);
  image0 = image2;
  TEST("Equality", image2, image0);
  TEST("comparison", image2 < image0, false);

  vil2_image_view<T> image7;
  TEST("Equality", image2 == image7, false);
  TEST("Comparison", image2 < image7, true);
  TEST("Comparison", image2 > image7, false);

  vcl_cout << "***********************************\n"
           << " Testing vil2_image_view functions\n"
           << "***********************************\n";

  image2.fill(0);
  image_win = vil2_crop(image2,2,1,1,2);
  TEST("Equality", image2 == image_win, false);
  TEST("Comparison", image_win < image2, true);
  TEST("Comparison", image_win > image2, false);
  image5.set_size(1,2);
  image5(0,0) = vil_rgb<S>(25,35,45);
  image5(0,1) = vil_rgb<S>(25,35,45);
  image0 = image5;

  vil2_copy_reformat(image0, image_win);
  vil2_print_all(vcl_cout, image2);
  vil2_image_view<S> test_image(5,4,3);
  test_image.fill(0);
  test_image(2,1,0) = test_image(2,2,0) = 25;
  test_image(2,1,1) = test_image(2,2,1) = 35;
  test_image(2,1,2) = test_image(2,2,2) = 45;
  TEST("vil2_reformat_copy, vil2_crop and vil2_deep_equality",
       vil2_image_view_deep_equality(test_image,image2), true);
  test_image(2,2,2) = 44;
  TEST("!vil2_deep_equality", vil2_image_view_deep_equality(test_image,image2), false);
  test_image.set_size(5,4,4);
  TEST("!vil2_deep_equality", vil2_image_view_deep_equality(test_image,image2), false);
  vil2_print_all(vcl_cout, image2);

  vil2_convert_cast(image2, image7);
  vil2_print_all(vcl_cout, image7);
  vil2_transform(image7, image7, vcl_bind2nd(vcl_plus<T>(),0.6f));
  vil2_convert_cast(image7, image2);
  vil2_print_all(vcl_cout, image2);

  const vil2_pixel_format format = vil2_pixel_format_of(S());
  if (format == VIL2_PIXEL_FORMAT_FLOAT || format == VIL2_PIXEL_FORMAT_DOUBLE)
  {
    TEST_NEAR("Rounding ", image2(0,0,0), 0.6, 1e-5);
    TEST_NEAR("Rounding ", image2(2,2,1), 35.6, 1e-5);
  }
  else
    TEST("Rounding ", image2(0,0,0) == 1 && image2(2,2,1) == 36, true);

  image7.clear();
  vil2_convert_rgb_to_grey(vil2_view_as_rgb(image2), image7);
  vil2_print_all(vcl_cout, image7);
  TEST("vil2_convert_rgb_to_grey(vil2_rgb)", image7, true);

  if (format == VIL2_PIXEL_FORMAT_FLOAT || format == VIL2_PIXEL_FORMAT_DOUBLE)
    TEST_NEAR("Conversion rgb to grey", image7(0,0), 0.6, 1e-5);
  else
    TEST_NEAR("Conversion rgb to grey", image7(0,0), 1.0, 1e-5);
  if (format == VIL2_PIXEL_FORMAT_FLOAT || format == VIL2_PIXEL_FORMAT_DOUBLE)
    TEST_NEAR("Conversion rgb to grey", image7(2,1), 34.196, 1e-5);
  else
    TEST_NEAR("Conversion rgb to grey", image7(2,1), 34.5960, 1e-5);

  vil2_convert_cast(image7, image5);
  TEST("vil2_convert_cast<T,S>", image5, true);
  vil2_print_all(vcl_cout, image5);

  // Only test rgba conversion for float and byte.
  // other components types don't have rgba instantiated.
  test_image_view_rgba(image2, image7);
}

static
void test_contiguous()
{
  vil2_image_view<vxl_byte> im1( 4, 5, 6 );
  TEST( "internal memory: contiguous", im1.is_contiguous(), true );

  vxl_byte memory[ 3*5*7 ];

  for( unsigned d1 = 0; d1 < 3; ++d1 ) {
    for( unsigned d2 = 0; d2 < 3; ++d2 ) {
      if( d2==d1 ) continue;
      for( unsigned d3 = 0; d3 < 3; ++d3 ) {
        if( d3==d1 || d3==d2 ) continue;
        vcl_ptrdiff_t step[3];
        step[d1] = 1;
        step[d2] = 3;
        step[d3] = 15;
        vcl_ostringstream str;
        str << "external memory: " << step[0] << "x"<<step[1]<<"x"<<step[2]<<" step contiguous";
        vil2_image_view<vxl_byte> im( memory, 3, 5, 7, step[d1], step[d2], step[d3] );
        TEST( str.str().c_str(), im.is_contiguous(), true );
      }
    }
  }

  for( unsigned d1 = 0; d1 < 3; ++d1 ) {
    for( unsigned d2 = 0; d2 < 3; ++d2 ) {
      if( d2==d1 ) continue;
      for( unsigned d3 = 0; d3 < 3; ++d3 ) {
        if( d3==d1 || d3==d2 ) continue;
        vcl_ptrdiff_t step[3];
        step[d1] = 2;
        step[d2] = 3;
        step[d3] = 15;
        vcl_ostringstream str;
        str << "external memory: " << step[0] << "x"<<step[1]<<"x"<<step[2]<<" step not contiguous";
        vil2_image_view<vxl_byte> im( memory, 3, 5, 7, step[d1], step[d2], step[d3] );
        TEST( str.str().c_str(), im.is_contiguous(), false );
      }
    }
  }
  vil2_image_view<float> im2 = vil2_new_image_view_plane_i_j(4, 5, 6, float());
  TEST( "vil2_new_image_view_plane_i_j is_continuous", im2.is_contiguous(), true );
  vil2_image_view<double> im3 = vil2_new_image_view_j_i_plane(4, 5, 6, double());
  TEST( "vil2_new_image_view_i_j_plane is_continuous", im2.is_contiguous(), true);
  
}


MAIN( test_image_view )
{
  START( "vil2_image_view" );
  vcl_cout << "*****************************************\n"
           << " Testing vil2_image_view<byte and float>\n"
           << "*****************************************\n";
  test_image_view(vxl_byte(), "vxl_byte", float());
  vcl_cout << "*****************************************\n"
           << " Testing vil2_image_view<float and double>\n"
           << "*****************************************\n";
  test_image_view(float(), "float", double());
  vcl_cout << "*****************************************\n"
           << " Testing vil2_image_view<int_16 and double>\n"
           << "*****************************************\n";
  test_image_view(vxl_int_16(), "vxl_int_16", float());
  vcl_cout << "*****************************************\n"
           << " Testing vil2_image_view<uint_32 and double>\n"
           << "*****************************************\n";
  test_image_view(vxl_uint_32(), "vxl_uint_32", float());
  test_contiguous();


  SUMMARY();
}
