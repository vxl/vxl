// This is brl/bseg/tests/sdet_test_region_proc.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vtol/vtol_intensity_face.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>
#include <testlib/testlib_test.h>

static void sdet_test_region_proc(int argc, char * argv[])
{
  vcl_string image_path = (argc < 2) ? "jar-closeup.tif" : argv[1];
  vcl_cout << "Loading Image " << image_path << '\n';
  vil1_image image = vil1_load(image_path.c_str());
  if (image)
  {
    static sdet_detector_params dp;
    dp.noise_multiplier=1.0;
    dp.aggressive_junction_closure=1;
    sdet_region_proc_params rpp(dp);
    sdet_region_proc rp(rpp);
    rp.set_image(image);
    rp.extract_regions();
    vcl_vector<vtol_intensity_face_sptr>& regions = rp.get_regions();
    int n = regions.size();
    vcl_cout << "nregions = " << n << '\n';
    TEST("nregions should be around 187", n>=185 && n<=193, true);
    if (n>0)
    {
      vtol_intensity_face_sptr f = regions[0];
      vcl_cout << "f->Npix() " << f->Npix() << '\n';
      TEST("size of first region should be around 41120",
           f->Npix()>=41120 && f->Npix()<=41121, true);
    }
  }
}

TESTMAIN_ARGS(sdet_test_region_proc);
