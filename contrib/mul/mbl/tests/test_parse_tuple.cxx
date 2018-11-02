// This is mul/mbl/tests/test_parse_tuple.cxx
#include <iostream>
#include <sstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mbl/mbl_parse_tuple.h>

#include <testlib/testlib_test.h>

void test_parse_tuple()
{
  std::cout << "\n*************************\n"
           <<   " Testing mbl_parse_tuple\n"
           <<   "*************************\n";

  {
    std::cout << "\nCase 1\n";
    std::istringstream ss("{ 1 1.0 }");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    std::cout << "Parsed: " << a << ' ' << b << std::endl;

    TEST("\"{  1 1.0 }\"", a==1 && b==1.0, true);
  }

  {
    std::cout << "\nCase 2\n";
    std::istringstream ss(" 1 1.0");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    std::cout << "Parsed: " << a << ' ' << b << std::endl;

    TEST("\"  1 1.0\"", a==1 && b==1.0, true);
  }

  {
    std::cout << "\nCase 3\n";
    std::istringstream ss("1 1.0");
    unsigned a;
    double b;
    mbl_parse_tuple(ss, a, b);
    std::cout << "Parsed: " << a << ' ' << b << std::endl;

    TEST("Case 3: \" 1 1.0\"", a==1 && b==1.0, true);
  }

  {
    std::cout << "\nCase 4\n";
    std::istringstream ss("{ 1 1.0 a }");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << std::endl;

    TEST("\"{ 1 1.0 a }\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    std::cout << "\nCase 5\n";
    std::istringstream ss(" 1 1.0 a");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << std::endl;

    TEST("Case 5: \" 1 1.0 a\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    std::cout << "\nCase 6\n";
    std::istringstream ss("1 1.0 a");
    unsigned a;
    double b;
    char c;
    mbl_parse_tuple(ss, a, b, c );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << std::endl;

    TEST("\"1 1.0 a\"", a==1 && b==1.0 && c=='a', true);
  }

  {
    std::cout << "\nCase 7\n";
    std::istringstream ss(" 1 1.0 a -4 ");
    unsigned a;
    double b;
    char c;
    short d;
    mbl_parse_tuple(ss, a, b, c, d);
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << std::endl;

    TEST("Case 7: \"  1 1.0 a -4 \"", a==1 && b==1.0 && c=='a' && d==-4, true);
  }

  {
    std::cout << "\nCase 8\n";
    std::istringstream ss("{ 1 1.0 a -4}");
    unsigned a;
    double b;
    char c;
    short d;
    mbl_parse_tuple(ss, a, b, c, d );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << std::endl;

    TEST("\"{ 1 1.0 a -4}\"", a==1 && b==1.0 && c=='a' && d==-4, true);
  }

  {
    std::cout << "\nCase 9\n";
    std::istringstream ss("{}");
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
    TEST("\"{}\"", ss.fail() && caught, true);
  }

  {
    std::cout << "\nCase 10\n";
    std::istringstream ss("");
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
    TEST("\"\"", ss.fail() && caught, true);
  }

  {
    std::cout << "\nCase 11\n";
    std::istringstream ss("{ 1.0 1 }");
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
    std::cout << "\nCase 12\n";
    std::istringstream ss("{ 1 }");
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
    TEST("\"{ 1 }\"", ss.fail() && caught, true);
  }
  {
    std::cout << "\nCase 13\n";
    std::istringstream ss("1");
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
    TEST("\"1\"", ss.fail() && caught, true);
  }
  {
    std::cout << "\nCase 14\n";
    std::istringstream ss("{ 1 1.0 a -4 -2e20 }");
    unsigned a;
    double b;
    char c;
    short d;
    float e;
    mbl_parse_tuple(ss, a, b, c, d, e );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << ' ' << e << std::endl;

    TEST("\"{ 1 1.0 a -4 -2e20 }\"", a==1 && b==1.0 && c=='a' && d==-4 && e==-2e20f, true);
  }
  {
    std::cout << "\nCase 15\n";
    std::istringstream ss("{ 1 1.0 a -4 -2e20 $ }");
    unsigned a;
    double b;
    char c;
    short d;
    float e;
    char f;
    mbl_parse_tuple(ss, a, b, c, d, e, f );
    std::cout << "Parsed: " << a << ' ' << b << ' ' << c << ' ' << d << ' ' << e << ' ' << f << std::endl;

    TEST("\"{ 1 1.0 a -4 -2e20 $ }\"", a==1 && b==1.0 && c=='a' && d==-4
      && e==-2e20f && f=='$', true);
  }

  std::cout << "\n\n";
}

TESTMAIN(test_parse_tuple);
