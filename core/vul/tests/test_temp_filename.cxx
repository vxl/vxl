// Amitha Perera <perera@cs.rpi.edu>

#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>

#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h>  // for vpl_unlink and vpl_sleep
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_first()
{
  std::string filename = vul_temp_filename();
  std::cout << "vul_temp_filename() returns '" << filename << "'\n";

  std::ofstream ostr( filename.c_str() );
  TEST("Creating temporary file", ostr.good(), true);

  // Writing to temporary file:
  ostr << 1;
  TEST("Writing to temporary file", ostr.good(), true);
  ostr.close();
  // now reading back, to see if the file really exists:
  {
    std::ifstream istr( filename.c_str() );
    TEST("Opening temporary file", istr.good(), true);
    int i=0; istr >> i;
    TEST("Reading from temporary file", i, 1);
  }
  TEST("Removing temporary file", vpl_unlink(filename.c_str()) == -1, false);
}

static void test_second()
{
  std::string filename1 = vul_temp_filename();
  std::cout << "vul_temp_filename() returns '" << filename1 << "'\n";
  // file must be opened for subsequent filename to be different (Borland)
  std::ofstream ostr1( filename1.c_str() );
  std::string filename2 = vul_temp_filename();
  ostr1.close();
  std::cout << "vul_temp_filename() returns '" << filename2 << "'\n";

  TEST("Testing multiple calls", filename1 == filename2, false);
}

static void test_temp_filename()
{
  test_first();
  test_second();
}

TEST_MAIN(test_temp_filename);
