// This is mul/mbl/tests/test_read_props.cxx
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vul/vul_string.h>

void test_string()
{
  vcl_cout << "****************************\n"
           << " Testing vul_string_to_bool \n"
           << "****************************\n";


  TEST ("\"YES\"", vul_string_to_bool("YES"), true);
  TEST ("\"trUe\"", vul_string_to_bool("trUe"), true);
  TEST ("\" 1 \"", vul_string_to_bool(" 1 "), true);
  TEST ("\" on\"", vul_string_to_bool(" on"), true);
  TEST ("Not \"FALSE\"", vul_string_to_bool("FALSE"), false);
  TEST ("Not \"0\"", vul_string_to_bool("0"), false);
  TEST ("Not \"onwibble\"", vul_string_to_bool("onwibble"), false);


}

TESTMAIN(test_string);
