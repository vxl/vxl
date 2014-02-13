#include <testlib/testlib_test.h>
#include <volm/volm_spherical_region_index.h>
#include <vul/vul_file.h>

static void test_region_index()
{
  // input files
  vcl_string sph_index_file = "";
  vcl_string unit_sph_file = "";
  if (vul_file::exists(sph_index_file) && vul_file::exists(unit_sph_file)) {
    vcl_map<vcl_string, vcl_string> filenames;
    filenames["ORIENTATION"] = sph_index_file;
    volm_spherical_region_index region_index(filenames,unit_sph_file);
    region_index.print(vcl_cout);
  }
  else {
    vcl_cout << " input files required for test is missing, ignore..." << vcl_endl;
  }
}

TESTMAIN(test_region_index);
