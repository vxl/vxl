//:
// \file
// converted from COOL/test/test_BigNum.C by Peter Vanroose, 25 April 2002.

#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_bignum.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h> // for vnl_math::maxdouble and vnl_math::maxfloat

static void run_constructor_tests() {
  vcl_cout << "\nbignum constructor tests:\n";

  vcl_cout << "long constructor:\n";
  {vnl_bignum b2(0L);
  TEST ("vnl_bignum b2(0L);", (long)b2 == 0L, true);}

  {vnl_bignum b3(1L);
  TEST ("vnl_bignum b3(1L);", (long)b3 == 1L, true);}

  {vnl_bignum b4(-1L);
  TEST ("vnl_bignum b4(-1L);", (long)b4 == -1L, true);}

  {vnl_bignum b5(0x7fffL);
  TEST ("vnl_bignum b5(0x7fffL);", (long)b5 == 0x7fffL, true);}

  {vnl_bignum b6(-0x7fffL);
  TEST ("vnl_bignum b6(-0x7fffL);", (long)b6 == -0x7fffL, true);}

  {vnl_bignum b7(0x7fffffffL);
  TEST ("vnl_bignum b7(0x7fffffffL);", (long)b7 == 0x7fffffffL, true);}

  {vnl_bignum b8(-0x7fffffffL);
  TEST ("vnl_bignum b8(-0x7fffffffL);", (long)b8 == -0x7fffffffL, true);}

  {vnl_bignum b10l(0xf00000L);
  TEST ("vnl_bignum b10l(0xf00000L);", (long)b10l == 0xf00000, true);}

  vcl_cout << "double constructor:\n";
  {vnl_bignum b11(0.0);
  TEST ("vnl_bignum b11(0.0);", (double)b11 == 0.0, true);}

  {vnl_bignum b12(1.0);
  TEST ("vnl_bignum b12(1.0);", (double)b12 == 1.0, true);}

  {vnl_bignum b13(-1.0);
  TEST ("vnl_bignum b13(-1.0);", (double)b13 == -1.0, true);}

  {vnl_bignum b14(vnl_math::maxdouble);
  TEST ("vnl_bignum b14(vnl_math::maxdouble);", (double)b14 == vnl_math::maxdouble, true);}

  {vnl_bignum b15(-vnl_math::maxdouble);
  TEST ("vnl_bignum b15(-vnl_math::maxdouble);", (double)b15 == -vnl_math::maxdouble, true);}

  {vnl_bignum b16(1234567.0);
  TEST ("vnl_bignum b16(1234567.0);", (double)b16 == 1234567.0, true);}

  {vnl_bignum b17(-1234567.0);
  TEST ("vnl_bignum b17(-1234567.0);", (double)b17 == -1234567.0, true);}

  {vnl_bignum b18(1234567000.0);
  TEST ("vnl_bignum b18(1234567000.0);", (double)b18 == 1234567000.0, true);}

  {vnl_bignum b19(-1234567000.0);
  TEST ("vnl_bignum b19(-1234567000.0);", (double)b19 == -1234567000.0, true);}

  {vnl_bignum b20(double(0xf00000)); TEST ("vnl_bignum b20(double(0xf00000));",
                                      (double)b20 == (double)0xf00000, true);}

  vcl_cout << "char* constructor:\n";
  {vnl_bignum b21("-1");
  TEST ("vnl_bignum b21(\"-1\");", (long)b21, -1);}

  {vnl_bignum b22("+1");
  TEST ("vnl_bignum b22(\"+1\");", (long)b22, 1);}

  {vnl_bignum b23("1");
  TEST ("vnl_bignum b23(\"1\");", (long)b23, 1);}

  {vnl_bignum b24("123");
  TEST ("vnl_bignum b24(\"123\");", (long)b24, 123);}

  {vnl_bignum b25("123e12");
  TEST ("vnl_bignum b25(\"123e12\");", (double)b25, 123e12);}

  {vnl_bignum b26("-123e120");
  TEST ("vnl_bignum b26(\"-123e120\");", (double)b26, -123e120);}

  {vnl_bignum b27("0x0");
  TEST ("vnl_bignum b27(\"0x0\");", (long)b27, 0x0);}

  {vnl_bignum b28("0x9");
  TEST ("vnl_bignum b28(\"0x9\");", (long)b28, 0x9);}

  {vnl_bignum b29("0xa");
  TEST ("vnl_bignum b29(\"0xa\");", (long)b29, 0xa);}

  {vnl_bignum b30("0xf");
  TEST ("vnl_bignum b30(\"0xf\");", (long)b30, 0xf);}

  {vnl_bignum b31("0xa");
  TEST ("vnl_bignum b31(\"0xa\");", (long)b31, 0xa);}

  {vnl_bignum b32("0xf");
  TEST ("vnl_bignum b32(\"0xf\");", (long)b32, 0xf);}

  {vnl_bignum b33("0x1af");
  TEST ("vnl_bignum b33(\"0x1af\");", (long)b33, 0x1af);}

  {vnl_bignum b34("0");
  TEST ("vnl_bignum b34(\"0\");", (long)b34, 0);}

  {vnl_bignum b35("00");
  TEST ("vnl_bignum b35(\"00\");", (long)b35, 0);}

  {vnl_bignum b36("012334567");
  TEST ("vnl_bignum b36(\"012334567\");", (long)b36, 012334567);}

  {vnl_bignum b37("9");
  TEST ("vnl_bignum b37(\"9\");", (long)b37, 9);}
#if 0
  vnl_bignum b38("09");
  TEST ("vnl_bignum b38(\"09\");", (long)b38, 9);

  vnl_bignum b39(" 9");
  TEST ("vnl_bignum b39(\" 9\");", (long)b39, 9);
#endif
  {vnl_bignum b40("0xf");
  TEST ("vnl_bignum b40(\"0xf\");", (long)b40, 0xf);}

  vcl_cout << "vnl_bignum& constructor:\n";
  {vnl_bignum b50(vnl_bignum(0L));
  TEST ("vnl_bignum b50(vnl_bignum(0L));", (long)b50, 0L);}

  {vnl_bignum b51(vnl_bignum(100L));
  TEST ("vnl_bignum b51(vnl_bignum(100L));", (long)b51, 100L);}
}

static void run_conversion_operator_tests() {
  vcl_cout << "\nConversion operator tests:\n";

  vcl_cout << "short conversion operator:\n";
  TEST("short(vnl_bignum(0L)) == 0", short(vnl_bignum(0L)), 0);
  TEST("short(vnl_bignum(0x7fffL)) == 0x7fff", short(vnl_bignum(0x7fffL)), 0x7fff);
  TEST("short(vnl_bignum(-0x7fffL)) == -0x7fff", short(vnl_bignum(-0x7fffL)), -0x7fff);
  TEST("short(vnl_bignum(-0x8000L)) == short(-0x8000)", short(vnl_bignum(-0x8000L)), short(-0x8000));

  vcl_cout << "int conversion operator:\n";
  TEST("int(vnl_bignum(0L)) == 0", int(vnl_bignum(0L)), 0);
  TEST("int(vnl_bignum(0x7fffffffL)) == 0x7fffffff", int(vnl_bignum(0x7fffffffL)), 0x7fffffff);
  TEST("int(vnl_bignum(-0x7fffffffL)) == -0x7fffffff", int(vnl_bignum(-0x7fffffffL)), -0x7fffffff);

  vnl_bignum b(0x7fffffffL);
  ++b;
  TEST("vnl_bignum b(0x7fffffffL); ++b; (long)b == 0x80000000L", (long)b, 0x80000000L);
  --b;
  TEST("vnl_bignum b(0x80000000L); --b; (long)b == 0x7fffffffL", (long)b, 0x7fffffffL);

  ++b; b = -b;
  TEST("vnl_bignum b(0x7fffffffL); ++b; b=-b; (long)b == -0x80000000L", (long)b, -0x80000000L);

  vcl_cout << "float conversion operator:\n";
  TEST("float(vnl_bignum(0.0)) == 0.0", ((float) vnl_bignum(0.0)), 0.0);
  TEST("float(vnl_bignum(99999.0)) == 99999.0",
       ((float) vnl_bignum(99999.0)), 99999.0);
  TEST("float(vnl_bignum(vnl_math::maxfloat)) == vnl_math::maxfloat",
       (vnl_math::maxfloat), (float) vnl_bignum(vnl_math::maxfloat));
  TEST("float(vnl_bignum(-vnl_math::maxfloat)) == -vnl_math::maxfloat",
       (-vnl_math::maxfloat), float(vnl_bignum(-vnl_math::maxfloat)));
  b = vnl_math::maxdouble;
  ++b;

  vcl_cout << "double conversion operator:\n";
  TEST("double(vnl_bignum(0.0)) == 0.0", ((double) vnl_bignum(0.0)), 0.0);
  TEST("double(vnl_bignum(99999.0)) == 99999.0",
       (double) vnl_bignum(99999.0), 99999.0);
  TEST("double(vnl_bignum(1e300)) == 1e300",
       double(vnl_bignum(1e300)), 1e300);
  TEST("double(vnl_bignum(-1e300)) == -1e300",
       double(vnl_bignum(-1e300)), -1e300);
  TEST("double(vnl_bignum(vnl_math::maxfloat)) == vnl_math::maxfloat",
       (vnl_math::maxfloat), (double) vnl_bignum(vnl_math::maxfloat));
  TEST("double(vnl_bignum(-vnl_math::maxfloat)) == -vnl_math::maxfloat",
       (-vnl_math::maxfloat), double(vnl_bignum(-vnl_math::maxfloat)));
  TEST("double(vnl_bignum(vnl_math::maxdouble)) == vnl_math::maxdouble",
       (double) vnl_bignum(vnl_math::maxdouble), vnl_math::maxdouble);
  TEST("double(vnl_bignum(-vnl_math::maxdouble)) == -vnl_math::maxdouble",
       (double) vnl_bignum(-vnl_math::maxdouble), -vnl_math::maxdouble);
}

static void run_assignment_tests() {
  vcl_cout << "\nStarting assignment tests:\n";
  vnl_bignum b1;

  TEST_RUN ("vnl_bignum b1; b1 = 0xffff;", b1 = 0xffffL, long(b1), 0xffffL);

  // double assignment operator
  TEST_RUN ("double(b1) == -1.23e6", b1 = -1.23e6, double(b1), -1.23e6);

  // long assignment operator
  TEST_RUN ("long(b1) = -0x7fffffff", b1 = -0x7fffffffL, long(b1), -0x7fffffff);

  // char * assignment operator
  TEST_RUN ("long(b1) = 0x1fffffL", b1 = "0x1fffff", long(b1), 0x1fffffL);


  // vnl_bignum& assignment operator
  b1 = "0";
  vnl_bignum b5(0x1ffffL);
  TEST_RUN ("b1 = b5", b1 = b5, b1, b5);
}

static void run_logical_comparison_tests() {
  vcl_cout << "\nStarting logical comparison tests:\n";
  vnl_bignum b0(0L);
  vnl_bignum b1(1L);
  vnl_bignum b2(0x7fffL);
  vnl_bignum b3(-0x7fffL);

  TEST("b0 == b0", b0 == b0, true);
  TEST("b0 == b1", b0 == b1, false);
  TEST("b0 == b2", b0 == b2, false);
  TEST("b0 == b3", b0 == b3, false);
  TEST("b1 == b2", b1 == b2, false);
  TEST("b1 == b3", b1 == b3, false);
  TEST("b2 == b3", b2 == b3, false);

  TEST("b0 != b0", b0 != b0, false);
  TEST("b0 != b1", b0 != b1, true);
  TEST("b0 != b2", b0 != b2, true);
  TEST("b0 != b3", b0 != b3, true);
  TEST("b1 != b2", b1 != b2, true);
  TEST("b1 != b3", b1 != b3, true);
  TEST("b2 != b3", b2 != b3, true);

  TEST("b0 < b0", b0 < b0, false);
  TEST("b0 < b1", b0 < b1, true);
  TEST("b0 < b2", b0 < b2, true);
  TEST("b0 < b3", b0 < b3, false);
  TEST("b1 < b2", b1 < b2, true);
  TEST("b1 < b3", b1 < b3, false);
  TEST("b2 < b3", b2 < b3, false);

  TEST("b0 > b0", b0 > b0, false);
  TEST("b0 > b1", b0 > b1, false);
  TEST("b0 > b2", b0 > b2, false);
  TEST("b0 > b3", b0 > b3, true);
  TEST("b1 > b2", b1 > b2, false);
  TEST("b1 > b3", b1 > b3, true);
  TEST("b2 > b3", b2 > b3, true);

  TEST ("b3 != b2", b3 != b2, true);
  TEST ("b3 != b3", b3 != b3, false);
  TEST ("b3 < b2", b3 < b2, true);
  TEST ("b3 <= b2", b3 <= b2, true);
  TEST ("b3 <= b3", b3 <= b3, true);
  TEST ("b3 > b3", b3 > b3, false);
  TEST ("b3 > b2", b3 > b2, false);
  TEST ("b3 >= b2", b3 >= b2, false);
  TEST ("b3 >= b3", b3 >= b3, true);
  TEST ("b2 >= b2", b2 >= b2, true);
  vcl_cout << b2 << " == " << &b2 << vcl_endl;
  TEST ("<<", 1, 1);
}

static void run_division_tests() {
  vcl_cout << "\nStarting division tests:\n";

  TEST("long(vnl_bignum(0L)/vnl_bignum(1L))", long(vnl_bignum(0L)/vnl_bignum(1L)), 0);
  TEST("long(vnl_bignum(-1L)/vnl_bignum(1L))", long(vnl_bignum(-1L)/vnl_bignum(1L)), -1);

  vnl_bignum b1,b2,b3;
  long i,j,k,l;
  long div_errors = 0;
  long mod_errors = 0;

  vcl_cout << " for (i = 0xffffff; i > 0; i /= 0x10) \n";
  vcl_cout << "   for (j = 0x7ffffff; j > 0; j /= 0x10) \n";
  vcl_cout << "     for (k = 1; k < 17; ++k) \n";
  vcl_cout << "       for (l = 1; l < 17; ++l) \n         ";
  for (i = 0xffffff; i > 0; i /= 0x10) {
    vcl_cout.put('.');
    vcl_cout.flush();
    for (j = 0x7ffffff; j > 0; j /= 0x10) {
      for (k = 1; k < 17; ++k) {
        for (l = 1; l < 17; ++l) {
          b1 = vnl_bignum(i+k);
          b2 = vnl_bignum(j+l);
          b3 = vnl_bignum(long((i+k)/(j+l)));
          if (b1/b2 != b3) {
            TEST("(vnl_bignum(i+k)/vnl_bignum(j+l)) == vnl_bignum(long((i+k)/(j+l)))", false, true);
            vcl_cout<<vcl_hex<< "i="<<i<<", j="<<j<<", k="<<k<<", l="<<l<<"\n";
            ++div_errors;
          }
          b3 = vnl_bignum(long((i+k)%(j+l)));
          if (b1%b2 != b3) {
            TEST("(vnl_bignum(i+k)%vnl_bignum(j+l)) == vnl_bignum(long((i+k)%(j+l)))", false, true);
            vcl_cout<<vcl_hex<< "i="<<i<<", j="<<j<<", k="<<k<<", l="<<l<<"\n";
            ++mod_errors;
          }
        }
      }
    }
  }

  vcl_cout << "\n";
  TEST("(vnl_bignum(i+k)/vnl_bignum(j+l)) == vnl_bignum(long((i+k)/(j+l)))",
       div_errors, 0);
  TEST("(vnl_bignum(i+k)%vnl_bignum(j+l)) == vnl_bignum(long((i+k)%(j+l)))",
       mod_errors, 0);

#ifdef INTERACTIVE
  char num[130], den[130];
  vnl_bignum b,r;

  while (1) {
    vcl_cout << "Enter next numerator:  ";
    vcl_cin >> num;
    vcl_cout << "Enter next denominator:  ";
    vcl_cin >> den;

    b = vnl_bignum(num)/vnl_bignum(den);
    r = vnl_bignum(num) % vnl_bignum(den);
    vcl_cout << "\nquotient  = " << b << "\n";
    vcl_cout <<   "            "; b.dump(); vcl_cout << "\n";
    vcl_cout << "\nremainder = " << r << "\n";
    vcl_cout <<   "            "; r.dump(); vcl_cout << "\n";
  }
#endif
}

static void run_multiplication_division_tests() {
  vcl_cout << "\nCheck example in book:\n";

  vnl_bignum b2 = "0xffffffff";                 // Create vnl_bignum object
  vnl_bignum b3 = "12345e30";                   // Create vnl_bignum object

  vcl_cout << "b2 = " << b2 << vcl_endl;
  vcl_cout << "b3 = " << b3 << vcl_endl;

  TEST ("(b2*b3) / b3 = b2", ((b2*b3) / b3 == b2), 1);
  TEST ("(b2*b3) / b2 = b3", ((b2*b3) / b2 == b3), 1);
  TEST ("((b3/b2) * b2) + (b3%b2) = b3", (((b3/b2) * b2) + (b3%b2) == b3), 1);
}

static void run_addition_subtraction_tests() {
  vcl_cout << "\nStarting addition, subtraction tests:\n";

  long i,j;
  long add_errors = 0;
  long sub_errors = 0;
  vnl_bignum bi,bj,bij;

  vcl_cout << " for (i = 1; i < 0xfffffff;  i *= 3) \n";
  vcl_cout << "   for (j = 1; j < 0xfffffff; j *= 3) \n      ";

  {for (i = 1; i < 0xfffffff;  i *= 3) {
    vcl_cout.put('.');
    vcl_cout.flush();
    for (j = 1; j < 0xfffffff; j *= 3) {
      bi = i;
      bj = j;
      bij = vnl_bignum(i+j);
      if (bi + bj != bij) {
        TEST("bi + bj == vnl_bignum(i + j)", false, true);
        vcl_cout << "i = "<<i<<", j = "<<j<<"\n";
        ++add_errors;
      }
      bij = vnl_bignum(i-j);
      if (bi - bj != bij) {
        TEST("bi - bj == vnl_bignum(i - j)", false, true);
        vcl_cout << "i = "<<i<<", j = "<<j<<"\n";
        ++sub_errors;
      }
    }
  }}
  vcl_cout << "\n";
  TEST("bi + bj == vnl_bignum(i + j)", add_errors, 0);
  TEST("bi - bj == vnl_bignum(i - j)", sub_errors, 0);

  vnl_bignum b0(0L);
  vnl_bignum zillion("1000000000000000000");
  vnl_bignum b1000(1000L), b1000000(1000000L);

  TEST("-b0 == b0", -b0, b0);
  TEST("-b1000 == vnl_bignum(-1L)*b1000", -b1000, vnl_bignum(-1L)*b1000);
  TEST("-(-b1000000) == b1000000", (-(-b1000000) == b1000000), true);
  TEST("b0 + b1000 == b1000", (b0 + b1000 == b1000), true);
  TEST("b0 + b1000000 == b1000000", (b0 + b1000000 == b1000000), true);
  TEST("b1000 + b0 == b1000", (b1000 + b0 == b1000), true);
  TEST("b1000000 + b0 == b1000000", (b1000000 + b0 == b1000000), true);
  TEST("b0 + (-b1000) == -b1000", (b0 + (-b1000) == -b1000), true);
  TEST("-b1000 + b0 == -b1000", (-b1000 + b0 == -b1000), true);
  TEST("-b1000 + (-b1000) == (vnl_bignum(2L)*(-b1000))",
       (-b1000 + (-b1000) == (vnl_bignum(2L)*(-b1000))), true);
  TEST("-b1000000 + (-b1000000) == (vnl_bignum(2L)*(-b1000000))",
       (-b1000000 + (-b1000000) == (vnl_bignum(2L)*(-b1000000))), true);
  TEST("b1000000 + (-b1000000) == b0", (b1000000 + (-b1000000) == b0), true);
  TEST("zillion + (-zillion) == b0", (zillion + (-zillion) == b0), true);
  TEST("zillion + b1000 == b1000000*b1000000*b1000000 + b1000",
       (zillion + b1000 == b1000000*b1000000*b1000000 + b1000), true);

  TEST("-b0 == b0", (-b0 == b0), true);
  TEST("-b1000 == vnl_bignum(-1L)*b1000", (-b1000 == vnl_bignum(-1L)*b1000), true);
  TEST("-(-b1000000) == b1000000", (-(-b1000000) == b1000000), true);

  TEST("b0 - b1000 == -b1000", (b0 - b1000 == -b1000), true);
  TEST("b0 - b1000000 == -b1000000", (b0 -b1000000 == -b1000000), true);
  TEST("b1000 - b0 == b1000", (b1000 - b0 == b1000), true);
  TEST("b1000000 - b0 == b1000000", (b1000000 - b0 == b1000000), true);
  TEST("b0 - (-b1000) == b1000", (b0 - (-b1000) == b1000), true);
  TEST("-b1000 - b0 == -b1000", (-b1000 - b0 == -b1000), true);
  TEST("-b1000 - (-b1000) == b0", (-b1000 - (-b1000) == b0), true);
  TEST("-b1000 - (-zillion) == zillion - b1000",
       -b1000 - (-zillion), zillion - b1000);
  TEST("-b1000000 - (-b1000000) == b0", -b1000000 - (-b1000000), b0);
  TEST("-b1000000 - (b1000000) == -vnl_bignum(2L)*b1000000",
       -b1000000 - (b1000000), -vnl_bignum(2L)*b1000000);
  TEST("b1000000 - (-b1000000) == vnl_bignum(2L)*b1000000",
       b1000000 - (-b1000000), vnl_bignum(2L)*b1000000);
  TEST("zillion - (-zillion) == vnl_bignum(2L)*zillion",
       zillion - (-zillion), vnl_bignum(2L)*zillion);
  TEST("zillion - b1000 == b1000000*b1000000*b1000000 - b1000",
       zillion - b1000, b1000000*b1000000*b1000000 - b1000);
  TEST("-zillion - b1000 == -b1000000*b1000000*b1000000 - b1000",
       -zillion - b1000, -b1000000*b1000000*b1000000 - b1000);

  // example in book
  vnl_bignum b2 = "0xffffffff";                 // Create vnl_bignum object
  vnl_bignum b3 = "12345e30";                   // Create vnl_bignum object
  TEST ("(b2+b3) - b2 = b3", (b2+b3) - b2 == b3, 1);
  TEST ("(b2+b3) - b3 = b2", (b2+b3) - b3 == b2, 1);
  b3.dump();
  TEST ("b3.dump()", 1, 1);
}

static void run_multiplication_tests() {
  vcl_cout << "\nStarting multiplication tests:\n";

  vnl_bignum b0(0L), b1000(1000L), b1000000(1000000L),
  zillion("1000000000000000000");


  TEST("b0*b0 == b0", (b0*b0 == b0), true);
  TEST("b0*b1000 == b0", (b0*b1000 == b0), true);
  TEST("b1000*b0 == b0", (b1000*b0 == b0), true);
  TEST("b1000*b1000 == b1000000", (b1000*b1000 == b1000000), true);
  TEST("b1000*b1000000 == b1000000*b1000",
       (b1000*b1000000 == b1000000*b1000), true);
  TEST("-b1000000*b1000000*b1000000 == -zillion",
       (-b1000000*b1000000*b1000000 == -zillion), true);
  TEST("zillion*-b1000 == b1000*-zillion",
       (zillion*-b1000 == b1000*-zillion), true);
}

static void run_left_shift_tests() {

  vnl_bignum b1(1L);

  // left shift
  TEST("int(b1) == 1",int(b1) == 1,true);
  TEST("long(b1 << 1) == 2",long(b1 << 1) == 2,true);
  TEST("long(b1 << 2) == 4",long(b1 << 2) == 4,true);
  TEST("long(b1 << 3) == 8",long(b1 << 3) == 8,true);
  TEST("long(b1 << 4) == 16",long(b1 << 4) == 16,true);
  TEST("long(b1 << 5) == 32",long(b1 << 5) == 32,true);
  TEST("long(b1 << 6) == 64",long(b1 << 6) == 64,true);
  TEST("long(b1 << 7) == 128",long(b1 << 7) == 128,true);
  TEST("long(b1 << 8) == 256",long(b1 << 8) == 256,true);
  TEST("long(b1 << 9) == 512",long(b1 << 9) == 512,true);
  TEST("long(b1 << 10) == 1024",long(b1 << 10) == 1024,true);
  TEST("long(b1 << 11) == 2048",long(b1 << 11) == 2048,true);
  TEST("long(b1 << 12) == 4096",long(b1 << 12) == 4096,true);
  TEST("long(b1 << 13) == 8192",long(b1 << 13) == 8192,true);
  TEST("long(b1 << 14) == 16384",long(b1 << 14) == 16384,true);
  TEST("long(b1 << 15) == 32768",long(b1 << 15) == 32768,true);
  TEST("long(b1 << 16) == 65536",long(b1 << 16) == 65536,true);
  TEST("(b1 << 32) == vnl_bignum(\"4294967296\")",
       (b1 << 32) == vnl_bignum("4294967296"),true);
  TEST("long(-b1 << 1) == -2",long(-b1 << 1) == -2,true);
  TEST("long(-b1 << 2) == -4",long(-b1 << 2) == -4,true);
  TEST("long(-b1 << 3) == -8",long(-b1 << 3) == -8,true);
  TEST("long(-b1 << 4) == -16",long(-b1 << 4) == -16,true);
  TEST("long(-b1 << 5) == -32",long(-b1 << 5) == -32,true);
  TEST("long(-b1 << 6) == -64",long(-b1 << 6) == -64,true);
  TEST("long(-b1 << 7) == -128",long(-b1 << 7) == -128,true);
  TEST("long(-b1 << 8) == -256",long(-b1 << 8) == -256,true);
  TEST("long(-b1 << 9) == -512",long(-b1 << 9) == -512,true);
  TEST("long(-b1 << 10) == -1024",long(-b1 << 10) == -1024,true);
  TEST("long(-b1 << 11) == -2048",long(-b1 << 11) == -2048,true);
  TEST("long(-b1 << 12) == -4096",long(-b1 << 12) == -4096,true);
  TEST("long(-b1 << 13) == -8192",long(-b1 << 13) == -8192,true);
  TEST("long(-b1 << 14) == -16384",long(-b1 << 14) == -16384,true);
  TEST("long(-b1 << 15) == -32768",long(-b1 << 15) == -32768,true);
  TEST("long(-b1 << 16) == -65536",long(-b1 << 16) == -65536,true);
  TEST("(-b1 << 32) == vnl_bignum(\"-4294967296\")",
       (-b1 << 32) == vnl_bignum("-4294967296"),true);
  TEST("long(b1 << -16) == 0",long(b1 << -16) == 0,true);
  TEST("long(-b1 << -16) == 0",long(-b1 << -16) == 0,true);
}

static void run_right_shift_tests() {
  // right shift
  vnl_bignum b2("4294967296");
  TEST("b2 == vnl_bignum(\"4294967296\")",b2 == vnl_bignum("4294967296"), true);
  TEST("(b2 >> 1) == vnl_bignum(\"2147483648\")",
       (b2 >> 1) == vnl_bignum("2147483648"),true);
  TEST("long(b2 >> 2) == 1073741824",long(b2 >> 2) == 1073741824L,true);
  TEST("long(b2 >> 3) == 536870912",long(b2 >> 3) == 536870912L,true);
  TEST("long(b2 >> 4) == 268435456",long(b2 >> 4) == 268435456L,true);
  TEST("long(b2 >> 5) == 134217728",long(b2 >> 5) == 134217728L,true);
  TEST("long(b2 >> 6) == 67108864",long(b2 >> 6) == 67108864L,true);
  TEST("long(b2 >> 7) == 33554432",long(b2 >> 7) == 33554432L,true);
  TEST("long(b2 >> 8) == 16777216",long(b2 >> 8) == 16777216L,true);
  TEST("long(b2 >> 9) == 8388608",long(b2 >> 9) == 8388608L,true);
  TEST("long(b2 >> 10) == 4194304",long(b2 >> 10) == 4194304L,true);
  TEST("long(b2 >> 11) == 2097152",long(b2 >> 11) == 2097152L,true);
  TEST("long(b2 >> 12) == 1048576",long(b2 >> 12) == 1048576L,true);
  TEST("long(b2 >> 13) == 524288",long(b2 >> 13) == 524288L,true);
  TEST("long(b2 >> 14) == 262144",long(b2 >> 14) == 262144L,true);
  TEST("long(b2 >> 15) == 131072",long(b2 >> 15) == 131072L,true);
  TEST("long(b2 >> 16) == 65536",long(b2 >> 16) == 65536L,true);
  TEST("long(b2 >> 32) == 1",long(b2 >> 32) == 1L,true);
  TEST("long(b2 >> 33) == 0",long(b2 >> 33) == 0L,true);
  TEST("long(b2 >> 67) == 0",long(b2 >> 67) == 0L,true);
  TEST("(-b2 >> 1) == vnl_bignum(\"-2147483648\")",
       (-b2 >> 1) == vnl_bignum("-2147483648"),true);
  TEST("long(-b2 >> 2) == -1073741824",long(-b2 >> 2) == -1073741824L,true);
  TEST("long(-b2 >> 3) == -536870912",long(-b2 >> 3) == -536870912L,true);
  TEST("long(-b2 >> 4) == -268435456",long(-b2 >> 4) == -268435456L,true);
  TEST("long(-b2 >> 5) == -134217728",long(-b2 >> 5) == -134217728L,true);
  TEST("long(-b2 >> 6) == -67108864",long(-b2 >> 6) == -67108864L,true);
  TEST("long(-b2 >> 7) == -33554432",long(-b2 >> 7) == -33554432L,true);
  TEST("long(-b2 >> 8) == -16777216",long(-b2 >> 8) == -16777216L,true);
  TEST("long(-b2 >> 9) == -8388608",long(-b2 >> 9) == -8388608L,true);
  TEST("long(-b2 >> 10) == -4194304",long(-b2 >> 10) == -4194304L,true);
  TEST("long(-b2 >> 11) == -2097152",long(-b2 >> 11) == -2097152L,true);
  TEST("long(-b2 >> 12) == -1048576",long(-b2 >> 12) == -1048576L,true);
  TEST("long(-b2 >> 13) == -524288",long(-b2 >> 13) == -524288L,true);
  TEST("long(-b2 >> 14) == -262144",long(-b2 >> 14) == -262144L,true);
  TEST("long(-b2 >> 15) == -131072",long(-b2 >> 15) == -131072L,true);
  TEST("long(-b2 >> 16) == -65536",long(-b2 >> 16) == -65536L,true);
  TEST("long(-b2 >> 32) == -1",long(-b2 >> 32) == -1,true);
  TEST("long(-b2 >> 33) == -0",long(-b2 >> 33) == 0,true);
  TEST("long(-b2 >> 67) == -0",long(-b2 >> 67) == 0,true);
}

static void run_shift_tests() {
  vcl_cout << "\nStarting shift tests:\n";

  run_left_shift_tests();
  run_right_shift_tests();
#ifdef INTERACTIVE
  vnl_bignum b;
  char s[100];
  int sh;

  while (1) {
    vcl_cout << "Enter next vnl_bignum:  ";
    vcl_cin >> s;
    b = s;
    vcl_cout << "Enter shift amount: ";
    vcl_cin >> sh;
    vcl_cout << "Shift == " << sh << "\n";

    b = b << sh;
  }
#endif
}

void test_bignum() {
  run_constructor_tests();
  run_conversion_operator_tests();
  run_assignment_tests();
  run_addition_subtraction_tests();
  run_multiplication_tests();
  run_division_tests();
  run_multiplication_division_tests();
  run_shift_tests();
  run_logical_comparison_tests();
}

TESTMAIN(test_bignum);
