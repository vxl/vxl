// This is core/vil/tests/test_convert.cxx
#define VXL_LEGACY_ERROR_REPORTING // REQUIRED FOR PASSING TESTS 2018-11-02
#include <iostream>
#include <vxl_config.h> // for vxl_byte
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <vil/vil_exception.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
static void test_convert1(const char * golden_data_dir)
{
  std::cout << "*******************************************\n"
           << " Testing vil_convert*(vil_image_view<T>..)\n"
           << "*******************************************\n";

 std::string datadir = golden_data_dir;
 if (*golden_data_dir) datadir += "/";
  vil_image_view<vxl_byte> image1 = vil_convert_to_grey_using_rgb_weighting(
    vil_load((datadir + "ff_grey8bit_raw.pgm").c_str()));
  TEST("vil_convert_to_grey_using_rgb_weighting(vil_load(grey_image))", image1?true:false, true);

  vil_print_all(std::cout, image1);

  vil_image_view<vxl_byte> image2 = vil_convert_to_grey_using_average(
    vil_load((datadir + "ff_rgb8bit_ascii.ppm").c_str()));
  TEST("vil_convert_to_grey_using_average(vil_load(rgb_image))", image2?true:false, true);

  vil_print_all(std::cout, image2);
}

static void test_convert_diff_types(const char * golden_data_dir)
{
  std::cout << "******************************************************\n"
           << " Testing vil_convert_cast(T,vil_image_view_base_sptr)\n"
           << "******************************************************\n";

  std::string datadir = golden_data_dir;
  if (*golden_data_dir) datadir += "/";

  vil_image_view<vxl_byte> image1 = vil_load((datadir + "ff_grey8bit_raw.pgm").c_str());
  vil_image_view_base_sptr image_base1 = vil_load((datadir + "ff_grey8bit_raw.pgm").c_str());
  vil_image_view_base_sptr image2 = vil_load((datadir + "ff_grey16bit_raw.pgm").c_str());
  vil_image_view_base_sptr image3 = vil_load((datadir + "ff_rgb8bit_raw.ppm").c_str());
  vil_image_view_base_sptr image4 = vil_load((datadir + "ff_rgb16bit_raw.ppm").c_str());
  TEST("Loading images" , image1 && image_base1, true);

  vil_image_view<vxl_uint_16> image_16_1;
  vil_convert_cast( image1, image_16_1 );
  TEST("Converting explicitly 8bit grey to 16bit grey", image_16_1(3,0), vxl_uint_16(image1(3,0)));

  vil_print_all(std::cout, image1);
  if ( image_16_1 )
    vil_print_all(std::cout, image_16_1);
  else
    std::cout << "(no dump)\n";

  vil_image_view<vxl_uint_16> image_16_2 = vil_convert_cast(vxl_uint_16(), image_base1 );
  TEST("Converting implicitly 8bit grey to 16bit grey", image_16_2(4,2), vxl_uint_16(image1(4,2)));

  vil_print_all(std::cout, image_base1);
  if ( image_16_2 )
    vil_print_all(std::cout, image_16_2);
  else
    std::cout << "(no dump)\n";

  vil_image_view<vxl_byte> image_8_2 = vil_convert_cast( vxl_byte(), image2 );
  vil_image_view<vxl_uint_16> image_2 = image2;
  TEST("Converting implicitly 16bit grey to 8bit grey", image_8_2(4,2), vxl_byte(image_2(4,2)));

  vil_print_all(std::cout, image2);
  if ( image_8_2 )
    vil_print_all(std::cout, image_8_2);
  else
    std::cout << "(no dump)\n";

  vil_image_view<vxl_byte> image_3 = image3;
  vil_image_view<vxl_byte> image_8_3;
  vil_convert_planes_to_grey( image_3, image_8_3 );
  TEST("Converting explicitly 8bit RGB to 8bit grey", image_8_3(1,0), image_3(1,0,1)); // accidentally a grey pixel ...

  vil_print_all(std::cout, image3);
  if ( image_8_3 )
    vil_print_all(std::cout, image_8_3);
  else
    std::cout << "(no dump)\n";

  vil_image_view<vxl_byte> image_8_4 = vil_convert_cast( vxl_byte(), image4 );
  vil_image_view<vxl_uint_16> image_4 = image4;
  TEST("Converting implicitly 16bit RGB to 8bit grey", image_8_4(1,0), vxl_byte(image_4(1,0,1)));

  vil_print_all(std::cout, image4);
  if ( image_8_4 )
    vil_print_all(std::cout, image_8_4);
  else
    std::cout << "(no dump)\n";
}

static void test_convert_stretch_range()
{
  std::cout<<"testing vil_convert_stretch_range(src,dest):\n";
  vil_image_view<float> f_image(10,10);
  for (unsigned j=0;j<f_image.nj();++j)
    for (unsigned i=0;i<f_image.ni();++i)  f_image(i,j)=0.1f*i+0.01f*j+5.f;

//  vil_print_all(std::cout, f_image) ;

  vil_image_view<vxl_byte> b_image;
  vil_convert_stretch_range(f_image,b_image);
  TEST("Width",b_image.ni(),f_image.ni());
  TEST("Height",b_image.nj(),f_image.nj());
  vxl_byte min_b,max_b;
  vil_math_value_range(b_image,min_b,max_b);
  TEST("Min. value",min_b,0);
  TEST("Max. value",max_b,255);
  TEST("b_image(5,5)",b_image(5,5),vxl_byte( (5.55-5.0)*255/0.99 ) );
}

static void test_convert_stretch_range_limited()
{
  std::cout<<"testing test_convert_stretch_range_limited(src,dest):\n";
  vil_image_view<float> f_image(10, 10);
  for (unsigned j=0; j<f_image.nj(); ++j)
    for (unsigned i=0; i<f_image.ni(); ++i) f_image(i,j)=0.1f*i + 0.01f*j + 5.f;
#if 0
  float min_f, max_f;
  vil_math_value_range(f_image, min_f, max_f );
  std::cout << "Min f value: " << min_f << '\n'
           << "Max f value: " << max_f << '\n';
  vil_print_all(std::cout, f_image) ;
#endif // 0

  float slo=5.2f, shi=5.8f;
  vxl_byte dlo=50, dhi=200;
  vil_image_view<vxl_byte> b_image;
  vil_convert_stretch_range_limited(f_image, b_image, slo, shi, dlo, dhi);
  TEST("Width", b_image.ni(), f_image.ni());
  TEST("Height", b_image.nj(), f_image.nj());
  vxl_byte min_b, max_b;
  vil_math_value_range(b_image, min_b, max_b);
  TEST("Min. value", min_b, dlo);
  TEST("Max. value", max_b, dhi);
#ifdef DEBUG
  vil_print_all(std::cout, b_image) ;
#endif // DEBUG

  float f55 = f_image(5,5);
  auto b55 = vxl_byte(dlo + (f55-slo)*(dhi-dlo)/(shi-slo) + 0.5);
#if 0
  std::cout << "f55= " << f55 << '\n'
           << "b55= " << (int)b55 << '\n'
           << "b_image(5,5)" << (int)b_image(5,5) << '\n';
#endif // 0
  TEST("b_image(5,5)", b_image(5,5), b55);
}

static void test_convert_to_n_planes()
{
  constexpr unsigned n = 10;
  std::cout<<"testing test_convert_to_n_planes(src,dest):\n";
  vil_image_view<float> f_image(n,n,2);
  vil_image_view<float> f_image_expected(n,n,3);
  vil_image_view<vxl_uint_16> u16_image_expected(n,n,3);
  for (unsigned j=0;j<f_image.nj();++j)
    for (unsigned i=0;i<f_image.ni();++i)
    {
      f_image(i,j,0)=1.0f*i+10.0f*j+5.0f;
      f_image(i,j,1)=1.0f*i+10.0f*j;
      f_image_expected(i,j,0)= i + 10.0f*j + 5.0f;
      f_image_expected(i,j,1)= i + 10.0f*j;
      f_image_expected(i,j,2)= i + 10.0f*j + 5.0f;
      u16_image_expected(i,j,0)= static_cast<vxl_uint_16>(i + 10*j + 5);
      u16_image_expected(i,j,1)= static_cast<vxl_uint_16>(i + 10*j);
      u16_image_expected(i,j,2)= static_cast<vxl_uint_16>(i + 10*j + 5);
    }

#ifdef DEBUG
  vil_print_all(std::cout, f_image);
#endif // DEBUG

  vil_image_view_base_sptr f_image_ref = new vil_image_view<float>(f_image);

  vil_image_view<float> f_image_dest =
    vil_convert_to_n_planes(3, f_image_ref);

  TEST("Image as expected",vil_image_view_deep_equality(f_image_dest,f_image_expected), true);

  vil_math_scale_and_offset_values(f_image,1.0f,0.499f);

  vil_image_view_base_sptr f_image_dest_sptr(new vil_image_view<float>(f_image_dest));
  vil_image_view<vxl_uint_16> image_16_3 = vil_convert_round(vxl_uint_16(), f_image_dest_sptr);
  TEST("implicit vil_convert_round float to 16bit with rounding", vil_image_view_deep_equality(image_16_3, u16_image_expected), true);

  vil_image_view<vxl_uint_16> image_16_3_stretched = vil_convert_stretch_range(vxl_uint_16(), f_image_ref);
  vxl_uint_16 minp,maxp;
  vil_math_value_range(image_16_3_stretched,minp,maxp);
  TEST("implicit vil_convert_stretch_range float to 16bit with rounding", minp==0 && maxp==65535, true);

#if !defined VXL_LEGACY_ERROR_REPORTING
  bool caught_exception = false;
  try
  {
    vil_image_view<vil_rgb<float> >(f_image_dest_sptr)?true:false;
  }
  catch (const vil_exception_pixel_formats_incompatible &e)
  {
    caught_exception = true;
    std::cout << "Exception: " << e.what() << std::endl;
  }
  TEST("Plane image cannot be directly converted to components", caught_exception, true);
  caught_exception = false;
  vil_image_view<vil_rgb<float> > rgb_image;
  try
  {
    rgb_image = vil_convert_to_component_order(f_image_dest_sptr);
  }
  catch (const vil_exception_pixel_formats_incompatible &e)
  {
    caught_exception = true;
    std::cout << "Exception: " << e.what() << std::endl;
  }
  TEST("implict vil_convert_to_component_order API", caught_exception, false);
#else
  TEST("Plane image cannot be directly converted to components",
       vil_image_view<vil_rgb<float> >(f_image_dest_sptr)?true:false, false);
  vil_image_view<vil_rgb<float> > rgb_image =
    vil_convert_to_component_order(f_image_dest_sptr);
  TEST("implict vil_convert_to_component_order API", rgb_image?true:false, true);
#endif

  TEST("implict vil_convert_to_component_order correct",
       vil_image_view_deep_equality(vil_image_view<float>(rgb_image), f_image_dest),
       true);
#ifdef DEBUG
  vil_print_all(std::cout, image_16_3_stretched);
  vil_print_all(std::cout, image_16_3);
  vil_print_all(std::cout, u16_image_expected);
#endif // DEBUG
}

static void test_simple_pixel_conversions()
{
  {
    vil_convert_round_pixel<float, int> op;
    int out;
    op(5.5f, out);
    std::cout << out << std::endl;
    TEST("round_pixel float->int", out, 6);
    op(5.4f, out);
    std::cout << out << std::endl;
    TEST("round_pixel float->int", out, 5);
  }
  {
    vil_convert_round_pixel<double, unsigned short> op;
    unsigned short out;
    op(5.5f, out);
    std::cout << out << std::endl;
    TEST("round_pixel double->ushort", out, 6);
    op(5.4f, out);
    std::cout << out << std::endl;
    TEST("round_pixel double->ushort", out, 5);
  }
}


static void test_convert(int argc, char* argv[])
{
  std::string path;
  if(argc>1){
    path = argv[1];
  }else{
    std::string root = testlib_root_dir();
    path = root + "/core/vil/tests/file_read_data";
  }
  bool exists = vul_file::is_directory(path);
  if(exists){
    path += "/";
    test_convert1(path.c_str());
    test_convert_diff_types(path.c_str());
  }else{
    TEST("test data exists", false, true);
    std::cout << "Failed path: " << path << '\n';
  }
  test_convert_to_n_planes();
 // test data path is not passed into argv - JLM
 // test_convert1(argc>1 ? argv[1] : "file_read_data");
  test_convert_stretch_range();
  test_convert_stretch_range_limited();
 // test data path is not passed into argv - JLM
 // test_convert_diff_types(argc>1 ? argv[1] : "file_read_data");
  test_simple_pixel_conversions();
}

TESTMAIN_ARGS(test_convert);
