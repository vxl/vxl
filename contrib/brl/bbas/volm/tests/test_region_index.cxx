#include <testlib/testlib_test.h>
#include <volm/volm_spherical_region_index.h>
#include <vul/vul_file.h>

static void test_region_index()
{
  // input files
  std::string sph_index_file = "";
  std::string unit_sph_file = "";
  if (vul_file::exists(sph_index_file) && vul_file::exists(unit_sph_file)) {
    std::map<std::string, std::string> filenames;
    filenames["ORIENTATION"] = sph_index_file;
    volm_spherical_region_index region_index(filenames,unit_sph_file);
    region_index.print(std::cout);
  }
  else {
    std::cout << " input files required for test is missing, ignore..." << std::endl;
  }
}

TESTMAIN(test_region_index);
