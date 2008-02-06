// This is mul/mbl/tests/test_parse_tuple.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_vector.h>

#include <mbl/mbl_parse_tuple.h>
#include <vsl/vsl_vector_io.h>

#include <testlib/testlib_test.h>



void test_parse_tuple()
{
  vcl_cout << "\n*************************\n"
           <<   " Testing mbl_parse_tuple\n"
           <<   "*************************\n";


  {
    vcl_cout << "\nCase 1\n";
    vcl_istringstream ss("{ 1 1.0 }");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    vcl_cout << "Parsed: " << a << ' ' << b << vcl_endl;

    TEST("Case 1: \"{  1 1.0 }\"", a==1 && b==1.0, true);
  }

  {
    vcl_cout << "\nCase 2\n";
    vcl_istringstream ss(" 1 1.0");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    vcl_cout << "Parsed: " << a << ' ' << b << vcl_endl;

    TEST("Case 2: \"  1 1.0\"", a==1 && b==1.0, true);
  }

  {
    vcl_cout << "\nCase 3\n";
    vcl_istringstream ss("1 1.0");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    vcl_cout << "Parsed: " << a << ' ' << b << vcl_endl;

    TEST("Case 3: \" 1 1.0\"", a==1 && b==1.0, true);
  }

  {
    vcl_cout << "\nCase 4\n";
    vcl_istringstream ss("{ 1 1.0 a }");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    vcl_cout << "Parsed: " << a << ' ' << b << ' ' << c << vcl_endl;

    TEST("Case 4: \"{ 1 1.0 a }\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    vcl_cout << "\nCase 5\n";
    vcl_istringstream ss(" 1 1.0 a");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    vcl_cout << "Parsed: " << a << ' ' << b << ' ' << c << vcl_endl;

    TEST("Case 5: \" 1 1.0 a\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    vcl_cout << "\nCase 6\n";
    vcl_istringstream ss("1 1.0 a");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    vcl_cout << "Parsed: " << a << ' ' << b << ' ' << c << vcl_endl;

    TEST("Case 6: \"1 1.0 a\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    vcl_cout << "\nCase 7\n";
    vcl_istringstream ss(" 1 1.0 a -4 ");
    unsigned a;
    double b;
    char c;
    short d;
    mbl_parse_tuple(ss, a, b, c, d);
    vcl_cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << vcl_endl;

    TEST("Case 7: \"  1 1.0 a -4 \"", a==1 && b==1.0 && c=='a' && d==-4, true);
  }

  {
    vcl_cout << "\nCase 8\n";
    vcl_istringstream ss("{ 1 1.0 a -4}");
    unsigned a;
    double b;
    char c;
    short d;
    mbl_parse_tuple(ss, a, b, c, d );
    vcl_cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << vcl_endl;

    TEST("Case 8: \"{ 1 1.0 a -4 }\"", a==1 && b==1.0 && c=='a' && d==-4, true);
  }


  {
    vcl_cout << "\nCase 9\n";
    vcl_istringstream ss("{}");
    unsigned a;
    double b;
    bool caught = false;
    try
    {
      mbl_parse_tuple(ss, a, b);
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    TEST("Case 9: \"{}\"", ss.fail() && caught, true);
  }

  {
    vcl_cout << "\nCase 10\n";
    vcl_istringstream ss("");
    unsigned a;
    double b;
    bool caught = false;
    try
    {
      mbl_parse_tuple(ss, a, b);
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    TEST("Case 10: \"\"", ss.fail() && caught, true);
  }

  {
    vcl_cout << "\nCase 11\n";
    vcl_istringstream ss("{ 1.0 1 }");
    unsigned a;
    double b;
    bool caught = false;
    try
    {
      mbl_parse_tuple(ss, a, b);
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    TEST("Case 11: \"{ 1.0 1 }\"", ss.fail() && caught, true);
  }

  {
    vcl_cout << "\nCase 12\n";
    vcl_istringstream ss("{ 1 }");
    unsigned a;
    double b;
    bool caught = false;
    try
    {
      mbl_parse_tuple(ss, a, b);
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    TEST("Case 12: \"{ 1 }\"", ss.fail() && caught, true);
  }
  {
    vcl_cout << "\nCase 13\n";
    vcl_istringstream ss("1");
    unsigned a;
    double b;
    bool caught = false;
    try
    {
      mbl_parse_tuple(ss, a, b);
    }
    catch (mbl_exception_parse_error&)
    {
      caught = true;
    }
    TEST("Case 13: \"1\"", ss.fail() && caught, true);
  }

  vcl_cout << "\n\n";
}



TESTMAIN(test_parse_tuple);
