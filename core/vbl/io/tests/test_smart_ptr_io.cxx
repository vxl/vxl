#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <testlib/testlib_test.h>
#include "vbl_io_test_classes.h"
#include "vbl_io_test_classes.cxx"


void test_smart_ptr_io()
{
  vcl_cout << "**************************\n";
  vcl_cout << "Testing smart_ptr<impl> io\n";
  vcl_cout << "**************************\n";

  const int n = 50;

  vbl_smart_ptr<impl > sp1_out(new impl(n));
  vbl_smart_ptr<impl> sp2_out(sp1_out);
  vbl_smart_ptr<impl > sp1_in, sp2_in;

  TEST ("sp1_out->get_references() == 2", sp1_out->get_references() ,2);

  vsl_b_ofstream bfs_out("vbl_smart_ptr_test_io.bvl.tmp");
  TEST ("Created vbl_smart_ptr_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, sp1_out);
  vsl_b_write(bfs_out, sp2_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_smart_ptr_test_io.bvl.tmp");
  TEST ("Opened vbl_smart_ptr_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, sp1_in);
  vsl_b_read(bfs_in, sp2_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST ("sp1_in == sp2_in", sp1_in, sp2_in);
  TEST ("sp1_in->get_references() == 2", sp1_in->get_references(), 2);
  vsl_print_summary(vcl_cout, sp1_out); vcl_cout<<vcl_endl;
}

void test_smart_ptr_prime()
{
  test_smart_ptr_io();
}


TESTLIB_DEFINE_MAIN(test_smart_ptr_prime);
