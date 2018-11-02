// This is mul/mbl/tests/test_parse_colon_pairs_list.cxx
#include <iostream>
#include <sstream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>


void test_parse_colon_pairs_list()
{
  std::cout << "\n************************************\n"
           <<   " Testing mbl_parse_colon_pairs_list\n"
           <<   "************************************\n";

  {
    std::cout << "\nCase 0\n";
    std::istringstream ss("{}");
    std::vector<std::string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(std::cout, item1,item2);
    TEST("Case 1: \"{}\"", item1.empty() && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{ aaa1 : aaa2 }");
    std::vector<std::string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(std::cout, item1,item2);
    TEST("Case 1: ", item1.size()==1 && item1[0]=="aaa1" && item2[0]=="aaa2" && !ss.fail(), true);
  }

  {
    std::cout << "\nCase 2\n";
    std::istringstream ss("{ aaa1 : aaa2   bbb1 : bbb2 }");
    std::vector<std::string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(std::cout, item1,item2);
    TEST("Case 2: ", item1.size()==2 && item1[0]=="aaa1" && item2[0]=="aaa2" && !ss.fail(), true);
  }

  {
    std::cout << "\nTest missing }\n";
    std::istringstream ss("{ 1 : 2");
    std::vector<std::string> item1,item2;
    bool caught = false;
    try
    {
      mbl_parse_colon_pairs_list(ss, item1,item2);
    }
    catch (mbl_exception_parse_error& e)
    {
      std::cout<<"Caught error: "<<e.what()<<std::endl;
      caught = true;
    }
    TEST("Missing }", ss.fail() && caught, true);
  }

  std::cout << "\n\n";
}

TESTMAIN(test_parse_colon_pairs_list);
