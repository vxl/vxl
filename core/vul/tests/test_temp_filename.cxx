// Amitha Perera <perera@cs.rpi.edu>

#include <testlib/testlib_test.h>

#include <vul/vul_temp_filename.h>
#include <vpl/vpl.h>  // for vpl_unlink and vpl_sleep
#include <vcl_fstream.h>

static void test_temp_filename()
{
  {
    vcl_string filename = vul_temp_filename();
    vcl_cout << "vul_temp_filename() returns '" << filename << "'\n";

    vcl_ofstream ostr( filename.c_str() );
    TEST("Creating temporary file", ostr.good(), true);

    // Writing to temporary file:
    ostr << 1;
    TEST("Writing to temporary file", ostr.good(), true);
    ostr.close();
    // now reading back, to see if the file really exists:
    {
      vcl_ifstream istr( filename.c_str() );
      TEST("Opening temporary file", istr.good(), true);
      int i=0; istr >> i;
      TEST("Reading from temporary file", i, 1);
    }
    TEST("Removing temporary file", vpl_unlink(filename.c_str()) == -1, false);
  }

  {
    vcl_string filename1 = vul_temp_filename();
    vcl_cout << "vul_temp_filename() returns '" << filename1 << "'\n";
    vcl_ofstream ostr1( filename1.c_str() );
    vcl_string filename2 = vul_temp_filename();
    ostr1.close();
    vcl_cout << "vul_temp_filename() returns '" << filename2 << "'\n";

    TEST("Testing multiple calls", filename1 == filename2, false);
  }
}

TESTMAIN(test_temp_filename);
