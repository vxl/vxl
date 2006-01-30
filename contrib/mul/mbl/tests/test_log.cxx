// This is mul/mbl/tests/test_log.cxx
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <vcl_deprecated.h>
#include <mbl/mbl_log.h>
#include <testlib/testlib_test.h>

void test_log()
{
  vcl_cout << "*****************\n"
           << " Testing mbl_log\n"
           << "*****************\n";

  vcl_ostringstream output;
  mbl_log_output log_output(&output, "");

  mbl_logger::root().default_logger.set(mbl_logger::INFO, log_output);

  mbl_logger current("wibble1");

  if (current.level() >= mbl_logger::INFO)
    current.log(mbl_logger::INFO) << "Output this whatever" << vcl_endl;

  MBL_LOG( WARN, current, "Also this number " << 54 <<
    " and" << vcl_endl << "multiline message");
  current.log(mbl_logger::DEBUG) << "But not this " << vcl_endl;

  vcl_cout << "LOG OUTPUT:\n\""<<output.str()<<'\"' <<vcl_endl;

  TEST("Log output is as expected", output.str(),
       "INFO: wibble1 Output this whatever\n"
       "WARN: wibble1 Also this number 54 and\nmultiline message\n");
}

TESTMAIN(test_log);
