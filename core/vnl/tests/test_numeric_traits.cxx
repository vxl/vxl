#include <vnl/vnl_numeric_traits.h>
#include <testlib/testlib_test.h>
#include <vcl_complex.h>
#include <vcl_iostream.h>

void test_numeric_traits()
{
  TEST("vnl_numeric_traits<bool>::zero", vnl_numeric_traits<bool>::zero, false);
  TEST("vnl_numeric_traits<bool>::one", vnl_numeric_traits<bool>::one, true);
  TEST("vnl_numeric_traits<char>::zero", vnl_numeric_traits<char>::zero, '\0');
  TEST("vnl_numeric_traits<char>::one", vnl_numeric_traits<char>::one, 1);
  TEST("vnl_numeric_traits<unsigned char>::zero", vnl_numeric_traits<unsigned char>::zero, '\0');
  TEST("vnl_numeric_traits<unsigned char>::one", vnl_numeric_traits<unsigned char>::one, 1);
  TEST("vnl_numeric_traits<signed char>::zero", vnl_numeric_traits<signed char>::zero, '\0');
  TEST("vnl_numeric_traits<signed char>::one", vnl_numeric_traits<signed char>::one, 1);
  TEST("vnl_numeric_traits<short>::zero", vnl_numeric_traits<short>::zero, 0);
  TEST("vnl_numeric_traits<short>::one", vnl_numeric_traits<short>::one, 1);
  TEST("vnl_numeric_traits<unsigned short>::zero", vnl_numeric_traits<unsigned short>::zero, 0);
  TEST("vnl_numeric_traits<unsigned short>::one", vnl_numeric_traits<unsigned short>::one, 1);
  TEST("vnl_numeric_traits<signed short>::zero", vnl_numeric_traits<signed short>::zero, 0);
  TEST("vnl_numeric_traits<signed short>::one", vnl_numeric_traits<signed short>::one, 1);
  TEST("vnl_numeric_traits<int>::zero", vnl_numeric_traits<int>::zero, 0);
  TEST("vnl_numeric_traits<int>::one", vnl_numeric_traits<int>::one, 1);
  TEST("vnl_numeric_traits<signed int>::zero", vnl_numeric_traits<signed int>::zero, 0);
  TEST("vnl_numeric_traits<signed int>::one", vnl_numeric_traits<signed int>::one, 1);
  TEST("vnl_numeric_traits<unsigned int>::zero", vnl_numeric_traits<unsigned int>::zero, 0);
  TEST("vnl_numeric_traits<unsigned int>::one", vnl_numeric_traits<unsigned int>::one, 1);
  TEST("vnl_numeric_traits<long>::zero", vnl_numeric_traits<long>::zero, 0L);
  TEST("vnl_numeric_traits<long>::one", vnl_numeric_traits<long>::one, 1L);
  TEST("vnl_numeric_traits<signed long>::zero", vnl_numeric_traits<signed long>::zero, 0L);
  TEST("vnl_numeric_traits<signed long>::one", vnl_numeric_traits<signed long>::one, 1L);
  TEST("vnl_numeric_traits<unsigned long>::zero", vnl_numeric_traits<unsigned long>::zero, 0L);
  TEST("vnl_numeric_traits<unsigned long>::one", vnl_numeric_traits<unsigned long>::one, 1L);
  TEST("vnl_numeric_traits<float>::zero", vnl_numeric_traits<float>::zero, 0.0f);
  TEST("vnl_numeric_traits<float>::one", vnl_numeric_traits<float>::one, 1.0f);
  TEST("vnl_numeric_traits<double>::zero", vnl_numeric_traits<double>::zero, 0.0);
  TEST("vnl_numeric_traits<double>::one", vnl_numeric_traits<double>::one, 1.0);
  TEST("vnl_numeric_traits<long double>::zero", vnl_numeric_traits<long double>::zero, 0.0);
  TEST("vnl_numeric_traits<long double>::one", vnl_numeric_traits<long double>::one, 1.0);
  TEST("vnl_numeric_traits<vcl_complex<float> >::zero",
       vnl_numeric_traits<vcl_complex<float> >::zero, vcl_complex<float>(0.0f));
  TEST("vnl_numeric_traits<vcl_complex<float> >::one",
       vnl_numeric_traits<vcl_complex<float> >::one, vcl_complex<float>(1.0f));
  TEST("vnl_numeric_traits<vcl_complex<double> >::zero",
       vnl_numeric_traits<vcl_complex<double> >::zero, vcl_complex<double>(0.0));
  TEST("vnl_numeric_traits<vcl_complex<double> >::one",
       vnl_numeric_traits<vcl_complex<double> >::one, vcl_complex<double>(1.0));
  TEST("vnl_numeric_traits<vcl_complex<long double> >::zero",
       vnl_numeric_traits<vcl_complex<long double> >::zero, vcl_complex<long double>(0.0));
  TEST("vnl_numeric_traits<vcl_complex<long double> >::one",
       vnl_numeric_traits<vcl_complex<long double> >::one, vcl_complex<long double>(1.0));
  vcl_cout << " vnl_numeric_traits<bool>::maxval = " << vnl_numeric_traits<bool>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<char>::maxval = " << (int)vnl_numeric_traits<char>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<unsigned char>::maxval = " << (int)vnl_numeric_traits<unsigned char>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<signed char>::maxval = " << (int)vnl_numeric_traits<signed char>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<short>::maxval = " << vnl_numeric_traits<short>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<unsigned short>::maxval = " << vnl_numeric_traits<unsigned short>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<int>::maxval = " << vnl_numeric_traits<int>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<unsigned int>::maxval = " << vnl_numeric_traits<unsigned int>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<long>::maxval = " << vnl_numeric_traits<long>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<unsigned long>::maxval = " << vnl_numeric_traits<unsigned long>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<float>::maxval = " << vnl_numeric_traits<float>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<double>::maxval = " << vnl_numeric_traits<double>::maxval << '\n';
  vcl_cout << " vnl_numeric_traits<long double>::maxval = " << vnl_numeric_traits<long double>::maxval << '\n';
}

TESTMAIN(test_numeric_traits);
