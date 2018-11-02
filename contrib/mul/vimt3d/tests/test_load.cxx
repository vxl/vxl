// This is mul/vimt3d/tests/test_load.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vil3d/vil3d_load.h>
#include <vimt3d/vimt3d_load.h>
#include <testlib/testlib_test.h>

static void test_load(int argc, char* argv[])
{
  std::cout << "*********************\n"
           << " Testing vimt3d_load\n"
           << "*********************\n";

  std::string imagename = argc<2 ? "../../vil3d/tests/file_read_data" : argv[1];
  imagename = imagename+"/ff_rgb8bit_ascii.1.ppm;"+
              imagename+"/ff_rgb8bit_ascii.2.ppm";
  vil3d_image_resource_sptr im = vil3d_load_image_resource(imagename.c_str());

  TEST("Load slice image", !im, false);
  if (im) {
    vimt3d_transform_3d t1 = vimt3d_load_transform(im);
    TEST("No header information", t1.is_identity(), true);
  }

  imagename = argc<2 ? "../../vil3d/tests/file_read_data" : argv[1];
  imagename = imagename+"/ff_grey_cross.gipl";
  im = vil3d_load_image_resource(imagename.c_str());

  TEST("Load gipl image", !im, false);

  if (!im) return;
  vimt3d_transform_3d t2 = vimt3d_load_transform(im);
  std::cout << "\n gipl transform: " << t2 << std::endl;

  typedef vgl_point_3d<double> GP;

  TEST_NEAR("Correct GIPL transform a",
            (t2(GP(0,0,0))-GP(0,0,0)).length(),0.0,1e-4);
  TEST_NEAR("Correct GIPL transform b",
            (t2(GP(0.001,0,0))-GP(1,0,0)).length(),0.0,1e-4);
  TEST_NEAR("Correct GIPL transform c",
            (t2(GP(0,0.001,0))-GP(0,1,0)).length(),0.0,1e-4);
  TEST_NEAR("Correct GIPL transform d",
            (t2(GP(0,0,0.001))-GP(0,0,1)).length(),0.0,1e-4);
}

TESTMAIN_ARGS(test_load);
