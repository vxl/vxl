// This is mul/mbl/tests/test_parse_block.cxx
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_block.h>
#include <testlib/testlib_test.h>

void test_parse_block()
{
  std::cout << "\n*************************\n"
           <<   " Testing mbl_parse_block\n"
           <<   "*************************\n";

  {
    std::istringstream ss("{}");
    TEST("Trivial case 1", mbl_parse_block(ss) == "{}" && !ss.fail(), true);
  }

  {
    std::istringstream ss("}");
    TEST("Trivial case 2", mbl_parse_block(ss,true) == "{}" && !ss.fail(), true);
  }

  {
    std::istringstream ss(" {}");
    TEST("Trivial case 3", mbl_parse_block(ss) == "{}" && !ss.fail(), true);
  }

  {
    std::istringstream ss("{ abc }");
    TEST("Simple case 1", mbl_parse_block(ss) == "{ abc }" && !ss.fail(), true);
  }

  {
    std::istringstream ss("{ a\n bc }");
    TEST("Simple case 2", mbl_parse_block(ss) == "{ a\n bc }" && !ss.fail(), true);
  }

  {
    std::istringstream ss("{ a\n {b}c }");
    TEST("Multi-level case 1", mbl_parse_block(ss) == "{ a\n {b}c }" && !ss.fail(), true);
  }

  {
    std::istringstream ss("{ a:\n {\n b: {\n c: cv\n d: dv { e: ev }\n }\n }\n}");
    TEST("Multi-level case 2", mbl_parse_block(ss) == "{ a:\n {\n b: {\n c: cv\n d: dv { e: ev }\n }\n }\n}" && !ss.fail(), true);
  }

  {
    std::istringstream ss("{ a\n {b\n  // wibble }\n}c }");
    TEST("Comment case 1", mbl_parse_block(ss) == "{ a\n {b\n}c }" && !ss.fail(), true);
  }
  {
    std::istringstream ss("// { a\n {b\n  // wibble }\n}\n //c }");
    TEST("Comment case 2", mbl_parse_block(ss) == "{b\n}" && !ss.fail(), true);
  }

  std::cout << "\n\n";
}

TESTMAIN(test_parse_block);
