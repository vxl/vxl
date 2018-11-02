// This is core/vsl/tests/test_indent.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_indent.h>
#include <testlib/testlib_test.h>

static void test_indent1(int t)
{
  std::cout<<"Tab Size is "<<vsl_indent_tab(std::cout)<<std::endl
          <<vsl_indent()<<"No Indent\n";
  TEST("Tab size", vsl_indent_tab(std::cout), t);
  vsl_indent_inc(std::cout);
  std::cout<<vsl_indent()<<"1 Indent\n";
  vsl_indent_inc(std::cout);
  std::cout<<vsl_indent()<<"2 Indent\n";
  vsl_indent_dec(std::cout);
  std::cout<<vsl_indent()<<"1 Indent\n";
  vsl_indent_dec(std::cout);
  std::cout<<vsl_indent()<<"No Indent\n\n";
}

void test_indent()
{
  std::cout << "******************\n"
           << "Testing vsl_indent\n"
           << "******************\n\n";

  std::cout<<"Using Tab size 2:\n\n";
  vsl_indent_set_tab(std::cout,2);
  test_indent1(2);


  std::cout<<"Using Tab size 4:\n\n";
  vsl_indent_set_tab(std::cout,4);
  test_indent1(4);

  std::cout<<vsl_indent()<<"Indent 0\n";
  vsl_indent_inc(std::cout);
  std::cout<<vsl_indent()<<"Indent 1\n";
  vsl_indent_inc(std::cout);
  std::cout<<vsl_indent()<<"Indent 2\n";
  vsl_indent_clear(std::cout);
  std::cout<<vsl_indent()<<"Indent Cleared\n";
}

TESTMAIN(test_indent);
