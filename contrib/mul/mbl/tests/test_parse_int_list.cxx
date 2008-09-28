// This is mul/mbl/tests/test_parse_int_list.cxx
#include <vcl_iostream.h>
#include <vcl_iterator.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <mbl/mbl_parse_int_list.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>


void test_parse_int_list()
{
  vcl_cout << "\n****************************\n"
           <<   " Testing mbl_parse_int_list\n"
           <<   "****************************\n";

  {
    vcl_cout << "\nCase 1\n";
    vcl_istringstream ss("{}");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 1: \"{}\"", v.empty() && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 2\n";
    vcl_istringstream ss("{ 5 }");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 2: \"{ 5 }\"", v.size()==1 && v[0]==5 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 3\n";
    vcl_istringstream ss("{ 1 2 }");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 3: \"{ 1 2 }\"", v.size()==2 && v[0]==1  && v[1]==2 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 4\n";
    vcl_istringstream ss("");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 4: \"\"", v.empty(), true);
  }

  {
    vcl_cout << "\nCase 5\n";
    vcl_istringstream ss(" 0 ");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 5: \" 0 \"", v.size()==1 && v[0]==0 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 6\n";
    vcl_istringstream ss(" 1 2 ");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 6: \" 1 2 \"", v.size()==2 && v[0]==1 && v[1]==2 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 7\n";
    vcl_istringstream ss(" 1 2 }");
    vcl_vector<unsigned> v;
    bool caught = false;
    try
    {
      mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    vsl_print_summary(vcl_cout, v);
    TEST("Case 7: \" 1 2 }\"", ss.fail() && caught, true);
  }

  {
    vcl_cout << "\nCase 8\n";
    vcl_istringstream ss("{ 1 2");
    vcl_vector<unsigned> v;
    bool caught = false;
    try
    {
      mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    vsl_print_summary(vcl_cout, v);
    TEST("Case 8: \"{ 1 2 \"", ss.fail() && caught, true);
  }

  {
    vcl_cout << "\nCase 9\n";
    vcl_istringstream ss(" 1 : 3 ");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 9: \" 1 : 3 \"", v.size()==3 && v[0]==1 && v[1]==2 && v[2]==3 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 10\n";
    vcl_istringstream ss(" { 1 : 3 } ");
    vcl_vector<unsigned> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), unsigned());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 10: \" { 1 : 3 }\"", v.size()==3 && v[0]==1 && v[1]==2 && v[2]==3 && !ss.fail(), true);
  }

  {
    vcl_cout << "\nCase 11\n";
    vcl_istringstream ss(" -1 : 1 ");
    vcl_vector<long> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), long());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 11: \" -1 : 1 \"", v.size()==3 && v[0]==-1 && v[1]==0 && v[2]==1 && !ss.fail(), true);
  }
  {
    vcl_cout << "\nCase 12\n";
    vcl_istringstream ss(" -1 : -1 ");
    vcl_vector<int> v;
    mbl_parse_int_list(ss, vcl_back_inserter(v), long());
    vsl_print_summary(vcl_cout, v);
    TEST("Case 12: \" -1 : -1 \"", v.size()==1 && v[0]==-1 && !ss.fail(), true);
  }

  vcl_cout << "\n\n";
}


TESTMAIN(test_parse_int_list);
