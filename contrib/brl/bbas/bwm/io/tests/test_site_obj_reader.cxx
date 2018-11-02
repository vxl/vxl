#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "../bwm_site.h"
#include <vul/vul_file.h>
#include <vpl/vpl.h>
void test_site_obj_reader()
{
  // test obj file
  std::string obj_path = "./test_obj";
  std::ofstream ostr(obj_path.c_str());
  if(!ostr){
    std::cout << "can't open test path" << std::endl;
    TEST("site obj reader", false, true);
    return;
  }
  ostr << "g 1" << std::endl;
  ostr << "v 0.0 0.0 0.0 " << std::endl;
  ostr << "v 10.0 0.0 0.0 " << std::endl;
  ostr << "v 10.0 10.0 0.0 " << std::endl;
  ostr << "v 0.0 10.0 0.0 " << std::endl;
  ostr << "f 1 2 3 4 " << std::endl;
  ostr << "g 2" << std::endl;
  ostr << "v 10.0 0.0 0.0 " << std::endl;
  ostr << "v 20.0 0.0 0.0 " << std::endl;
  ostr << "v 20.0 20.0 0.0 " << std::endl;
  ostr << "v 10.0 20.0 0.0 " << std::endl;
  ostr << "v 20.0 0.0 0.0 " << std::endl;
  ostr << "v 30.0 0.0 0.0 " << std::endl;
  ostr << "v 30.0 30.0 0.0 " << std::endl;
  ostr << "v 20.0 30.0 0.0 " << std::endl;
  ostr << "f 5 6 7 8" << std::endl;
  ostr << "f 9 10 11 12" << std::endl;
  ostr.close();
  bwm_site bs;
  bs.name_ = "bwm_test_site";
  bs.path_ = ".";
  std::string dir_path = bs.path_ + "/" + bs.name_ +"_objects/";
  vul_file::make_directory(dir_path);
  bs.convert_object_groups_obj_to_multiple_ply(obj_path);
  // test first file
  std::string out_file = dir_path + "mets_1.ply";
  std::ifstream tistr(out_file.c_str());
  std::string temp;
  while(tistr >> temp >>std::ws && temp!="vertex")
    ;
  size_t nv;
  tistr >> nv;
  tistr.close();
  TEST("convert obj to ply",nv, 4);
  vul_file::delete_file_glob(dir_path + "*");
  vpl_rmdir(dir_path.c_str());
}
TESTMAIN(test_site_obj_reader);
