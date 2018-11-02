// This is mul/mbl/tests/test_log.cxx
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstddef>
#include <iomanip>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_string_io.h>
#include <mbl/mbl_log.h>
#include <testlib/testlib_test.h>


namespace
{
  // This class stores log messages in a vector of strings - one log entry per element.
  // The last element should be empty and unused.
  class test_streambuf: public std::streambuf
  {
   public:
    std::vector<std::string> messages;
    test_streambuf(): messages(1) {}
    int sync () override
    {
      int n = static_cast<int>(pptr() - pbase()); // Can't be larger than int. See pbump in c++ standard

      if (n)
        messages.back().append(pbase(), n);
      if (!messages.back().empty()) // Ignore flushes on empty messages
        messages.emplace_back();

      pbump(-n);  // Reset pptr().
      return 0;
    }

    int overflow (int ch) override
    {
      int n = static_cast<int>(pptr() - pbase()); // Can't be larger than int. See pbump in c++ standard

      if (n)
        messages.back().append(pbase(), n);
      pbump(-n);  // Reset pptr().

      if (ch == EOF)
      return 0;

      char cbuf = (char)ch;
      messages.back().append(&cbuf, 1);
      return ch;
    }

    std::streamsize xsputn(const char *ptr, std::streamsize nchar) override
    {
      // Output anything already in buffer
      int n = static_cast<int>(pptr() - pbase()); // Can't be larger than int. See pbump in c++ standard
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
  std::cout << "*****************\n"
           << " Testing mbl_log\n"
           << "*****************\n";
  {
    mbl_logger foo("foo");
    MBL_LOG(WARN, foo, "This is a simple test with no logger system initialisation");
  }


  std::ostringstream output;  // Standard test output

  test_streambuf my_test_streambuf; // Special test output
  std::ostream special_output(&my_test_streambuf);

  {
    std::ofstream cfg_file("mbl_log.properties");
    cfg_file <<
      "root: { level: INFO stream_output: test }\n"
      "obj3: { level: INFO stream_output: cout dump_prefix: ./ }\n"
      "obj4: { level: INFO stream_output: test_streambuf }\n"
      "mul.mbl.log: { level: NOTICE stream_output: test_streambuf }\n";
  }
  std::map<std::string, std::ostream*> registered_streams;
  registered_streams["test"] = &output;
  registered_streams["test_streambuf"] = &special_output;

  mbl_logger::root().load_log_config_file(registered_streams);
  mbl_logger::root().categories().print(std::cout);

  mbl_logger current("wibble1");
  mbl_logger current2("wibble2");

  if (current.level() >= mbl_logger::INFO)
    current.log(mbl_logger::INFO) << "Output this whatever" << std::endl;

  MBL_LOG(WARN, current, "Also this number " << 54 <<
          " and" << std::endl << "multiline message" << std::setprecision(16));


  MBL_LOG( WARN, current2, "Check the precision changes do not propagate: " << 1.0/3.0 <<
    " (should be 6 not 16 prec)");

// Manual expansion of MBL_LOG macro
//  if (current.level() >= mbl_logger:: WARN)
//  {
//    current.mtstart(mbl_logger:: WARN, __FILE__, __LINE__);
//    current.mtlog() << "Also this number " << 54
//                    << " and" << std::endl << "multiline message" << std::endl;
//    current.mtstop();
//  }

  current.log(mbl_logger::DEBUG) << "But not this " << std::endl;

  std::cout << "LOG OUTPUT:\n\""<<output.str()<<'\"' <<std::endl;

  TEST("Log output is as expected", output.str(),
       "INFO: wibble1 Output this whatever\n"
       "WARN: wibble1 Also this number 54 and\nmultiline message\n"
       "WARN: wibble2 Check the precision changes do not propagate: 0.333333 (should be 6 not 16 prec)\n");


  mbl_logger obj3("obj3");
  std::cout << "Print some random stuff with no flush\n";
  MBL_LOG(WARN, obj3, "and check that this message doesn't preceed the direct output to cout");


  // Check for standard macro as function compile problems.
  if (true)
    MBL_LOG(WARN, obj3, "Test");
  else
    MBL_LOG(WARN, obj3, "Test");


  // Check that it is possible to use a custom streambuf as the output destination, and
  // that flushes indicate a terminating log message.
  mbl_logger obj4("obj4");
  MBL_LOG(WARN, obj4, "A split line" << std::endl << "message");
  obj4.log(mbl_logger::NOTICE) << "A second message" << std::endl;

  std::cout << "Special output messages: ";
  vsl_print_summary(std::cout, my_test_streambuf.messages);

  // Test for one extra due to class invariant.
  TEST ("Special output got 2 messages", my_test_streambuf.messages.size(), 3);
  TEST ("Message 1 correct", my_test_streambuf.messages[0], "WARN: obj4 A split line\nmessage\n");
  TEST ("Message 2 correct", my_test_streambuf.messages[1], "NOTICE: obj4 A second message\n");

  // Test dump_prefix stuff works
  TEST ("obj3.dump", obj3.dump(), true);
  TEST ("obj3.dump_prefix", obj3.dump_prefix(), "./");
  TEST ("obj4.dump", obj4.dump(), false);
  TEST ("current2.dump", current2.dump(), false);

  std::cout << "\n\n";

  {
    std::ofstream cfg_file("mbl_log.properties");
    cfg_file <<
      "AA.11.bb: { level: CRIT stream_output: cerr dump_prefix: ./logdump1_ }\n"
      "AA.22.aa.ii: { level: WARN file_output: test1.log }\n"
      "AA.11.aa: { level: ALERT }\n"
      "AA: { level: DEBUG }\n"
      "BB: { level: INFO }\n"
      "AA.11: { level: ERR dump_prefix: ./logdump2_ timestamp: %c }\n"
      "root: { level: EMERG }\n";
  }

  mbl_logger::root().load_log_config_file();
  mbl_logger::root().categories().print(std::cout);
  std::cout << "\n\n";

  TEST("AA.11.dd", mbl_logger::root().categories().get("AA.11.dd").level, mbl_logger::ERR);
  TEST("AA.11.dd dump", mbl_logger::root().categories().get("AA.11.dd").dump_prefix, "./logdump2_");
  TEST("AA.22", mbl_logger::root().categories().get("AA.22").level, mbl_logger::DEBUG);
  TEST("AA.22 dump", mbl_logger::root().categories().get("AA.22").dump_prefix, "");
  TEST("AA.111", mbl_logger::root().categories().get("AA.111").level, mbl_logger::DEBUG);
  TEST("AA.111 dump", mbl_logger::root().categories().get("AA.111").dump_prefix, "");
}

TESTMAIN(test_log);
