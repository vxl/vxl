// This is mul/mbl/tests/test_parse_int_list.cxx
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_int_list.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>


void test_parse_int_list()
{
  std::cout << "\n****************************\n"
           <<   " Testing mbl_parse_int_list\n"
           <<   "****************************\n";

  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{}");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 1: \"{}\"", v.empty() && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 2\n";
    std::istringstream ss("{ 5 }");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 2: \"{ 5 }\"", v.size()==1 && v[0]==5 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 3\n";
    std::istringstream ss("{ 1 2 }");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 3: \"{ 1 2 }\"", v.size()==2 && v[0]==1  && v[1]==2 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 4\n";
    std::istringstream ss("");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 4: \"\"", v.empty(), true);
  }

  {
    std::cout << "\nCase 5\n";
    std::istringstream ss(" 0 ");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 5: \" 0 \"", v.size()==1 && v[0]==0 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 6\n";
    std::istringstream ss(" 1 2 ");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 6: \" 1 2 \"", v.size()==2 && v[0]==1 && v[1]==2 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 7\n";
    std::istringstream ss(" 1 2 }");
    std::vector<unsigned> v;
    bool caught = false;
    try
    {
      mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    vsl_print_summary(std::cout, v);
    TEST("Case 7: \" 1 2 }\"", ss.fail() && caught, true);
  }

  {
    std::cout << "\nCase 8\n";
    std::istringstream ss("{ 1 2");
    std::vector<unsigned> v;
    bool caught = false;
    try
    {
      mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    vsl_print_summary(std::cout, v);
    TEST("Case 8: \"{ 1 2 \"", ss.fail() && caught, true);
  }

  {
    std::cout << "\nCase 9\n";
    std::istringstream ss(" 1 : 3 ");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 9: \" 1 : 3 \"", v.size()==3 && v[0]==1 && v[1]==2 && v[2]==3 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 10\n";
    std::istringstream ss(" { 1 : 3 } ");
    std::vector<unsigned> v;
    mbl_parse_int_list(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 10: \" { 1 : 3 }\"", v.size()==3 && v[0]==1 && v[1]==2 && v[2]==3 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 11\n";
    std::istringstream ss(" -1 : 1 ");
    std::vector<long> v;
    mbl_parse_int_list(ss, std::back_inserter(v), long());
    vsl_print_summary(std::cout, v);
    TEST("Case 11: \" -1 : 1 \"", v.size()==3 && v[0]==-1 && v[1]==0 && v[2]==1 && !ss.fail(), true);
  }
  {
    std::cout << "\nCase 12\n";
    std::istringstream ss(" -1 : -1 ");
    std::vector<int> v;
    mbl_parse_int_list(ss, std::back_inserter(v), long());
    vsl_print_summary(std::cout, v);
    TEST("Case 12: \" -1 : -1 \"", v.size()==1 && v[0]==-1 && !ss.fail(), true);
  }

  std::cout << "\n\n";
}


TESTMAIN(test_parse_int_list);
