// This is mul/mbl/tests/test_log.cxx
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <vcl_fstream.h>
#include <mbl/mbl_log.h>
#include <testlib/testlib_test.h>

void test_log()
{
  vcl_cout << "*****************\n"
           << " Testing mbl_log\n"
           << "*****************\n";
  {
    mbl_logger foo("foo");
    MBL_LOG(WARN, foo, "This is a simple test with no logger system initialisation");
  }


  vcl_ostringstream output;


  {
    vcl_ofstream cfg_file("mbl_log.properties");
    cfg_file << 
      "root: { level: INFO stream_output: test }\n" <<
      "obj3: { level: INFO stream_output: cout }\n";
  }
  vcl_map<vcl_string, vcl_ostream*> registered_streams;
  registered_streams["test"] = &output;

  mbl_logger::root().load_log_config_file(registered_streams);
  mbl_logger::root().categories().print(vcl_cout);

  mbl_logger current("wibble1");

  if (current.level() >= mbl_logger::INFO)
    current.log(mbl_logger::INFO) << "Output this whatever" << vcl_endl;

  MBL_LOG( WARN, current, "Also this number " << 54 <<
    " and" << vcl_endl << "multiline message");
// Manual expansion of MBL_LOG macro
//  if (current.level() >= mbl_logger:: WARN)
//  {
//    current.mtstart(mbl_logger:: WARN, __FILE__, __LINE__);
//    current.mtlog() << "Also this number " << 54 <<
//    " and" << vcl_endl << "multiline message" << vcl_endl;
//    current.mtstop();
//  }

  current.log(mbl_logger::DEBUG) << "But not this " << vcl_endl;

  vcl_cout << "LOG OUTPUT:\n\""<<output.str()<<'\"' <<vcl_endl;

  TEST("Log output is as expected", output.str(),
       "INFO: wibble1 Output this whatever\n"
       "WARN: wibble1 Also this number 54 and\nmultiline message\n");


  mbl_logger obj3("obj3");
  vcl_cout << "Print some random stuff with no flush\n";
  MBL_LOG(WARN, obj3, "and check that this message doesn't preceed the direct output to cout");


  // Check for standard macro as function compile problems.
  if (true)
    MBL_LOG(WARN, obj3, "Test");
  else
    MBL_LOG(WARN, obj3, "Test");


  vcl_cout << "\n\n";

  {
    vcl_ofstream cfg_file("mbl_log.properties");
    cfg_file << 
      "AA.11.bb: { level: CRIT stream_output: cerr }\n"
      "AA.22.aa.ii: { level: WARN file_output: test1.log }\n"
      "AA.11.aa: { level: ALERT }\n"
      "AA: { level: DEBUG }\n"
      "BB: { level: INFO }\n"
      "AA.11: { level: ERR }\n"
      "root: { level: EMERG }\n";
  }

  mbl_logger::root().load_log_config_file();
  mbl_logger::root().categories().print(vcl_cout);
  vcl_cout << "\n\n";

  TEST("AA.11.dd", mbl_logger::root().categories().get("AA.11.dd").level, mbl_logger::ERR);
  TEST("AA.22", mbl_logger::root().categories().get("AA.22").level, mbl_logger::DEBUG);
  TEST("AA.111", mbl_logger::root().categories().get("AA.111").level, mbl_logger::DEBUG);
}

TESTMAIN(test_log);
