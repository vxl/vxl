//:
// \file
// converted from COOL/test/test_BigNum.C by Peter Vanroose, 25 April 2002.

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_bignum.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h> // for vnl_math::maxdouble and vnl_math::maxfloat

static void run_constructor_tests() {
  vcl_cout << "\nbignum constructor tests:\n";

  vcl_cout << "long constructor:\n";
  {vnl_bignum b(0L); TEST("vnl_bignum b(0L);", b, 0L);}
  {vnl_bignum b(1L); TEST("vnl_bignum b(1L);", b, 1L);}
  {vnl_bignum b(-1L); TEST("vnl_bignum b(-1L);", b, -1L);}
  {vnl_bignum b(0x7fffL); TEST("vnl_bignum b(0x7fffL);", b, 0x7fffL);}
  {vnl_bignum b(-0x7fffL); TEST("vnl_bignum b(-0x7fffL);", b, -0x7fffL);}
  {vnl_bignum b(0x7fffffffL); TEST("vnl_bignum b(0x7fffffffL);", b, 0x7fffffffL);}
  {vnl_bignum b(-0x7fffffffL); TEST("vnl_bignum b(-0x7fffffffL);", b, -0x7fffffffL);}
  {vnl_bignum b(0xf00000L); TEST("vnl_bignum b(0xf00000L);", b, 0xf00000);}

  vcl_cout << "double constructor:\n";
  {vnl_bignum b(0.0); TEST("vnl_bignum b(0.0);", (double)b, 0.0);}
  {vnl_bignum b(1.0); TEST("vnl_bignum b(1.0);", (double)b, 1.0);}
  {vnl_bignum b(-1.0); TEST("vnl_bignum b(-1.0);", (double)b, -1.0);}
  {vnl_bignum b(vnl_math::maxdouble);
   TEST("vnl_bignum b(vnl_math::maxdouble);", (double)b, vnl_math::maxdouble);}
  {vnl_bignum b(-vnl_math::maxdouble);
   TEST("vnl_bignum b(-vnl_math::maxdouble);", (double)b, -vnl_math::maxdouble);}
  {vnl_bignum b(1234567.0); TEST("vnl_bignum b(1234567.0);", (double)b, 1234567.0);}
  {vnl_bignum b(-1234567.0); TEST("vnl_bignum b(-1234567.0);", (double)b, -1234567.0);}
  {vnl_bignum b(1234567e3); TEST("vnl_bignum b(1234567e3);", (double)b, 1234567e3);}
  {vnl_bignum b(-1234567e3); TEST("vnl_bignum b(-1234567e3);", (double)b, -1234567e3);}
  {vnl_bignum b(double(0xf00000)); TEST("vnl_bignum b(double(0xf00000));", b, 0xf00000);}

  vcl_cout << "char* constructor:\n";
  {vnl_bignum b("-1"); TEST("vnl_bignum b(\"-1\");", b, -1L);}
  {vnl_bignum b("+1"); TEST("vnl_bignum b(\"+1\");", b, 1L);}
  {vnl_bignum b("1"); TEST("vnl_bignum b(\"1\");", b, 1L);}
  {vnl_bignum b("123"); TEST("vnl_bignum b(\"123\");", b, 123L);}
  {vnl_bignum b("123e5"); TEST("vnl_bignum b(\"123e5\");", b, 12300000L);}
  {vnl_bignum b("123e+4"); TEST("vnl_bignum b(\"123e+4\");", b, 1230000L);}
  {vnl_bignum b("123e12"); TEST("vnl_bignum b(\"123e12\");", (double)b, 123e12);}
  {vnl_bignum b("-1e120"); vcl_stringstream s; s << b;
   // verify that b outputs as  "-1000...00" (120 zeros)
   bool t = s.str()[0] == '-' && s.str()[1] == '1' && s.str()[122] == '\0'; 
   for (int i=0; i<120; ++i) t = t && s.str()[i+2] == '0';
   TEST("vnl_bignum b(\"-1e120\") outputs as \"-10000...00\"", t, true);}
  {vnl_bignum b("0x0"); TEST("vnl_bignum b(\"0x0\");", b, 0x0);}
  {vnl_bignum b("0x9"); TEST("vnl_bignum b(\"0x9\");", b, 0x9);}
  {vnl_bignum b("0xa"); TEST("vnl_bignum b(\"0xa\");", b, 0xa);}
  {vnl_bignum b("0xf"); TEST("vnl_bignum b(\"0xf\");", b, 0xf);}
  {vnl_bignum b("0xA"); TEST("vnl_bignum b(\"0xA\");", b, 0xa);}
  {vnl_bignum b("0xF"); TEST("vnl_bignum b(\"0xF\");", b, 0xf);}
  {vnl_bignum b("0x1aF"); TEST("vnl_bignum b(\"0x1aF\");", b, 0x1af);}
  {vnl_bignum b("0"); TEST("vnl_bignum b(\"0\");", b, 0);}
  {vnl_bignum b("00"); TEST("vnl_bignum b(\"00\");", b, 0);}
  {vnl_bignum b("012334567"); TEST("vnl_bignum b(\"012334567\");", b, 012334567);}
  {vnl_bignum b("9"); TEST("vnl_bignum b(\"9\");", b, 9);}
  {vnl_bignum b(" 9"); TEST("vnl_bignum b(\" 9\");", b, 9);}

  vcl_cout << "reading from istream:\n";
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "+1"; is >> b; TEST("\"+1\" >> b;", b, 1L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "-1"; is >> b; TEST("\"-1\" >> b;", b, -1L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "123"; is >> b; TEST("\"123\" >> b;", b, 123L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "123e5"; is >> b; TEST("\"123e5\" >> b;", b, 12300000L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "123e+4"; is >> b; TEST("\"123e+4\" >> b;", b, 1230000L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0x0"; is >> b; TEST("\"0x0\" >> b;", b, 0x0);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0x9"; is >> b; TEST("\"0x9\" >> b;", b, 0x9);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0xa"; is >> b; TEST("\"0xa\" >> b;", b, 0xa);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0xf"; is >> b; TEST("\"0xf\" >> b;", b, 0xf);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0xA"; is >> b; TEST("\"0xA\" >> b;", b, 0xa);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0xF"; is >> b; TEST("\"0xF\" >> b;", b, 0xf);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0x1aF"; is >> b; TEST("\"0x1aF\" >> b;", b, 0x1af);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "0"; is >> b; TEST("\"0\" >> b;", b, 0L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "00"; is >> b; TEST("\"00\" >> b;", b, 0L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "012334567"; is >> b; TEST("\"012334567\" >> b;", b, 012334567);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << "9"; is >> b; TEST("\"9\" >> b;", b, 9L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_bignum b;
   is << " 9"; is >> b; TEST("\" 9\" >> b;", b, 9L);}

  vcl_cout << "vnl_bignum& constructor:\n";
  {vnl_bignum b50(vnl_bignum(0L));
  TEST("vnl_bignum b50(vnl_bignum(0L));", (long)b50, 0L);}

  {vnl_bignum b51(vnl_bignum(100L));
  TEST("vnl_bignum b51(vnl_bignum(100L));", (long)b51, 100L);}
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

  TEST("b3 != b2", b3 != b2, true);
  TEST("b3 != b3", b3 != b3, false);
  TEST("b3 < b2", b3 < b2, true);
  TEST("b3 <= b2", b3 <= b2, true);
  TEST("b3 <= b3", b3 <= b3, true);
  TEST("b3 > b3", b3 > b3, false);
  TEST("b3 > b2", b3 > b2, false);
  TEST("b3 >= b2", b3 >= b2, false);
  TEST("b3 >= b3", b3 >= b3, true);
  TEST("b2 >= b2", b2 >= b2, true);
  vcl_cout << b2 << " == " << &b2 << vcl_endl;
  TEST("<<", 1, 1);
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

  TEST("(b2*b3) / b3 = b2", ((b2*b3) / b3 == b2), 1);
  TEST("(b2*b3) / b2 = b3", ((b2*b3) / b2 == b3), 1);
  TEST("((b3/b2) * b2) + (b3%b2) = b3", (((b3/b2) * b2) + (b3%b2) == b3), 1);
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
  TEST("-(-b1000000) == b1000000", -(-b1000000), b1000000);
  TEST("b0 + b1000 == b1000", b0 + b1000, b1000);
  TEST("b0 + b1000000 == b1000000", b0 + b1000000, b1000000);
  TEST("b1000 + b0 == b1000", b1000 + b0, b1000);
  TEST("b1000000 + b0 == b1000000", b1000000 + b0, b1000000);
  TEST("b0 + (-b1000) == -b1000", b0 + (-b1000), -b1000);
  TEST("-b1000 + b0 == -b1000", -b1000 + b0, -b1000);
  TEST("-b1000 + (-b1000) == (vnl_bignum(2L)*(-b1000))",
        -b1000 + (-b1000),   (vnl_bignum(2L)*(-b1000)));
  TEST("-b1000000 + (-b1000000) == (vnl_bignum(2L)*(-b1000000))",
        -b1000000 + (-b1000000), (vnl_bignum(2L)*(-b1000000)));
  TEST("b1000000 + (-b1000000) == b0", b1000000 + (-b1000000), b0);
  TEST("zillion + (-zillion) == b0", zillion + (-zillion), b0);
  TEST("zillion + b1000 == b1000000*b1000000*b1000000 + b1000",
        zillion + b1000,   b1000000*b1000000*b1000000 + b1000);

  TEST("-b0 == b0", -b0, b0);
  TEST("-b1000 == vnl_bignum(-1L)*b1000", -b1000, vnl_bignum(-1L)*b1000);
  TEST("-(-b1000000) == b1000000", -(-b1000000), b1000000);

  TEST("b0 - b1000 == -b1000", b0 - b1000, -b1000);
  TEST("b0 - b1000000 == -b1000000", b0 -b1000000, -b1000000);
  TEST("b1000 - b0 == b1000", b1000 - b0, b1000);
  TEST("b1000000 - b0 == b1000000", b1000000 - b0, b1000000);
  TEST("b0 - (-b1000) == b1000", b0 - (-b1000), b1000);
  TEST("-b1000 - b0 == -b1000", -b1000 - b0, -b1000);
  TEST("-b1000 - (-b1000) == b0", -b1000 - (-b1000), b0);
  TEST("-b1000 - (-zillion) == zillion - b1000",
        -b1000 - (-zillion),   zillion - b1000);
  TEST("-b1000000 - (-b1000000) == b0", -b1000000 - (-b1000000), b0);
  TEST("-b1000000 - (b1000000) == -vnl_bignum(2L)*b1000000",
        -b1000000 - (b1000000),   -vnl_bignum(2L)*b1000000);
  TEST("b1000000 - (-b1000000) == vnl_bignum(2L)*b1000000",
        b1000000 - (-b1000000),   vnl_bignum(2L)*b1000000);
  TEST("zillion - (-zillion) == vnl_bignum(2L)*zillion",
        zillion - (-zillion),   vnl_bignum(2L)*zillion);
  TEST("zillion - b1000 == b1000000*b1000000*b1000000 - b1000",
        zillion - b1000,   b1000000*b1000000*b1000000 - b1000);
  TEST("-zillion - b1000 == -b1000000*b1000000*b1000000 - b1000",
        -zillion - b1000,   -b1000000*b1000000*b1000000 - b1000);

  // example in book
  vnl_bignum b2 = "0xffffffff";                 // Create vnl_bignum object
  vnl_bignum b3 = "12345e30";                   // Create vnl_bignum object
  TEST("(b2+b3) - b2 = b3", (b2+b3) - b2 == b3, 1);
  TEST("(b2+b3) - b3 = b2", (b2+b3) - b3 == b2, 1);
  b3.dump();
  TEST("b3.dump()", 1, 1);
}

static void run_multiplication_tests() {
  vcl_cout << "\nStarting multiplication tests:\n";

  vnl_bignum b0(0L), b1000(1000L), b1000000(1000000L),
  zillion("1000000000000000000");

  TEST("b0*b0 == b0", b0*b0, b0);
  TEST("b0*b1000 == b0", b0*b1000, b0);
  TEST("b1000*b0 == b0", b1000*b0, b0);
  TEST("b1000*b1000 == b1000000", b1000*b1000, b1000000);
  TEST("b1000*b1000000 == b1000000*b1000", b1000*b1000000, b1000000*b1000);
  TEST("-b1000000*b1000000*b1000000 == -zillion", -b1000000*b1000000*b1000000, -zillion);
  TEST("zillion*-b1000 == b1000*-zillion", zillion*-b1000, b1000*-zillion);

  TEST("sqrt(b1000000) == 1000", vcl_sqrt(b1000000), b1000);
  TEST("sqrt(zillion) == b1000*b1000000", vcl_sqrt(zillion), b1000*b1000000);
}

static void run_left_shift_tests() {

  vnl_bignum b1(1L);

  // left shift
  TEST("int(b1) == 1",int(b1), 1);
  TEST("long(b1 << 1) == 2",long(b1 << 1), 2);
  TEST("long(b1 << 2) == 4",long(b1 << 2), 4);
  TEST("long(b1 << 3) == 8",long(b1 << 3), 8);
  TEST("long(b1 << 4) == 16",long(b1 << 4), 16);
  TEST("long(b1 << 5) == 32",long(b1 << 5), 32);
  TEST("long(b1 << 6) == 64",long(b1 << 6), 64);
  TEST("long(b1 << 7) == 128",long(b1 << 7), 128);
  TEST("long(b1 << 8) == 256",long(b1 << 8), 256);
  TEST("long(b1 << 9) == 512",long(b1 << 9), 512);
  TEST("long(b1 << 10) == 1024",long(b1 << 10), 1024);
  TEST("long(b1 << 11) == 2048",long(b1 << 11), 2048);
  TEST("long(b1 << 12) == 4096",long(b1 << 12), 4096);
  TEST("long(b1 << 13) == 8192",long(b1 << 13), 8192);
  TEST("long(b1 << 14) == 16384",long(b1 << 14), 16384);
  TEST("long(b1 << 15) == 32768",long(b1 << 15), 32768);
  TEST("long(b1 << 16) == 65536",long(b1 << 16), 65536);
  TEST("(b1 << 32) == vnl_bignum(\"4294967296\")",
       (b1 << 32), vnl_bignum("4294967296"));
  TEST("long(-b1 << 1) == -2",long(-b1 << 1), -2);
  TEST("long(-b1 << 2) == -4",long(-b1 << 2), -4);
  TEST("long(-b1 << 3) == -8",long(-b1 << 3), -8);
  TEST("long(-b1 << 4) == -16",long(-b1 << 4), -16);
  TEST("long(-b1 << 5) == -32",long(-b1 << 5), -32);
  TEST("long(-b1 << 6) == -64",long(-b1 << 6), -64);
  TEST("long(-b1 << 7) == -128",long(-b1 << 7), -128);
  TEST("long(-b1 << 8) == -256",long(-b1 << 8), -256);
  TEST("long(-b1 << 9) == -512",long(-b1 << 9), -512);
  TEST("long(-b1 << 10) == -1024",long(-b1 << 10), -1024);
  TEST("long(-b1 << 11) == -2048",long(-b1 << 11), -2048);
  TEST("long(-b1 << 12) == -4096",long(-b1 << 12), -4096);
  TEST("long(-b1 << 13) == -8192",long(-b1 << 13), -8192);
  TEST("long(-b1 << 14) == -16384",long(-b1 << 14), -16384);
  TEST("long(-b1 << 15) == -32768",long(-b1 << 15), -32768);
  TEST("long(-b1 << 16) == -65536",long(-b1 << 16), -65536);
  TEST("(-b1 << 32) == vnl_bignum(\"-4294967296\")",
       (-b1 << 32), vnl_bignum("-4294967296"));
  TEST("long(b1 << -16) == 0",long(b1 << -16), 0);
  TEST("long(-b1 << -16) == 0",long(-b1 << -16), 0);
}

static void run_right_shift_tests() {
  // right shift
  vnl_bignum b2("4294967296");
  TEST("b2 == vnl_bignum(\"4294967296\")",b2, vnl_bignum("4294967296"));
  TEST("(b2 >> 1) == vnl_bignum(\"2147483648\")", (b2 >> 1), vnl_bignum("2147483648"));
  TEST("long(b2 >> 2) == 1073741824",long(b2 >> 2), 1073741824L);
  TEST("long(b2 >> 3) == 536870912",long(b2 >> 3), 536870912L);
  TEST("long(b2 >> 4) == 268435456",long(b2 >> 4), 268435456L);
  TEST("long(b2 >> 5) == 134217728",long(b2 >> 5), 134217728L);
  TEST("long(b2 >> 6) == 67108864",long(b2 >> 6), 67108864L);
  TEST("long(b2 >> 7) == 33554432",long(b2 >> 7), 33554432L);
  TEST("long(b2 >> 8) == 16777216",long(b2 >> 8), 16777216L);
  TEST("long(b2 >> 9) == 8388608",long(b2 >> 9), 8388608L);
  TEST("long(b2 >> 10) == 4194304",long(b2 >> 10), 4194304L);
  TEST("long(b2 >> 11) == 2097152",long(b2 >> 11), 2097152L);
  TEST("long(b2 >> 12) == 1048576",long(b2 >> 12), 1048576L);
  TEST("long(b2 >> 13) == 524288",long(b2 >> 13), 524288L);
  TEST("long(b2 >> 14) == 262144",long(b2 >> 14), 262144L);
  TEST("long(b2 >> 15) == 131072",long(b2 >> 15), 131072L);
  TEST("long(b2 >> 16) == 65536",long(b2 >> 16), 65536L);
  TEST("long(b2 >> 32) == 1",long(b2 >> 32), 1L);
  TEST("long(b2 >> 33) == 0",long(b2 >> 33), 0L);
  TEST("long(b2 >> 67) == 0",long(b2 >> 67), 0L);
  TEST("(-b2 >> 1) == vnl_bignum(\"-2147483648\")", (-b2 >> 1), vnl_bignum("-2147483648"));
  TEST("long(-b2 >> 2) == -1073741824",long(-b2 >> 2), -1073741824L);
  TEST("long(-b2 >> 3) == -536870912",long(-b2 >> 3), -536870912L);
  TEST("long(-b2 >> 4) == -268435456",long(-b2 >> 4), -268435456L);
  TEST("long(-b2 >> 5) == -134217728",long(-b2 >> 5), -134217728L);
  TEST("long(-b2 >> 6) == -67108864",long(-b2 >> 6), -67108864L);
  TEST("long(-b2 >> 7) == -33554432",long(-b2 >> 7), -33554432L);
  TEST("long(-b2 >> 8) == -16777216",long(-b2 >> 8), -16777216L);
  TEST("long(-b2 >> 9) == -8388608",long(-b2 >> 9), -8388608L);
  TEST("long(-b2 >> 10) == -4194304",long(-b2 >> 10), -4194304L);
  TEST("long(-b2 >> 11) == -2097152",long(-b2 >> 11), -2097152L);
  TEST("long(-b2 >> 12) == -1048576",long(-b2 >> 12), -1048576L);
  TEST("long(-b2 >> 13) == -524288",long(-b2 >> 13), -524288L);
  TEST("long(-b2 >> 14) == -262144",long(-b2 >> 14), -262144L);
  TEST("long(-b2 >> 15) == -131072",long(-b2 >> 15), -131072L);
  TEST("long(-b2 >> 16) == -65536",long(-b2 >> 16), -65536L);
  TEST("long(-b2 >> 32) == -1",long(-b2 >> 32), -1);
  TEST("long(-b2 >> 33) == -0",long(-b2 >> 33), 0);
  TEST("long(-b2 >> 67) == -0",long(-b2 >> 67), 0);
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
