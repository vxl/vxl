// This is mul/mbl/tests/test_parse_colon_pairs_list.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>


void test_parse_colon_pairs_list()
{
  vcl_cout << "\n************************************\n"
           <<   " Testing mbl_parse_colon_pairs_list\n"
           <<   "************************************\n";

  {
    vcl_cout << "\nCase 0\n";
    vcl_istringstream ss("{}");
    vcl_vector<vcl_string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(vcl_cout, item1,item2);
    TEST("Case 1: \"{}\"", item1.empty() && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 1\n";
    vcl_istringstream ss("{ aaa1 : aaa2 }");
    vcl_vector<vcl_string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(vcl_cout, item1,item2);
    TEST("Case 1: ", item1.size()==1 && item1[0]=="aaa1" && item2[0]=="aaa2" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 2\n";
    vcl_istringstream ss("{ aaa1 : aaa2   bbb1 : bbb2 }");
    vcl_vector<vcl_string> item1,item2;
    mbl_parse_colon_pairs_list(ss, item1,item2);
    mbl_write_colon_pairs_list(vcl_cout, item1,item2);
    TEST("Case 2: ", item1.size()==2 && item1[0]=="aaa1" && item2[0]=="aaa2" && !ss.fail(), true);
  }

  {
    vcl_cout << "\nTest missing }\n";
    vcl_istringstream ss("{ 1 : 2");
    vcl_vector<vcl_string> item1,item2;
    bool caught = false;
    try
    {
      mbl_parse_colon_pairs_list(ss, item1,item2);
    }
    catch (mbl_exception_parse_error& e)
    {
      vcl_cout<<"Caught error: "<<e.what()<<vcl_endl;
      caught = true;
    }
    TEST("Missing }", ss.fail() && caught, true);
  }

  vcl_cout << "\n\n";
}

TESTMAIN(test_parse_colon_pairs_list);
