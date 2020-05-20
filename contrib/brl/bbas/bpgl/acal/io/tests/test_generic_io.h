// This is acal/io/test/test_utils.h
#ifndef acal_io_test_utils_h_
#define acal_io_test_utils_h_

// This is acal/io/tests/test_acal_io_match_utils.cxx
#include <iostream>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

#include "testlib/testlib_test.h"
#include "vsl/vsl_binary_io.h"
#include "vsl/vsl_indent.h"
#include "vpl/vpl.h"

//: Generic test for acal io classes.  Requires the class TEST_T to have
// defined vsl_b_write, vsl_b_read, and an equality operator
template <class TEST_T>
void test_generic_io(const TEST_T& obj, std::string name)
{
  // note
  std::cout << "*******************************\n"
            << " Testing " << name << "\n"
            << "*******************************\n";

  // temporary file & message
  std::string tmp_file = name + ".bvl.tmp";

  // output
  vsl_b_ofstream os(tmp_file.c_str(), std::ios::out | std::ios::binary);
  TEST(("Created " + tmp_file + " for writing").c_str(), (!os), false);
  vsl_b_write(os, obj);
  os.close();

  // input
  vsl_b_ifstream is(tmp_file.c_str(), std::ios::in | std::ios::binary);
  TEST(("Opened " + tmp_file + " for reading").c_str(), (!is), false);

  TEST_T obj_copy;
  vsl_b_read(is, obj_copy);
  TEST(("Finished reading " + tmp_file).c_str(), (!is), false);
  is.close();

  // remove file
  vpl_unlink(tmp_file.c_str());

  // test equality
  TEST("object == vsl_b_read(vsl_b_write(object))", obj, obj_copy);

  // // print summary
  // vsl_print_summary(std::cout, obj_copy);
  // std::cout << std::endl;

  // cleanup
  vsl_indent_clear_all_data();
}

#endif // acal_io_test_utils_h_
