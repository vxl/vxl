// This is brl/bseg/betr/tests/test_json_params.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <vpl/vpl.h>
#include <testlib/testlib_test.h>
#include <bjson/bjson.h>
#include "../betr_params.h"
#include "../betr_edgel_change_detection_params.h"
#include "../betr_edgel_factory_params.h"
void test_json_params()
{
  auto* cd_params = new betr_edgel_change_detection_params();
  auto* cd_params_read = new betr_edgel_change_detection_params();
  betr_params_sptr params_ptr = cd_params;
  betr_params_sptr read_params_ptr = cd_params_read;
  std::ostringstream ostr;
  bool good = write_params_json(ostr, params_ptr);
  std::cout << ostr.str();
  std::string path = "./test_json.json";
  std::ofstream ofs(path.c_str());
  std::string cd_json1 ="{\n\"noise_mul\":1.75,\n\"sigma\":2.0\n}";
  ofs << cd_json1;
  ofs.close();
  std::ifstream ifs(path.c_str());
  good = good && read_params_json(ifs, read_params_ptr);
  ifs.close();
  auto* read_cd = dynamic_cast<betr_edgel_change_detection_params*>(read_params_ptr.ptr());
  good = good && read_cd && read_cd->sigma_ == 2.0;
  TEST("JSON read write params", good, true);
  std::string empty_json = "{}";
  auto* empty_cd = new betr_edgel_change_detection_params();
   betr_params_sptr empty_ptr = empty_cd;
   good = read_params_json(empty_json, empty_ptr);
   auto* read_empty_cd = dynamic_cast<betr_edgel_change_detection_params*>(empty_ptr.ptr());
   //edgel factory params tests
   auto* fparams = new betr_edgel_factory_params();
   betr_params_sptr fp_optr = fparams;
   std::ofstream ofsf(path.c_str());
   good = write_params_json(ofsf, fp_optr);
   ofsf.close();
   std::string test_ef = "{\"gradient_range\" : 60.0,\"min_region_edge_length\" : 10.0,\"nbins\" : 20,\"upsample_factor\" : 2.0}";
   auto* ifparams = new betr_edgel_factory_params();
   betr_params_sptr fp_ptr = ifparams;
   good = good && read_params_json(test_ef, fp_ptr);

  vpl_unlink(path.c_str());
}

TESTMAIN(test_json_params);
