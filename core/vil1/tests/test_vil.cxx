// This is core/vil1/tests/test_vil.cxx
#include <vil1/vil1_vil.h>
#include <vxl_config.h> // for vxl_byte
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_load.h>
#include <vil/vil_load.h>


static void test_vil(int argc, char* argv[])
{
  vcl_string image_base;
  if ( argc >= 2 )
  {
    image_base = argv[1];
#ifdef VCL_WIN32
    image_base += "\\";
#else
    image_base += "/";
#endif
  }

  vcl_cout << "************************\n"
           << " Testing vil1_vil_*(..)\n"
           << "************************\n";

  vil_image_view<vxl_byte> image0(10,8);

  for (unsigned int j=0;j<image0.nj();++j)
   for (unsigned int i=0;i<image0.ni();++i)
     image0(i,j) = i+j;

  vil1_memory_image_of<vxl_byte> vil1_mem = vil1_from_vil_image_view(image0);

  TEST ("vil1_from_vil_image_view sizes", vil1_mem.width() == int(image0.ni()) &&
    vil1_mem.height() == int(image0.nj()), true);

  bool bad_pixels = false;
  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      if (vil1_mem(i,j) != i+j) bad_pixels = true;

  TEST ("vil1_from_vil_image_view pixels", bad_pixels, false);

  vil_image_view<vxl_byte> image1 = vil1_to_vil_image_view(vil1_mem);

  TEST ("vil1_to_vil_image_view ", vil_image_view_deep_equality(image0, image1), true);


  vil1_image tiff_image = vil1_load( (image_base+"ff_rgb8bit_uncompressed.tif").c_str());
  // If this test fails check that you have passed the correct image_base directory as a program argument.
  TEST("vil1_load worked", tiff_image?true:false, true);

  vil_image_view<vxl_byte> ppm_image = vil_load( (image_base+"ff_rgb8bit_raw.ppm").c_str());
  TEST("vil_load worked", ppm_image?true:false, true);

  if (tiff_image && ppm_image)
  {
    vil_image_resource_sptr resource1 = vil1_to_vil_image_resource(tiff_image);
    image1 = resource1->get_view(0, resource1->ni(), 0, resource1->nj());
    TEST("Loaded images correctly via vil1", vil_image_view_deep_equality(image1, ppm_image),true);
  }

  // Check if we can create a image_view and put it into a vil1_image
  vil1_memory_image_of<double> vil_mem(10,8);
  vil_image_resource_sptr res1 = vil1_to_vil_image_resource(vil_mem);
  vil_image_view<double> image2(10,8);
  for (unsigned int j=0;j<image2.nj();++j)
   for (unsigned int i=0;i<image2.ni();++i)
     image2(i,j) = (double)i-(double)j;
  res1->put_view(image2,0,0);

  bad_pixels = false;
  for (int j=0;j<vil_mem.height();++j)
    for (int i=0;i<vil_mem.width();++i)
      if (vil_mem(i,j) != (double)i-(double)j) bad_pixels = true;

  TEST ("vil1_vil_image_resource::putview(..)", bad_pixels, false);
}

TESTMAIN_ARGS(test_vil);
