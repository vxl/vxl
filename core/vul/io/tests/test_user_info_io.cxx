// This is vxl/vul/io/tests/test_user_info_io.cxx
#include <vul/vul_user_info.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vul/io/vul_io_user_info.h>
#include <vsl/vsl_binary_io.h>

#include <testlib/testlib_test.h>

void test_user_info_io()
{
  vcl_cout << "************************\n";
  vcl_cout << "Testing vul_user_info io\n";
  vcl_cout << "************************\n";
  //
  vcl_string name="cjb";
  vul_user_info p_out(name), p_in("");
  p_out.init("cjb");


  vsl_b_ofstream bfs_out("vul_user_info_test_io.bvl.tmp");
  TEST ("Created vul_user_info_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, p_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vul_user_info_test_io.bvl.tmp");
  TEST ("Opened vul_user_info_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, p_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("p_out == p_in",p_out.uid==p_in.uid && p_out.gid==p_in.gid && 
    p_out.name==p_in.name && p_out.home_directory==p_in.home_directory && 
    p_out.full_name==p_in.full_name && p_out.shell==p_in.shell && 
    p_out.passwd==p_in.passwd, true);


  vsl_print_summary(vcl_cout, p_out);
  vcl_cout << vcl_endl;
}

TESTMAIN(test_user_info_io);
