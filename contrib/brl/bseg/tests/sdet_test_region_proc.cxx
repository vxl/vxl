// This is brl/bseg/tests/sdet_test_region_proc.cxx
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vil/vil_image.h>
#include <vil/vil_load.h>
#include <vdgl/vdgl_intensity_face.h>
#include <sdet/sdet_region_proc_params.h>
#include <sdet/sdet_region_proc.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_detector.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }

int main(int argc, char * argv[])
{
  int success=0, failures=0;
  vcl_string image_path = (argc < 2) ? "" : argv[1];
  if (image_path=="")
    image_path = "c:/images/ShortBaseline/jar-closeup.tif";
  vcl_cout << "Loading Image " << image_path << "\n";
  vil_image image = vil_load(image_path.c_str());
  if (image)
    {
      static sdet_detector_params dp;
      dp.noise_multiplier=1.0;
      dp.aggressive_junction_closure=1;
      sdet_region_proc_params rpp(false, true, dp);
      sdet_region_proc rp(rpp);
      rp.set_image(image);
      rp.extract_regions();
      vcl_vector<vdgl_intensity_face_sptr>& regions = rp.get_regions();
      int n = regions.size();
      vcl_cout << "nregions = " << n << "\n";
      Assert(n==192);
      vdgl_intensity_face_sptr f = regions[0];
      vcl_cout << "f->Npix() " << f->Npix() << "\n";
      Assert(f->Npix()==41348);
    }
  vcl_cout << "finished testing sdet_detector\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
