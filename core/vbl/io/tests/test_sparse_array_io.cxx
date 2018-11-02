// This is core/vbl/io/tests/test_sparse_array_io.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/io/vbl_io_sparse_array_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_indent.h>
#include <vbl/vbl_sparse_array_2d.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_sparse_array_io()
{
  std::cout << "**************************************\n"
           << "Testing vbl_sparse_array_2d<double> io\n"
           << "**************************************\n";

  vbl_sparse_array_2d<double> v_out, v_in;

  // fill v_in with incorrect values
   v_in(4,5) = 3.0;

  // create a sparse array - more than 5 elements so only
  // the first 5 are written out
  v_out(1,1)=0.4;
  v_out(2000,10000)=1e5;
  v_out(10,10)=0.0;
  for (unsigned k=60; k<70; k++)
    v_out(k,400)=30.3;

  vsl_b_ofstream bfs_out("vbl_sparse_array_test_io.bvl.tmp");
  TEST("Created vbl_sparse_array_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, v_out);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vbl_sparse_array_test_io.bvl.tmp");
  TEST("Opened vbl_sparse_array_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, v_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vbl_sparse_array_test_io.bvl.tmp");

  bool test_result = true;
  //same number of non zero elements?
  if (v_out.count_nonempty() != v_in.count_nonempty())
    test_result=false;
  else {
    //check every key/data pair, require same order too.
  auto s = v_in.begin();
  vbl_sparse_array_2d<double>::const_iterator r;
  //N.B. relies on sensible == operator for <T>
  for (r = v_out.begin(); r != v_out.end(); ++r){
    if (!((*s).first == (*r).first) || !((*s).second == (*r).second))
      test_result=false;
    s++;
  }
  }
  TEST("v_out == v_in", test_result, true);

  vsl_print_summary(std::cout, v_in);
  std::cout << std::endl;
  vsl_indent_clear_all_data ();
}

TESTMAIN(test_sparse_array_io);
