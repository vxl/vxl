// This is mul/mbl/tests/test_read_props.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_read_props.h>

void test_read_props()
{
  vcl_cout << "************************\n"
           << " Testing mbl_read_props \n"
           << "************************\n";


  TEST ("\"YES\"", mbl_read_props_str_to_bool("YES"), true);
  TEST ("\"trUe\"", mbl_read_props_str_to_bool("trUe"), true);
  TEST ("\" 1 \"", mbl_read_props_str_to_bool(" 1 "), true);
  TEST ("\" on\"", mbl_read_props_str_to_bool(" on"), true);
  TEST ("Not \"FALSE\"", mbl_read_props_str_to_bool("FALSE"), false);
  TEST ("Not \"0\"", mbl_read_props_str_to_bool("0"), false);
  TEST ("Not \"onwibble\"", mbl_read_props_str_to_bool("onwibble"), false);


}

TESTLIB_DEFINE_MAIN(test_read_props);
