// This is mul/vil2/tests/test_convert.cxx
#include <vil2/vil2_convert.h>
#include <vil2/vil2_image_view.h>
#include <vxl_config.h> // for vxl_byte
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil2/vil2_print.h>
#include <vil2/vil2_load.h>


static
void test_convert1(const char * golden_data_dir)
{
  vcl_cout << "*********************************************\n"
           << " Testing vil2_convert*(vil2_image_view<T>..)\n"
           << "*********************************************\n";

  vcl_string datadir = golden_data_dir;
  if (*golden_data_dir) datadir += "/";

  vil2_image_view<vxl_byte> image1 = vil2_convert_to_grey_using_rgb_weighting(
    vil2_load((datadir + "ff_grey8bit_raw.pgm").c_str()), vxl_byte());
  TEST("vil2_convert_to_grey_using_rgb_weighting(vil2_load(grey_image))", image1, true);

  vil2_print_all(vcl_cout, image1);

  vil2_image_view<float> image2 = vil2_convert_to_grey_using_average(
    vil2_load((datadir + "ff_rgb8bit_ascii.ppm").c_str()), float());
  TEST("vil2_convert_to_grey_using_average(vil2_load(rgb_image))", image2, true);

  vil2_print_all(vcl_cout, image2);
}

static
void test_convert_diff_types(const char * golden_data_dir)
{
  vcl_cout << "**************************************************************\n"
           << " Testing vil2_convert_cast(vil2_image_base,vil2_image_view<T>)\n"
           << "**************************************************************\n";

  vcl_string datadir = golden_data_dir;
  if (*golden_data_dir) datadir += "/";

  testlib_test_begin( "Loading images" );
  vil2_image_view<vxl_byte> image1 = vil2_load((datadir + "ff_grey8bit_raw.pgm").c_str());
  vil2_image_view_base_sptr image_base1 = vil2_load((datadir + "ff_grey8bit_raw.pgm").c_str());
  testlib_test_perform( image1 && image_base1 );

  testlib_test_begin( "Converting explicitly 8bit to 16bit" );
  vil2_image_view<vxl_uint_16> image_16_1;
  vil2_convert_cast( image1, image_16_1 );
  testlib_test_perform( image_16_1 && image_16_1(4,0) == vxl_uint_16( image1(4,0) ) );

  testlib_test_begin( "Converting implicitly 8bit to 16bit" );
  vil2_image_view<vxl_uint_16> image_16_2;
  vil2_convert_cast( image_base1, image_16_2 );
  testlib_test_perform( image_16_2 && image_16_2(4,0) == vxl_uint_16( image1(4,0) ) );

  if( image_16_2 ) {
    vil2_print_all(vcl_cout, image_16_2);
  } else {
    vcl_cout << "(no dump)\n";
  }
}

static
void test_convert_stretch_range()
{
  vcl_cout<<"testing vil2_convert_stretch_range(src,dest):"<<vcl_endl;
  vil2_image_view<float> f_image(10,10);
  for (unsigned j=0;j<f_image.nj();++j)
    for (unsigned i=0;i<f_image.ni();++i)  f_image(i,j)=0.1f*i+0.01f*j+5.f;

  vil2_print_all(vcl_cout, f_image) ;

  vil2_image_view<vxl_byte> b_image;
  vil2_convert_stretch_range(f_image,b_image);
  TEST("Width",b_image.ni(),f_image.ni());
  TEST("Height",b_image.nj(),f_image.nj());
  vxl_byte min_b,max_b;
  vil2_math_value_range(b_image,min_b,max_b);
  TEST("Min. value",min_b,0);
  TEST("Max. value",max_b,255);
  TEST("b_image(5,5)",b_image(5,5),vxl_byte( (5.55-5.0)*255/0.99 ) );
}

MAIN( test_convert )
{
  START( "vil2_convert" );

  test_convert1(argc>1 ? argv[1] : "file_read_data");
  test_convert_stretch_range();
  test_convert_diff_types(argc>1 ? argv[1] : "file_read_data");

  SUMMARY();
}
