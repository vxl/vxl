// This is mul/vil2/tests/test_vil1.cxx
#include <vil2/vil2_vil1.h>
#include <vxl_config.h> // for vxl_byte
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil2/vil2_print.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_load.h>
#include <vil2/vil2_load.h>


MAIN( test_vil1 )
{
vcl_string image_base;
  if ( argc >= 2 ) {
    image_base = argv[1];
#ifdef VCL_WIN32
    image_base += "\\";
#else
    image_base += "/";
#endif
  }

  START( "vil2_vil1t" );

  vcl_cout << "*************************\n"
           << " Testing vil2_vil1_*(..)\n"
           << "*************************\n";


  vil2_image_view<vxl_byte> image0(10,8);

  for (unsigned int j=0;j<image0.nj();++j)
   for (unsigned int i=0;i<image0.ni();++i)
     image0(i,j) = i+j;

  vil_memory_image_of<vxl_byte> vil1_mem = vil2_vil1_from_image_view(image0);

  TEST ("vil2_vil1_from_image_view sizes", vil1_mem.width() == int(image0.ni()) &&
    vil1_mem.height() == int(image0.nj()), true);

  bool bad_pixels = false;
  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      if (vil1_mem(i,j) != i+j) bad_pixels = true;

  TEST ("vil2_vil1_from_image_view pixels", bad_pixels, false);

  vil2_image_view<vxl_byte> image1 = vil2_vil1_to_image_view(vil1_mem);

  TEST ("vil2_vil1_to_image_view ", vil2_image_view_deep_equality(image0, image1), true);


  vil_image tiff_image = vil_load( (image_base+"ff_rgb8bit_uncompressed.tif").c_str());
  // If this test fails check that you have passed the correct image_base directory as a program argument.
  TEST("vil_load worked", tiff_image, true);

  vil2_image_view<vxl_byte> ppm_image = vil2_load( (image_base+"ff_rgb8bit_raw.ppm").c_str());
  TEST("vil2_load worked", ppm_image, true);
  
  vil2_image_resource_sptr resource1 = vil2_vil1_to_image_resource(tiff_image);
  image1 = resource1->get_view(0, resource1->ni(), 0, resource1->nj());
  vil2_print_all(vcl_cout, image1);
  vil2_print_all(vcl_cout, ppm_image);
  


  SUMMARY();
}
