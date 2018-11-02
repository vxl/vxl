// This is core/vul/tests/test_string.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vul/vul_string.h>
#include <vpl/vpl.h>

void test_string()
{
  std::cout << "********************\n"
           << " Testing vul_string\n"
           << "********************\n";

  {char s[] = " 1 trUe False";
   TEST("vul_string_c_upcase(\" 1 trUe False\")", vul_string_c_upcase(s), std::string(" 1 TRUE FALSE"));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_downcase(\" 1 trUe False\")", vul_string_c_downcase(s), std::string(" 1 true false"));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_capitalize(\" 1 trUe False\")", vul_string_c_capitalize(s), std::string(" 1 TrUe False"));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_trim(\" 1 trUe False\",\" \")", vul_string_c_trim(s," "), std::string("1trUeFalse"));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_left_trim(\" 1 trUe False\",\" \")", vul_string_c_left_trim(s," "), std::string("1 trUe False"));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_right_trim(\" 1 trUe False\",\" \")", vul_string_c_right_trim(s," "), std::string(s));
  }
  {char s[] = " 1 trUe False";
   TEST("vul_string_c_reverse(\" 1 trUe False\")", vul_string_c_reverse(s), std::string("eslaF eUrt 1 "));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_upcase(\" 1 trUe False\")", vul_string_upcase(s), std::string(" 1 TRUE FALSE"));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_downcase(\" 1 trUe False\")", vul_string_downcase(s), std::string(" 1 true false"));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_capitalize(\" 1 trUe False\")", vul_string_capitalize(s), std::string(" 1 TrUe False"));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_trim(\" 1 trUe False\",\" \")", vul_string_trim(s," "), std::string("1trUeFalse"));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_left_trim(\" 1 trUe False\")", vul_string_left_trim(s," "), std::string("1 trUe False"));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_right_trim(\" 1 trUe False\")", vul_string_right_trim(s," "), std::string(s));
  }
  {std::string s = " 1 trUe False";
   TEST("vul_string_reverse(\" 1 trUe False\")", vul_string_reverse(s), std::string("eslaF eUrt 1 "));
  }

  TEST("vul_string_atoi(\"123\")", vul_string_atoi("123"), 123);
  TEST("vul_string_atoi(\"-123\")", vul_string_atoi("-123"), -123);
  TEST("vul_string_atoi(\"0\")", vul_string_atoi("0"), 0);
  TEST("vul_string_atoi(\"nonsense\")", vul_string_atoi("nonsense"), 0);

  TEST("vul_string_atof_withsuffix(\"123\")", vul_string_atof_withsuffix("123"), 123.0);
  TEST("vul_string_atof_withsuffix(\"-123\")", vul_string_atof_withsuffix("-123"), -123.0);
  TEST("vul_string_atof_withsuffix(\"0\")", vul_string_atof_withsuffix("0"), 0.0);
  TEST("vul_string_atof_withsuffix(\"nonsense\")", vul_string_atof_withsuffix("nonsense"), 0.0);
  TEST("vul_string_atof_withsuffix(\"123k\")", vul_string_atof_withsuffix("123k"), 123.0e3);
  TEST("vul_string_atof_withsuffix(\"123kb\")", vul_string_atof_withsuffix("123ki"), 125952.0);
  TEST("vul_string_atof_withsuffix(\"123kb\")", vul_string_atof_withsuffix("123.0e-3ki"), 125.9520);
  TEST("vul_string_atof_withsuffix(\"3M\")", vul_string_atof_withsuffix("3M"), 3e6);
  TEST("vul_string_atof_withsuffix(\"-123Mb\")", vul_string_atof_withsuffix("-123Mi"), -128974848.0);
  TEST("vul_string_atof_withsuffix(\"-1G\")", vul_string_atof_withsuffix("-1G"), -1.0e9);
  TEST("vul_string_atof_withsuffix(\"1T\")", vul_string_atof_withsuffix("1T"), 1.0e12);


  TEST("vul_string_to_bool(\"YES\")", vul_string_to_bool("YES"), true);
  TEST("vul_string_to_bool(\"trUe\")", vul_string_to_bool("trUe"), true);
  TEST("vul_string_to_bool(\" 1 \")", vul_string_to_bool(" 1 "), true);
  TEST("vul_string_to_bool(\" on\")", vul_string_to_bool(" on"), true);
  TEST("vul_string_to_bool(Not \"FALSE\")", vul_string_to_bool("FALSE"), false);
  TEST("vul_string_to_bool(Not \"0\")", vul_string_to_bool("0"), false);
  TEST("vul_string_to_bool(Not \"onwibble\")", vul_string_to_bool("onwibble"), false);

  int int_list_data[] = {1,2,10,21,24,-1,-3,-5,-7};
  std::vector<int> int_list(int_list_data, int_list_data+9);
  TEST("vul_string_to_int_list(\"1:2,10,21:3:25,-1:-2:-7\")",
       vul_string_to_int_list("1:2,10,21:3:25,-1:-2:-7"), int_list);
  TEST("vul_string_to_int_list(\"1:2,10, 21:3:25\")",
       vul_string_to_int_list("1:2,10, 21:3:25").empty(), true);
  TEST("vul_string_to_int_list(\"1:2,10,\")",
       vul_string_to_int_list("1:2,10,").empty(), true);

  vpl_putenv("VUL_1=foo");
  vpl_putenv("VUL_2=bar");
  {
    std::string s("wibble$VUL_1wobble");
    TEST("vul_string_expand_var", vul_string_expand_var(s), true);
    TEST("vul_string_expand_var", s, "wibblefoowobble");
  }
  {
    std::string s("wibble$VUL_3wobble$VUL_2splat${VUL_1}");
    TEST("vul_string_expand_var", vul_string_expand_var(s), false);
    TEST("vul_string_expand_var", s, "wibble$VUL_3wobblebarsplatfoo");
  }
  {
    std::string s("wibble$VUL_3wobble$VUL_2splat$(VUL_1)");
    TEST("vul_string_expand_var", vul_string_expand_var(s), false);
    TEST("vul_string_expand_var", s, "wibble$VUL_3wobblebarsplatfoo");
  }
  {
    std::string s("wibble$$$VUL_1 wobble$[]");
    TEST("vul_string_expand_var", vul_string_expand_var(s), false);
    TEST("vul_string_expand_var", s, "wibble$foo wobble$[]");
  }

#if 0
  // replaces instances "find_str" in "full_str" with "replace_str"
  // a given "num_times"
  vul_string_replace( std::string& full_str,
                      const std::string& find_str,
                      const std::string& replace_str,
                      int num_times=1000);
#endif // 0
  {
    std::string s("I hate mices to pieces");
    vul_string_replace( s, "hate", "love" );
    TEST( "vul_string_replace", s== "I love mices to pieces", true);
  }

  {
    std::string s("Too many \r\003\t\v\a\f\x05 control chars.\n");
    TEST("vul_string_escape_dodgy_chars(\"Too many \\r\\003\\t\\v\\a\\f\\x05 control chars.\\n\")",
         vul_string_escape_ctrl_chars(s), "Too many \\r\\x03\\t\\v\\a\\f\\x05 control chars.\n");
  }
}

TEST_MAIN(test_string);
