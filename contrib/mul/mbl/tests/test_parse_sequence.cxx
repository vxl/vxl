// This is mul/mbl/tests/test_parse_sequence.cxx
#include <iostream>
#include <sstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_sequence.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>



void test_parse_sequence()
{
  std::cout << "\n****************************\n"
           <<   " Testing mbl_parse_sequence\n"
           <<   "****************************\n";


  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{}");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 1: \"{}\"", v.empty() && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 2\n";
    std::istringstream ss("{ 5 }");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 2: \"{ 5 }\"", v.size()==1 && v[0]==5 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 3\n";
    std::istringstream ss("{ 1 2 }");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 3: \"{ 1 2 }\"", v.size()==2 && v[0]==1  && v[1]==2 && !ss.fail(), true);
  }
  {
    std::cout << "\nCase 3b\n";
    std::istringstream ss("{ 1.0 2.0 }");
    std::vector<double> v;
    mbl_parse_sequence(ss, std::back_inserter(v), double());
    vsl_print_summary(std::cout, v);
    TEST("Case 3: \"{ 1.0 2.0 }\"", v.size()==2 && v[0]==1.0  && v[1]==2.0 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 4\n";
    std::istringstream ss("");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 4: \"{}\"", v.empty(), true);
  }

  {
    std::cout << "\nCase 5\n";
    std::istringstream ss(" 5 ");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    vsl_print_summary(std::cout, v);
    TEST("Case 5: \" 0 \"", v.size()==1 && v[0]==5 && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 6\n";
    std::istringstream ss(" 1 2 ");
    std::vector<unsigned> v;
    mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
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
      mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
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
      mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    vsl_print_summary(std::cout, v);
    TEST("Case 8: \"{ 1 2 \"", ss.fail() && caught, true);
  }

  {
    std::cout << "\nCase 8\n";
    std::istringstream ss("{ 1 2");
    std::vector<unsigned> v;
    bool caught = false;
    try
    {
      mbl_parse_sequence(ss, std::back_inserter(v), unsigned());
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
    std::istringstream ss("a ba");
    std::vector<std::string> v;
    mbl_parse_sequence(ss, std::back_inserter(v), std::string());
    vsl_print_summary(std::cout, v);
    TEST("Case 9 : \"a ba\"", v.size()==2 && v[0]=="a" && v[1]=="ba" , true);
  }



  std::cout << "\n\n";
}



TESTMAIN(test_parse_sequence);
