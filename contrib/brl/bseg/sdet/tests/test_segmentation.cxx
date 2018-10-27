// This is brl/bseg/sdet/tests/test_segmentation.cxx
#include <string>
#include <map>
#include <fstream>
#include <sdet/sdet_graph_img_seg.h>
#include <sdet/sdet_region.h>
#include <sdet/sdet_region_sptr.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <testlib/testlib_test.h>
static void test_segmentation(int argc, char * argv[])
{
  vil_image_view<vxl_byte> img(4,4);
  img.fill(0); img(1,2) = 255;  img(2,2) = 255;
  vil_image_view<vil_rgb<vxl_byte> > out_img;
  sdet_segment_img<vxl_byte>(img, 1, 8, (vxl_byte)100, 0, 0, out_img);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) != out_img(1,2), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(1,1) == out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) != out_img(2,1), true);
  TEST("out img pix 0 is not the same as pix 1", out_img(2,2) == out_img(1,2), true);
  /*
  std::string image_path = ".\\beach.png";
  std::cout << "Loading Image " << image_path << '\n';
  vil_image_view<vxl_byte> imgi = vil_load(image_path.c_str());
  vil_image_view<vxl_byte> img_grey;
  //vil_convert_rgb_to_grey(img, img_grey);
  vil_convert_planes_to_grey(imgi,img_grey);
  vil_image_view<vil_rgb<vxl_byte> > out_imgi;
  sdet_segment_img<vxl_byte>(img_grey, 10, 8, 50, 1, 50, out_imgi);
  vil_save(out_imgi, ".\\beach_out.tif");


  // test a float image
  image_path = ".\\0_09SEP07161459-P1BS-052808601050_01_P004.NTF_reflectance_truncated.tif";
  vil_image_view<float> imgi2 = vil_load(image_path.c_str());
  vil_image_view<vil_rgb<vxl_byte> > out_imgi2;
  sdet_segment_img<float>(imgi2, 10, 8, 0.1, 1, 50, out_imgi2);

  std::string out_img_path = ".\\0_09SEP07161459-P1BS-052808601050_01_P004.NTF_reflectance_truncated_seg.tif";
  vil_save(out_imgi2, out_img_path.c_str());
   */
}

TESTMAIN_ARGS(test_segmentation);
