// This is mul/mbl/tests/test_parse_block.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <mbl/mbl_parse_block.h>
#include <testlib/testlib_test.h>

void test_parse_block()
{
  vcl_cout << "\n*************************\n"
           <<   " Testing mbl_parse_block\n"
           <<   "*************************\n";

#if VCL_HAS_WORKING_STRINGSTREAM
  {
    vcl_istringstream ss("{}");
    TEST("Trivial case 1", mbl_parse_block(ss) == "{}" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("}");
    TEST("Trivial case 2", mbl_parse_block(ss,true) == "{}" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss(" {}");
    TEST("Trivial case 3", mbl_parse_block(ss) == "{}" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{ abc }");
    TEST("Simple case 1", mbl_parse_block(ss) == "{ abc }" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{ a\n bc }");
    TEST("Simple case 2", mbl_parse_block(ss) == "{ a\n bc }" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{ a\n {b}c }");
    TEST("Multi-level case 1", mbl_parse_block(ss) == "{ a\n {b}c }" && !ss.fail(), true);
  }

  {
    vcl_istringstream ss("{ a\n {b\n  // wibble }\n}c }");
    TEST("Comment case 1", mbl_parse_block(ss) == "{ a\n {b\n}c }" && !ss.fail(), true);
  }

  vcl_cout << "\n\n";
#else // VCL_HAS_WORKING_STRINGSTREAM
  vcl_cout << "\nTests not run since this compiler has no fully functional std:stringstream\n\n";
#endif // VCL_HAS_WORKING_STRINGSTREAM
}

TESTMAIN(test_parse_block);
