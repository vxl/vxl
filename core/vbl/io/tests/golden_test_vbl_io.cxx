// This is core/vbl/io/tests/golden_test_vbl_io.cxx
#include <testlib/testlib_test.h>
//:
// \file
// \brief Read in a golden data file, and check the values are correct.
//
// If you need to recreate the golden data file build this as a
// standalone program, and run it with the single parameter create:
// \verbatim
// golden_test_vnl_io create
// \endverbatim

#include <vsl/vsl_binary_io.h>

#include <vbl/io/vbl_io_array_1d.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vbl/io/vbl_io_array_3d.h>

#include "vbl_io_test_classes.h"

#include <vbl/io/vbl_io_smart_ptr.h>

#include <vbl/io/vbl_io_sparse_array_1d.h>
#include <vbl/io/vbl_io_bounding_box.h>

#include <vcl_string.h>
#include <testlib/testlib_root_dir.h>

static void golden_test_vbl_io(bool save_file)
{
  //------------------------------------------------------------------------
  // Create objects:
  // If the "create" flag was used on the command line, then the program saves
  // an example of each class. Otherwise it just fills them with values
  // for comparison to the values read in.
  //------------------------------------------------------------------------

  vcl_cout << "***********************************************************\n"
           << " Testing a golden data file for cross platform consistency\n"
           << "***********************************************************\n";

  // vbl_bounding_box
  vbl_bounding_box<double,2> b_box_in, b_box_out;

  b_box_out.update(1.2, 3.4); // Bounding box now has only one point and therefore no size
  b_box_out.update(5.6, 7.8); // Socond point now defines a bounding box

  // 1d Array
  int n = 50;
  vbl_array_1d<float> a1_out, a1_in;

  a1_out.reserve(n+10);
  for (int i=0; i<n; i++)
    a1_out.push_back(1.f*i*i);


  // 2d Array
  const int array_rows = 8;
  const int array_cols = 6;
  vbl_array_2d<int> a2_out(array_rows, array_cols), a2_in;

  for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j< array_cols; j++)
      a2_out(i,j) = i*j*j;
  }


  // 3d Array
  const int array_row1 = 8;
  const int array_row2 = 7;
  const int array_row3 = 9;
  vbl_array_3d<int> a3_out(array_row1, array_row2, array_row3), a3_in;
  for (int i=0; i<array_row1; i++)
    for (int j=0; j< array_row2; j++)
      for (int k=0; k<array_row3; k++)
        a3_out(i,j,k) = i*j*j*k;


  // Sparse Array
  vbl_sparse_array_1d<double> sa_out, sa_in;
  unsigned key1=3,key2=4,key3=5;
  double data1=1.2, data2=3.4, data3=5.6;

  //create a sparse array - more than 5 elements so only first 5 are written out
  sa_out(key1)=data1;
  sa_out(key2)=data2;
  sa_out(key3)=data3;
  for (unsigned k=60; k<70; k++)
    sa_out(k)=data1;


  // Smart Pointer
  n = 50;
  vbl_smart_ptr<impl > sp1_out(new impl(n));
  vbl_smart_ptr<impl> sp2_out(sp1_out);
  vbl_smart_ptr<impl > sp1_in, sp2_in;


  // Save if option set
  if (save_file)
  {
    vcl_cout << "Going to create the golden test file\n";
    vsl_b_ofstream bfs_out("golden_test_vbl_io.bvl");
    TEST("Opened golden_test_vbl_io.bvl for writing ", ! bfs_out, false);

    vsl_b_write(bfs_out, b_box_out);
    vsl_b_write(bfs_out, a1_out);
    vsl_b_write(bfs_out, a2_out);
    vsl_b_write(bfs_out, a3_out);
    vsl_b_write(bfs_out, sa_out);
    vsl_b_write(bfs_out, sp1_out);
    vsl_b_write(bfs_out, sp2_out);

    bfs_out.close();
  }

  // Read in file to each class in turn

  vcl_cout << "Did we get this far ?\n";

  vcl_string gold_path=testlib_root_dir()+"/core/vbl/io/tests/golden_test_vbl_io.bvl";
  vsl_b_ifstream bfs_in(gold_path.c_str());

  TEST("Opened golden_test_vbl_io.bvl for reading ", ! bfs_in, false);

  vsl_b_read(bfs_in, b_box_in);
  vsl_b_read(bfs_in, a1_in);
  vsl_b_read(bfs_in, a2_in);
  vsl_b_read(bfs_in, a3_in);
  vsl_b_read(bfs_in, sa_in);
  vsl_b_read(bfs_in, sp1_in);
  vsl_b_read(bfs_in, sp2_in);

  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();


  // Test that each object created is the same as read in from the file.

  // Test bounding box
  TEST("vbl_bounding_box: b_box_out.empty() == b_box_in.empty()", b_box_out.empty(), b_box_in.empty());
  TEST("vbl_bounding_box: b_box_out.min()[0] == b_box_in.min()[0]", b_box_out.min()[0], b_box_in.min()[0]);
  TEST("vbl_bounding_box: b_box_out.min()[1] == b_box_in.min()[1]", b_box_out.min()[1], b_box_in.min()[1]);
  TEST("vbl_bounding_box: b_box_out.max()[0] == b_box_in.max()[0]", b_box_out.max()[0], b_box_in.max()[0]);
  TEST("vbl_bounding_box: b_box_out.max()[1] == b_box_in.max()[1]", b_box_out.max()[1], b_box_in.max()[1]);


  //Test 1d array
  bool test_result1 = true;
  if (a1_out.size() != a1_in.size())
    test_result1 = false;
  else if (a1_out.capacity() != a1_in.capacity())
    test_result1 = false;
  else
  {
    unsigned array_size = a1_out.size();
    for (unsigned i=0; i<array_size; i++)
    {
      if (a1_out[i] != a1_in[i])
        test_result1 = false;
    }
  }
  TEST("vbl_array_1d: a1_out == a1_in", test_result1, true);


  // Test 2d array
  bool test_result2 = true;
  if (a2_out.rows() != a2_in.rows())
    test_result2 = false;
  else if (a2_out.cols() != a2_in.cols())
    test_result2 = false;
  else
  {
    unsigned array_rows = a2_out.rows();
    unsigned array_cols = a2_out.cols();
    for (unsigned i=0; i<array_rows; i++)
    {
      for (unsigned j=0; j<array_cols; j++)
        if (a2_out(i,j) != a2_in(i,j))
          test_result2 = false;
    }
  }
  TEST("vbl_array_2d: a2_out == a2_in", test_result2, true);


  // Test 3d array
  bool test_result3 = true;
  if (a3_out.get_row1_count() != a3_in.get_row1_count())
    test_result3 = false;
  else if (a3_out.get_row2_count() != a3_in.get_row2_count())
    test_result3 = false;
  else if (a3_out.get_row3_count() != a3_in.get_row3_count())
    test_result3 = false;
  else
  {
    unsigned array_row1 = a3_out.get_row1_count();
    unsigned array_row2 = a3_out.get_row2_count();
    unsigned array_row3 = a3_out.get_row3_count();
    for (unsigned i=0; i<array_row1; i++)
      for (unsigned j=0; j<array_row2; j++)
        for (unsigned k=0; k<array_row3; k++)
          if (a3_out(i,j,k) != a3_in(i,j,k))
            test_result3 = false;
  }
  TEST("vbl_array_3d: a3_out == a3_in", test_result3, true);


  // Test Sparse Array
  bool test_result4 = true;
  //same number of non zero elements?
  if (sa_out.count_nonempty() != sa_in.count_nonempty())
    test_result4=false;
  else {
    //check every key/data pair, require same order too.
    vbl_sparse_array_1d<double>::const_iterator s = sa_in.begin();
    //N.B. relies on sensible == operator for <T>
    for (vbl_sparse_array_1d<double>::const_iterator r = sa_out.begin(); r != sa_out.end(); ++r){
      if (((*s).first != (*r).first) || ((*s).second != (*r).second)) test_result4=false;
      s++;
    }
  }
  TEST("vbl_sparse_array_1d: sa_out == sa_in", test_result4, true);


  // Test Smart Pointer
  TEST("vbl_smart_ptr: sp1_in == sp2_in", sp1_in == sp2_in, true);
  TEST("vbl_smart_ptr: sp1_in->get_references() == 2",
       ((sp1_in) && (sp1_in->get_references() ==2)), true);
}

static void golden_test_vbl_io(int argc, char* argv[])
{
  golden_test_vbl_io(argc==2 && vcl_string(argv[1])==vcl_string("create"));
}

TESTMAIN_ARGS(golden_test_vbl_io);
