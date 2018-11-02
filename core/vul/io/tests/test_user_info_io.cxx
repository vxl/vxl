// This is core/vul/io/tests/test_user_info_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_user_info.h>
#include <vul/io/vul_io_user_info.h>
#include <vsl/vsl_binary_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_user_info_io()
{
  std::cout << "************************\n"
           << "Testing vul_user_info io\n"
           << "************************\n";
  //
  std::string name="cjb";
  vul_user_info p_out(name), p_in("");
  p_out.init("cjb");


  vsl_b_ofstream bfs_out("vul_user_info_test_io.bvl.tmp");
  TEST("Created vul_user_info_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vul_user_info_test_io.bvl.tmp");
  TEST("Opened vul_user_info_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vul_user_info_test_io.bvl.tmp");

  TEST("p_out == p_in",
       p_out.uid==p_in.uid && p_out.gid==p_in.gid &&
       p_out.name==p_in.name && p_out.home_directory==p_in.home_directory &&
       p_out.full_name==p_in.full_name && p_out.shell==p_in.shell &&
       p_out.passwd==p_in.passwd, true);

  vsl_print_summary(std::cout, p_out);
  std::cout << std::endl;
}

TESTMAIN(test_user_info_io);
