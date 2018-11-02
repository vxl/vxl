// This is brl/bseg/sdet/tests/test_region_proc.cxx
#include <vector>
#include <string>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vtol/vtol_intensity_face.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
static void test_region_proc(int argc, char * argv[])
{
  std::string root = testlib_root_dir();
  std::string image_path = root + "/contrib/brl/bseg/sdet/tests/jar-closeup.tif";
  std::cout << "Loading Image " << image_path << '\n';
  vil_image_resource_sptr image = vil_load_image_resource(image_path.c_str());
  if (image)
  {
    static sdet_detector_params dp;
    dp.noise_multiplier=1.0f;
    dp.aggressive_junction_closure=1;
    sdet_region_proc_params rpp(dp);
    sdet_region_proc rp(rpp);
    rp.set_image_resource(image);
    rp.extract_regions();
    std::vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
    int n = regions.size();
    TEST_NEAR("nregions", n, 190, 5);
    if (n>0)
    {
      vtol_intensity_face_sptr f = regions[0];
      TEST_NEAR("size of first region", f->Npix(), 41100, 50);
    }
  }else{
    TEST("image could not be loaded - no fault", true, true);
  }
}

TESTMAIN_ARGS(test_region_proc);
