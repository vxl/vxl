// This is mul/mbl/tests/test_parse_block.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <mbl/mbl_parse_block.h>
#include <testlib/testlib_test.h>

void test_parse_block()
{
  vcl_cout << "\n*************************\n"
           <<   " Testing mbl_parse_block \n"
           <<   "*************************\n";


  vcl_stringstream  ss("{}");
  TEST("Trivial case 1", mbl_parse_block(ss) == "{}" && !ss.fail(), true);

  ss.str("}");
  ss.clear();
  TEST("Trivial case 2", mbl_parse_block(ss,true) == "{}" && !ss.fail(), true);

  ss.str(" {}");
  ss.clear();
  TEST("Trivial case 3", mbl_parse_block(ss) == "{}" && !ss.fail(), true);

  ss.str("{ abc }");
  ss.clear();
  TEST("Simple case 1", mbl_parse_block(ss) == "{ abc }" && !ss.fail(), true);
  
  ss.str("{ a\n bc }");
  ss.clear();
  TEST("Simple case 2", mbl_parse_block(ss) == "{ a\n bc }" && !ss.fail(), true);
  
  ss.str("{ a\n {b}c }");
  ss.clear();
  TEST("Multi-level case 1", mbl_parse_block(ss) == "{ a\n {b}c }" && !ss.fail(), true);
  
  ss.str("{ a\n {b\n  // wibble } \n}c }");
  ss.clear();
  TEST("Comment case 1", mbl_parse_block(ss) == "{ a\n {b\n}c }" && !ss.fail(), true);
  
  vcl_cout << "\n\n";
}

TESTLIB_DEFINE_MAIN(test_parse_block);
