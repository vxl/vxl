// This is mul/mbl/tests/test_log.cxx
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cstddef.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_string_io.h>
#include <mbl/mbl_log.h>
#include <testlib/testlib_test.h>


namespace
{
  // This class stores log messages in a vector of strings - one log entry per element.
  // The last element should be empty and unused.
  class test_streambuf: public vcl_streambuf
  {
  public:
    vcl_vector<vcl_string> messages;
    test_streambuf(): messages(1) {}
    virtual int sync ()
    {
      vcl_ptrdiff_t n = pptr() - pbase();

      if (n)
        messages.back().append(pbase(), n);
      if (!messages.back().empty()) // Ignore flushes on empty messages
        messages.push_back(vcl_string());

      pbump(-n);  // Reset pptr().
      return 0;
    }

    virtual int overflow (int ch)
    {
      vcl_ptrdiff_t n = pptr() - pbase();

      if (n)
        messages.back().append(pbase(), n);
      pbump(-n);  // Reset pptr().

      if (ch == EOF)
      return 0;

      char cbuf = ch;
      messages.back().append(&cbuf, 1);
      return ch;
    }

    virtual vcl_streamsize xsputn(const char *ptr, vcl_streamsize nchar)
    {
      // Output anything already in buffer
      long n = pptr() - pbase();
      if (n)
        messages.back().append(pbase(), n);
      pbump(-n);  // Reset pptr().

      messages.back().append(ptr, nchar);
      return nchar;
    }
  };
}




void test_log()
{
  vcl_cout << "*****************\n"
           << " Testing mbl_log\n"
           << "*****************\n";
  {
    mbl_logger foo("foo");
    MBL_LOG(WARN, foo, "This is a simple test with no logger system initialisation");
  }


  vcl_ostringstream output;  // Standard test output

  test_streambuf my_test_streambuf; // Special test output
  vcl_ostream special_output(&my_test_streambuf);

  {
    vcl_ofstream cfg_file("mbl_log.properties");
    cfg_file << 
      "root: { level: INFO stream_output: test }\n"
      "obj3: { level: INFO stream_output: cout }\n"
      "obj4: { level: INFO stream_output: test_streambuf }\n";
  }
  vcl_map<vcl_string, vcl_ostream*> registered_streams;
  registered_streams["test"] = &output;
  registered_streams["test_streambuf"] = &special_output;

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
//    current.mtlog() << "Also this number " << 54
//                    << " and" << vcl_endl << "multiline message" << vcl_endl;
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


  // Check that is is possible to use a custom streambuf as the output destination, and
  // that flushes indicate a terminating log message.
  mbl_logger obj4("obj4");
  MBL_LOG(WARN, obj4, "A split line" << vcl_endl << "message");
  obj4.log(mbl_logger::NOTICE) << "A second message" << vcl_endl;

  vcl_cout << "Special output messages: ";
  vsl_print_summary(vcl_cout, my_test_streambuf.messages);

  // Test for one extra due to class invariant.
  TEST ("Special output got 2 messages", my_test_streambuf.messages.size(), 3);
  TEST ("Message 1 correct", my_test_streambuf.messages[0], "WARN: obj4 A split line\nmessage\n");
  TEST ("Message 2 correct", my_test_streambuf.messages[1], "NOTICE: obj4 A second message\n");


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
