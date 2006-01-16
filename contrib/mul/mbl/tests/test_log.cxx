// This is mul/mbl/tests/test_test.cxx
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <mbl/mbl_log.h>
#include <testlib/testlib_test.h>

void test_log()
{
  vcl_cout << "******************\n"
           << " Testing mbl_log\n"
           << "******************\n";

  vcl_ostringstream output;
  mbl_log_output log_output(&output, "");

  mbl_logger::root().default_logger.set(mbl_logger::ALL, log_output);

  mbl_logger current("wibble1");
  
  if (current.level() >= mbl_logger::INFO)
    current.log() << "Output this whatever" << vcl_endl;

  vcl_cout << "STRINGSTREAM OUTPUT: \""<<output.str()<<'\"';

  TEST("Log output is as expected", output.str(), "wibble1 Output this whatever\n");


}

TESTMAIN(test_log);
