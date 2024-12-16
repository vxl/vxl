// This is core/vil/tests/test_rgb.cxx
#include "testlib/testlib_test.h"
#include "vil/vil_open.h"
#include "vil/vil_stream.h"
#include "vil/vil_new.h"
#include "vil/vil_rgb.h"
#include <iostream>
#include <typeinfo>
#include <cmath>
#include <limits>

template <class T>
void
test_vil_rgb(T)
{
  T v = 7.0;
  vil_rgb<T> J0(v);
  TEST("Construct a vil_rgb value. ", J0, vil_rgb<T>(7.0, 7.0, 7.0));
  std::string type_name = typeid(J0).name();
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "vil_rgb<T> should be [7.0 7.0 7.0], is " << J0 << std::endl;
  J0.r = 6.0;
  TEST("Construct a vil_rgb value. ", J0, vil_rgb<T>(6.0, 7.0, 7.0));

  T r0 = 25.0, g0 = 35.0, b0 = 45.0;
  T r1 = 25.0, g1 = 35.0, b1 = 45.0;
  vil_rgb<T> B0(r0, g0, b0);
  TEST_NEAR("Convert vil_rpb to grey ", B0.grey(), 33.15, 0.15);

  vil_rgb<T> B1(r1, g1, b1);
  TEST("operator!= ", operator!=(B0, B1), false);
  B1.r = 15.0, B1.g = 25.0;
  TEST("operator!= ", operator!=(B0, B1), true);

  TEST("average ", average(B0, B1), vil_rgb<T>(20.0, 30.0, 45.0));
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "average of B0 and B1 should be [20 30 45], is " << average(B0, B1) << std::endl;
  TEST("add vil_rgb + vil_rgb ", operator+(B0, B1), vil_rgb<T>(40.0, 60.0, 90.0));
  // Work on mulitply vil_rgb * vil_rgb for char and unsigned char.
  // TEST("multipy vil_rgb * vil_rgb ", operator*(B0, B1), vil_rgb<T>(375.0, 875.0, 2025.0));

  double d = 5.0f;
  TEST("vil_rgb<T> / double ", operator/(B0, d), vil_rgb<double>(5.0, 7.0, 9.0));
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << type_name << " / double should be [5.0 7.0 9.0], is " << operator/(B0, d) << std::endl;
  TEST("vil_rgb<T> * double ", operator*(B0, d), vil_rgb<double>(125.0, 175.0, 225.0));
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << type_name << " * double should be [125.0 175.0 225.0], is " << operator*(B0, d) << std::endl;
  d = 3.0f;
  TEST("double * vil_rgb<T> ", operator*(d, B0), vil_rgb<double>(75.0, 105.0, 135.0));
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "double * " << type_name << " should be [75.0 105.0 135.0], is " << operator*(d, B0) << std::endl;

  vil_rgb<T> A(6.0, 8.0, 12.0);
  vil_rgb<T> A0(1.0, 2.0, 3.0);

  TEST("operator+ ", A.operator+(A0), vil_rgb<T>(7.0, 10.0, 15.0));
  TEST("operator- ", A.operator-(A0), vil_rgb<T>(5.0, 6.0, 9.0));
  TEST("operator/ ", A.operator/(A0), vil_rgb<T>(6.0, 4.0, 4.0));
  TEST("operator/ ", A.operator/(A0) == vil_rgb<T>(0.0, 1.0, 0.0), false);
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "operator/ should be false. " << A.operator/(A0) << " != " << vil_rgb<T>(0.0, 1.0, 0.0) << std::endl;

  TEST("operator+= ", A.operator+=(A0), vil_rgb<T>(7.0, 10.0, 15.0));
  TEST("operator-= ", A.operator-=(A0), vil_rgb<T>(6.0, 8.0, 12.0));
  TEST("operator-= ", A.operator-=(A0) == vil_rgb<T>(1.0, 0.0, 1.0), false);
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "operator-= should be false. " << A << " != " << vil_rgb<T>(1.0, 0.0, 1.0) << std::endl;

  A.r = 2.0, A.g = 4.0, A.b = 6.0;
  TEST("operator* ", A.operator*(2.0), vil_rgb<T>(4.0, 8.0, 12.0));
  TEST("operator/ ", A.operator/(2.0) == vil_rgb<T>(1.0, 2.0, 1.0), false);
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "operator/ should be [1.0 2.0 3.0], is " << A.operator/(2.0) << std::endl;

  TEST("operator/ ", A.operator/(2.0), vil_rgb<T>(1.0, 2.0, 3.0));
  TEST("operator*= ", A.operator*=(2.0), vil_rgb<T>(4.0, 8.0, 12.0));
  TEST("operator*= ", A.operator*=(4.0), vil_rgb<T>(16.0, 32.0, 48.0));
  TEST("operator/= ", A.operator/=(8.0), vil_rgb<T>(2.0, 4.0, 6.0));
  if (type_name != "struct vil_rgb<signed char>")
    std::cout << "operator/= should be [2.0 4.0 6.0], is " << A << std::endl;
}

static void
test_vil_rgb_bool()
{
  std::cout << "****************************************\n"
            << " Testing vil_rgb<bool>\n"
            << "****************************************\n";
  bool v = true;
  vil_rgb<bool> J0(v);
  TEST("bool ", J0, vil_rgb<bool>(1, 1, 1));
  std::cout << "vil_rgb<bool> should be [1, 1, 1], is " << J0 << std::endl;
  J0.r = false;
  TEST("bool ", J0, vil_rgb<bool>(0, 1, 1));

  bool r = true, g = false, b = true;
  vil_rgb<bool> J(r, g, b);
  TEST("bool ", J, vil_rgb<bool>(1, 0, 1));
  std::cout << "vil_rgb<bool> should be [1 0 1], is " << J << std::endl;

  double d = 2.0f;
  std::string type_name = typeid(J).name();
  TEST("vil_rgb<bool> / double ", operator/(J, d), vil_rgb<double>(0.5, 0.0, 0.5));
  std::cout << type_name << " / double should be [0.5 0.0 0.5], is " << operator/(J, d) << std::endl;
  TEST("vil_rgb<bool> * double ", operator*(J, d), vil_rgb<double>(2.0, 0.0, 2.0));
  std::cout << type_name << " * double should be [2.0 0.0 2.0], is " << operator*(J, d) << std::endl;
  d = 3.0f;
  TEST("double * vil_rgb<bool> ", operator*(d, J), vil_rgb<double>(3.0, 0.0, 3.0));
  std::cout << "double * " << type_name << " should be [3.0 0.0 3.0], is " << operator*(d, J) << std::endl;

  vil_rgb<bool> A0(true, false, true);
  vil_rgb<bool> A1(true, true, false);
  TEST("operator != ", operator!=(A0, A1), true);
  TEST("average ", average(A0, A1), vil_rgb<bool>(1, 0, 0));
  std::cout << "The average of vil_rgb<bool> " << A0 << " and vil_rgb<bool> " << A1 << " is: " << average(A0, A1)
            << std::endl;
  TEST("add vil_rgb + vil_rgb ", operator+(A0, A1), vil_rgb<bool>(1, 1, 1));
  TEST("multipy vil_rgb * vil_rgb ", operator*(A0, A1), vil_rgb<bool>(1, 0, 0));

  r = true, g = false, b = true;
  vil_rgb<bool> A(r, g, b);
  TEST("operator+ ", A.operator+(A), vil_rgb<bool>(1, 0, 1));
  A.r = false;
  TEST("operator- ", A.operator-(A), vil_rgb<bool>(1, 1, 1));
  TEST("operator/ ", A.operator/(A), vil_rgb<bool>(0, 0, 0));
  TEST("operator/ ", A.operator/(A) == vil_rgb<bool>(0, 1, 0), false);
  std::cout << "operator/ should be false, is " << (A.operator/(A) == vil_rgb<bool>(false, true, false)) << " since "
            << A << " / " << A << " is false." << std::endl;

  TEST("operator+= ", A.operator+=(A), vil_rgb<bool>(0, 0, 1));
  TEST("operator-= ", A.operator-=(A), vil_rgb<bool>(1, 1, 1));
  TEST("operator-= ", A.operator-=(A) == vil_rgb<bool>(1, 0, 1), false);
  std::cout << "operator-= should be false, is " << (A.operator-=(A) == vil_rgb<bool>(true, false, true)) << std::endl;

  A.r = true, A.g = true, A.b = false;
  TEST("operator* ", A.operator*(true), vil_rgb<bool>(1, 1, 0));
  TEST("operator/ ", A.operator/(true) == vil_rgb<bool>(1, 1, 0), false);
  std::cout << "operator/ should be false, is " << (A.operator/(true) == vil_rgb<bool>(true, true, false)) << std::endl;

  TEST("operator/ ", A.operator/(true), vil_rgb<bool>(0, 0, 0));
  TEST("operator*= ", A.operator*=(true), vil_rgb<bool>(1, 1, 0));
  TEST("operator*= ", A.operator*=(false), vil_rgb<bool>(0, 0, 0));
  A.r = true, A.g = true;
  TEST("operator/= ", A.operator/=(false), vil_rgb<bool>(1, 1, 0));
  TEST("operator/= ", A.operator/=(true), vil_rgb<bool>(0, 0, 0));
  std::cout << "operator/= should be true, is " << (A.operator/=(true) == vil_rgb<bool>(false, false, false))
            << std::endl;
}

static void
test_rgb()
{
  std::cout << "****************************************\n"
            << " Testing vil_rgb<byte>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_byte());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<double>\n"
            << "****************************************\n";
  test_vil_rgb(double());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<float>\n"
            << "****************************************\n";
  test_vil_rgb(float());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<int_16>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_int_16());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<int_32>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_int_32());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<int_64>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_int_64());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<sbyte>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_sbyte());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<uint_16>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_uint_16());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<uint_32>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_uint_32());

  std::cout << "****************************************\n"
            << " Testing vil_rgb<uint_64>\n"
            << "****************************************\n";
  test_vil_rgb(vxl_uint_64());

  test_vil_rgb_bool();
}

TESTMAIN(test_rgb);
