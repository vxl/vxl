// This is core/vbl/io/tests/test_smart_ptr_io.cxx
#include <vcl_iostream.h>
#include "vbl_io_test_classes.h"
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_smart_ptr_io()
{
  vcl_cout << "**************************\n"
           << "Testing smart_ptr<impl> io\n"
           << "**************************\n";

  const int n = 50;

  vbl_smart_ptr<impl > sp1_out(new impl(n));
  vbl_smart_ptr<impl> sp2_out(sp1_out);
  vbl_smart_ptr<impl> null1_out, null2_out;
  vbl_smart_ptr<impl > sp1_in, sp2_in;
  vbl_smart_ptr<impl> null1_in(new impl(n)), null2_in;

  TEST("sp1_out->get_references() == 2", sp1_out->get_references() ,2);
  TEST("null1_in!=0", null1_in.ptr()!=0, true);
  TEST("null2_in==0", null2_in.ptr(), 0);

  vsl_b_ofstream bfs_out("vbl_smart_ptr_test_io.bvl.tmp");
  TEST("Created vbl_smart_ptr_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, sp1_out);
  vsl_b_write(bfs_out, sp2_out);
  vsl_b_write(bfs_out, null1_out);
  vsl_b_write(bfs_out, null2_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_smart_ptr_test_io.bvl.tmp");
  TEST("Opened vbl_smart_ptr_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, sp1_in);
  vsl_b_read(bfs_in, sp2_in);
  vsl_b_read(bfs_in, null1_in);
  vsl_b_read(bfs_in, null2_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vbl_smart_ptr_test_io.bvl.tmp");

  TEST("sp1_in == sp2_in", sp1_in, sp2_in);
  TEST("sp1_in->get_references() == 2", sp1_in->get_references(), 2);
  TEST("null1_in==0", null1_in.ptr(), 0);
  TEST("null2_in==0", null2_in.ptr(), 0);
  vsl_print_summary(vcl_cout, sp1_out); vcl_cout<<vcl_endl;
}

TESTMAIN(test_smart_ptr_io);
