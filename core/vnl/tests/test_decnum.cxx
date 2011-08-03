#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <vnl/vnl_decnum.h>
#include <testlib/testlib_test.h>

// Two auxiliary functions, used in multiplication and division tests

// Factorial
static vnl_decnum factorial(unsigned long n)
{
  if (n <= 1) return vnl_decnum(1L);
  else        return factorial(n-1) * vnl_decnum(n);
}

// Combinations (n choose k)
static vnl_decnum Combinations(unsigned long n, unsigned long k)
{ 
  vnl_decnum r(1L);
  if (k >= n || k == 0) return r;
  else 
  {
    for (unsigned long i=n; i>n-k; --i)
      r *= i; 
    r /= factorial(k);
    return r;
  }
}

static void run_constructor_tests()
{
  vcl_cout << "\ndecnum constructor tests:\n";

  vcl_cout << "long constructor:\n";
  {vnl_decnum b(0L); TEST("vnl_decnum b(0L);", b, 0L);}
  {vnl_decnum b(1L); TEST("vnl_decnum b(1L);", b, 1L);}
  {vnl_decnum b(-1L); TEST("vnl_decnum b(-1L);", b, -1L);}
  {vnl_decnum b(0x7fffL); TEST("vnl_decnum b(0x7fffL);", b, 0x7fffL);}
  {vnl_decnum b(-0x7fffL); TEST("vnl_decnum b(-0x7fffL);", b, -0x7fffL);}
  {vnl_decnum b(0x7fffffffL); TEST("vnl_decnum b(0x7fffffffL);", b, 0x7fffffffL);}
  {vnl_decnum b(-0x7fffffffL); TEST("vnl_decnum b(-0x7fffffffL);", b, -0x7fffffffL);}
  {vnl_decnum b(0xf00000L); TEST("vnl_decnum b(0xf00000L);", b, 0xf00000);}

  vcl_cout << "char* constructor:\n";
  {vnl_decnum b("-1"); TEST("vnl_decnum b(\"-1\");", b, -1L);}
  {vnl_decnum b("+1"); TEST("vnl_decnum b(\"+1\");", b, 1L);}
  {vnl_decnum b("1"); TEST("vnl_decnum b(\"1\");", b, 1L);}
  {vnl_decnum b("123"); TEST("vnl_decnum b(\"123\");", b, 123L);}
  {vnl_decnum b("12300000"); TEST("vnl_decnum b(\"12300000\");", b, 12300000L);}
  {vnl_decnum b("-1230000"); TEST("vnl_decnum b(\"-1230000\");", b, -1230000L);}
  {vnl_decnum b("-1000000000000000000000000000000");
   vcl_stringstream s; s << b << ' ';
   // verify that b outputs as  "-1000...00" (30 zeros)
   bool t = s.str()[0] == '-' && s.str()[1] == '1' && s.str()[32] == ' ';
   for (int i=0; i<30; ++i) t = t && s.str()[i+2] == '0';
   TEST("vnl_decnum b(\"-100...00\") outputs as \"-100...00\"", t, true);
  }
  {vnl_decnum b("0"); TEST("vnl_decnum b(\"0\");", b, 0L);}
  {vnl_decnum b("00"); TEST("vnl_decnum b(\"00\");", b, 0L);}
  {vnl_decnum b("012334567"); TEST("vnl_decnum b(\"012334567\");", b, 12334567L);}
  {vnl_decnum b("9"); TEST("vnl_decnum b(\"9\");", b, 9L);}
  {vnl_decnum b(" 9"); TEST("vnl_decnum b(\" 9\");", b, 9L);}

  vcl_cout << "reading from istream:\n";
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << "+1"; is >> b; TEST("\"+1\" >> b;", b, 1L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << "-1"; is >> b; TEST("\"-1\" >> b;", b, -1L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << "123"; is >> b; TEST("\"123\" >> b;", b, 123L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << "12300000"; is >> b; TEST("\"12300000\" >> b;", b, 12300000L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << "-1230000"; is >> b; TEST("\"-1230000\" >> b;", b, -1230000L);}
  {vcl_stringstream is(vcl_ios_in | vcl_ios_out); vnl_decnum b;
   is << " 9"; is >> b; TEST("\" 9\" >> b;", b, 9L);}

  vcl_cout << "vnl_decnum& constructor:\n";
  {vnl_decnum b50(vnl_decnum(0L));
   TEST("vnl_decnum b50(vnl_decnum(0L));", b50, 0L);}

  {vnl_decnum b51(vnl_decnum(100L));
   TEST("vnl_decnum b51(vnl_decnum(100L));", b51, 100L);}
}

static void run_conversion_operator_tests()
{
  vcl_cout << "\nConversion operator tests:\n";

  vcl_cout << "int conversion operator:\n";
  TEST("int(vnl_decnum(0L)) == 0", int(vnl_decnum(0L)), 0);
  TEST("int(vnl_decnum(0x7fffffffL)) == 0x7fffffff", int(vnl_decnum(0x7fffffffL)), 0x7fffffff);
  TEST("int(vnl_decnum(-0x7fffffffL)) == -0x7fffffff", int(vnl_decnum(-0x7fffffffL)), -0x7fffffff);

  vcl_cout << "long conversion operator:\n";
  vnl_decnum b(0x7fffffffL); ++b;
  TEST("vnl_decnum b(0x7fffffffL); ++b; (unsigned long)b == 0x80000000UL", (unsigned long)b, 0x80000000UL);
  --b;
  TEST("vnl_decnum b(0x80000000UL); --b; long(b) == 0x7fffffffL", long(b), 0x7fffffffL);

}

static void run_assignment_tests()
{
  vcl_cout << "\nStarting assignment tests:\n";
  vnl_decnum b1;

  TEST_RUN ("vnl_decnum b1; b1 = 0xffff;", b1 = 0xffffL, long(b1), 0xffffL);

  // long assignment operator
  TEST_RUN ("long(b1) = -0x7fffffff", b1 = -0x7fffffffL, long(b1), -0x7fffffff);

  // char * assignment operator
  TEST_RUN ("long(b1) = 0x1fffffL", b1 = "123456789", long(b1), 123456789L);

  // vnl_decnum& assignment operator
  b1 = "0";
  vnl_decnum b5(0x1ffffL);
  TEST_RUN ("b1 = b5", b1 = b5, b1, b5);
}

static void run_logical_comparison_tests()
{
  vcl_cout << "\nStarting logical comparison tests:\n";
  vnl_decnum b0(0L);
  vnl_decnum b1(1L);
  vnl_decnum b2(0x7fffL);
  vnl_decnum b3(-0x7fffL);

  TEST("b0 == b0", b0 == b0, true);
  TEST("b0 == b1", b0 == b1, false);
  TEST("b0 == b2", b0 == b2, false);
  TEST("b0 == b3", b0 == b3, false);
  TEST("b1 == b1", b1 == b1, true);
  TEST("b1 == b2", b1 == b2, false);
  TEST("b1 == b3", b1 == b3, false);
  TEST("b2 == b2", b2 == b2, true);
  TEST("b2 == b3", b2 == b3, false);
  TEST("b3 == b3", b3 == b3, true);

  TEST("b0 != b0", b0 != b0, false);
  TEST("b0 != b1", b0 != b1, true);
  TEST("b0 != b2", b0 != b2, true);
  TEST("b0 != b3", b0 != b3, true);
  TEST("b1 != b1", b1 != b1, false);
  TEST("b1 != b2", b1 != b2, true);
  TEST("b1 != b3", b1 != b3, true);
  TEST("b2 != b2", b2 != b2, false);
  TEST("b2 != b3", b2 != b3, true);
  TEST("b3 != b3", b3 != b3, false);

  TEST("b0 < b0", b0 < b0, false);
  TEST("b0 < b1", b0 < b1, true);
  TEST("b0 < b2", b0 < b2, true);
  TEST("b0 < b3", b0 < b3, false);
  TEST("b1 < b1", b1 < b1, false);
  TEST("b1 < b2", b1 < b2, true);
  TEST("b1 < b3", b1 < b3, false);
  TEST("b2 < b2", b2 < b2, false);
  TEST("b2 < b3", b2 < b3, false);
  TEST("b3 < b3", b3 < b3, false);

  TEST("b0 > b0", b0 > b0, false);
  TEST("b0 > b1", b0 > b1, false);
  TEST("b0 > b2", b0 > b2, false);
  TEST("b0 > b3", b0 > b3, true);
  TEST("b1 > b1", b1 > b1, false);
  TEST("b1 > b2", b1 > b2, false);
  TEST("b1 > b3", b1 > b3, true);
  TEST("b2 > b2", b2 > b2, false);
  TEST("b2 > b3", b2 > b3, true);
  TEST("b3 > b3", b3 > b3, false);

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
}

static void run_division_tests()
{
  vcl_cout << "\nStarting division tests:\n";

  TEST("long(vnl_decnum(0L)/vnl_decnum(1L))", long(vnl_decnum(0L)/vnl_decnum(1L)), 0L);
  TEST("long(vnl_decnum(-1L)/vnl_decnum(1L))", long(vnl_decnum(-1L)/vnl_decnum(1L)), -1L);

  for (long i = 0xffff; i!=0; i /= -16) {
   for (long k = -6; k < 9; k+=3) {
     vnl_decnum bi(i+k);
     for (long j = 0xffff; j!=0; j /= -16) {
      for (long l = -4; l < 9; l+=3) { // avoid division by zero...
       vnl_decnum bj(j+l);
       vnl_decnum bij(long((i+k)/(j+l)));
       {vcl_stringstream s; s << bi << " / " << bj << " == " << bi/bj << " == " << bij << " ?";
        TEST(s.str().c_str(), bi/bj, bij);}
       bij = long((i+k)%(j+l));
       {vcl_stringstream s; s << bi << " % " << bj << " == " << bi%bj << " == " << bij << " ?";
        TEST(s.str().c_str(), bi%bj, bij);}
     }}
  }}
}

static void run_large_division_tests()
{
  vcl_cout << "\nStarting large division tests:\n";

  vnl_decnum a("10000000"), b("10000001"); b *= a; vnl_decnum c = b/10000001;
  vcl_cout << b << " / 10000001 = " << c << ", must be 10000000\n";
  TEST("100000010000000 / 10000001", c, a);

  // an other example:
  a = "111111"; b = "111111"; b *= a; c = b/111111;
  vcl_cout << b << " / 111111 = " << c << ", must be 111111\n";
  TEST("12345654321 / 111111", c, a);

  a = "98789"; b = "98789"; b *= a; c = b/98789;
  vcl_cout << b << " / 98789 = " << c << ", must be 98789\n";
  TEST("9759266521 / 98789", c, a);

  a = "10000000000000000000000000"; b = "100000000000000000000000000000000000000000000000000"; c = b/a;
  vcl_cout << "100000000000000000000000000000000000000000000000000 / 10000000000000000000000000 = "
           << c << ", must be 10000000000000000000000000\n";
  TEST("100000000000000000000000000000000000000000000000000 / 10000000000000000000000000", c, a);

  a = "-10000000000000000000000000"; b = "100000000000000000000000000000000000000000000000000"; c = b/a;
  vcl_cout << "100000000000000000000000000000000000000000000000000 / -10000000000000000000000000 = "
           << c << ", must be -10000000000000000000000000\n";
  TEST("100000000000000000000000000000000000000000000000000 / -10000000000000000000000000", c, a);

  a = "10000000000000000000000000"; b = "-100000000000000000000000000000000000000000000000000"; c = b/a;
  vcl_cout << "-100000000000000000000000000000000000000000000000000 / 10000000000000000000000000 = "
           << c << ", must be -10000000000000000000000000\n";
  TEST("-100000000000000000000000000000000000000000000000000 / 10000000000000000000000000", c, -a);

  a = "-10000000000000000000000000"; b = "-100000000000000000000000000000000000000000000000000"; c = b/a;
  vcl_cout << "-100000000000000000000000000000000000000000000000000 / -10000000000000000000000000 = "
           << c << ", must be 10000000000000000000000000\n";
  TEST("-100000000000000000000000000000000000000000000000000 / -10000000000000000000000000", c, -a);

  a = "10000000000000000000000000"; b = "100000000000000000000000000000000000000000000000000"; c = b%a;
  vcl_cout << "100000000000000000000000000000000000000000000000000 % 10000000000000000000000000 = "
           << c << ", must be 0\n";
  TEST("10000000000000000000000000^2 % 10000000000000000000000000", c, 0);

  vcl_cout << "C(16,8) = " << Combinations(16,8) << vcl_endl;
  TEST("16 choose 8 = 12870", Combinations(16,8), 12870);
  vcl_cout << "C(18,9) = " << Combinations(18,9) << vcl_endl;
  TEST("18 choose 9 = 48620", Combinations(18,9), 48620);
  vcl_cout << "C(20,10) = " << Combinations(20,10) << vcl_endl;
  TEST("20 choose 10 = 184756", Combinations(20,10), 184756);
  vcl_cout << "C(100,44) = " << Combinations(100,44) << vcl_endl;
  TEST("100 choose 44 = 49378235797073715747364762200",
       Combinations(100,44), "49378235797073715747364762200");
}

static void run_multiplication_division_tests()
{
  vcl_cout << "\nCheck example in book:\n";

  vnl_decnum b2 = "4294967295"; // == 0xffffffff         // Create vnl_decnum object
  vnl_decnum b3 = "12345000000000000000000000000000000"; // Create vnl_decnum object

  vcl_cout << "b2 = " << b2 << vcl_endl
           << "b3 = " << b3 << vcl_endl;

  TEST("(b2*b3) / b3 = b2", ((b2*b3) / b3 == b2), 1);
  TEST("(b2*b3) / b2 = b3", ((b2*b3) / b2 == b3), 1);
  TEST("((b3/b2) * b2) + (b3%b2) = b3", (((b3/b2) * b2) + (b3%b2) == b3), 1);
}

static void run_addition_subtraction_tests()
{
  vcl_cout << "\nStarting addition, subtraction tests:\n";

  long i,j;
  vnl_decnum bi,bj,bij;

  {for (i = 1; i < 0xffff;  i *= -3) {
    bi = i;
    for (j = 1; j < 0xffff; j *= -3) {
      bj = j;
      bij = vnl_decnum(i+j);
      {vcl_stringstream s; s << bi << " + " << bj << " == " << bi+bj << " == " << i+j << " ?";
       TEST(s.str().c_str(), bi+bj, bij);}
      bij = vnl_decnum(i-j);
      {vcl_stringstream s; s << bi << " - " << bj << " == " << bi-bj << " == " << i-j << " ?";
       TEST(s.str().c_str(), bi-bj, bij);}
    }
  }}

  vnl_decnum b0(0L);
  vnl_decnum zillion("1000000000000000000000000000000000000");
  vnl_decnum b1000000(1000000L), b1000000000000("1000000000000");

  TEST("-b0 == b0", -b0, b0);
  TEST("-b1000000 == vnl_decnum(-1L)*b1000000", -b1000000, vnl_decnum(-1L)*b1000000);
  TEST("-(-b1000000000000) == b1000000000000", -(-b1000000000000), b1000000000000);
  TEST("b0 + b1000000 == b1000000", b0 + b1000000, b1000000);
  TEST("b0 + b1000000000000 == b1000000000000", b0 + b1000000000000, b1000000000000);
  TEST("b1000000 + b0 == b1000000", b1000000 + b0, b1000000);
  TEST("b1000000000000 + b0 == b1000000000000", b1000000000000 + b0, b1000000000000);
  TEST("b0 + (-b1000000) == -b1000000", b0 + (-b1000000), -b1000000);
  TEST("-b1000000 + b0 == -b1000000", -b1000000 + b0, -b1000000);
  TEST("-b1000000 + (-b1000000) == (vnl_decnum(2L)*(-b1000000))",
        -b1000000 + (-b1000000),   (vnl_decnum(2L)*(-b1000000)));
  TEST("-b1000000000000 + (-b1000000000000) == (vnl_decnum(2L)*(-b1000000000000))",
        -b1000000000000 + (-b1000000000000), (vnl_decnum(2L)*(-b1000000000000)));
  TEST("b1000000000000 + (-b1000000000000) == b0", b1000000000000 + (-b1000000000000), b0);
  TEST("zillion + (-zillion) == b0", zillion + (-zillion), b0);
  TEST("zillion + b1000000 == b1000000000000*b1000000000000*b1000000000000 + b1000000",
        zillion + b1000000,   b1000000000000*b1000000000000*b1000000000000 + b1000000);

  TEST("b0 - b1000000 == -b1000000", b0 - b1000000, -b1000000);
  TEST("b0 - b1000000000000 == -b1000000000000", b0 -b1000000000000, -b1000000000000);
  TEST("b1000000 - b0 == b1000000", b1000000 - b0, b1000000);
  TEST("b1000000000000 - b0 == b1000000000000", b1000000000000 - b0, b1000000000000);
  TEST("b0 - (-b1000000) == b1000000", b0 - (-b1000000), b1000000);
  TEST("-b1000000 - b0 == -b1000000", -b1000000 - b0, -b1000000);
  TEST("-b1000000 - (-b1000000) == b0", -b1000000 - (-b1000000), b0);
  TEST("-b1000000 - (-zillion) == zillion - b1000000",
        -b1000000 - (-zillion),   zillion - b1000000);
  TEST("-b1000000000000 - (-b1000000000000) == b0", -b1000000000000 - (-b1000000000000), b0);
  TEST("-b1000000000000 - (b1000000000000) == -vnl_decnum(2L)*b1000000000000",
        -b1000000000000 - (b1000000000000),   -vnl_decnum(2L)*b1000000000000);
  TEST("b1000000000000 - (-b1000000000000) == vnl_decnum(2L)*b1000000000000",
        b1000000000000 - (-b1000000000000),   vnl_decnum(2L)*b1000000000000);
  TEST("zillion - (-zillion) == vnl_decnum(2L)*zillion",
        zillion - (-zillion),   vnl_decnum(2L)*zillion);
  TEST("zillion - b1000000 == b1000000000000*b1000000000000*b1000000000000 - b1000000",
        zillion - b1000000,   b1000000000000*b1000000000000*b1000000000000 - b1000000);
  TEST("-zillion - b1000000 == -b1000000000000*b1000000000000*b1000000000000 - b1000000",
        -zillion - b1000000,   -b1000000000000*b1000000000000*b1000000000000 - b1000000);

  // example in book
  vnl_decnum b2 = "4294967295"; // == 0xffffffff         // Create vnl_decnum object
  vnl_decnum b3 = "12345000000000000000000000000000000"; // Create vnl_decnum object
  TEST("(b2+b3) - b2 = b3", (b2+b3) - b2 == b3, 1);
  TEST("(b2+b3) - b3 = b2", (b2+b3) - b3 == b2, 1);
  vcl_cout << b3 << '\n';
  TEST("cout << b3", 1, 1);
}


static void run_increment_tests()
{
  vcl_cout << "increment special cases:\n";
  vnl_decnum b1;
  TEST("b1     ==  0", b1, 0);
  ++b1;
  TEST("++b1   ==  1", b1, 1);
  ++b1;
  TEST("++b1   ==  2", b1, 2);
  --b1;
  TEST("--b1   ==  1", b1, 1);
  --b1;
  TEST("--b1   ==  0", b1, 0);
  --b1;
  TEST("--b1   == -1", b1, -1);
  --b1;
  TEST("--b1   == -2", b1, -2);
  ++b1;
  TEST("++b1   == -1", b1, -1);
  ++b1;
  TEST("++b1   ==  0", b1, 0);

  vnl_decnum b4("65534");
  TEST("b4     ==  65534", b4, 65534);
  ++b4;
  TEST("++b4   ==  65535", b4, 65535);
  ++b4;
  TEST("++b4   ==  65536", b4, 65536);
  ++b4;
  TEST("++b4   ==  65537", b4, 65537);
  --b4;
  TEST("--b4   ==  65536", b4, 65536);
  --b4;
  TEST("--b4   ==  65535", b4, 65535);
  --b4;
  TEST("--b4   ==  65534", b4, 65534);


  vnl_decnum b5("-65534");
  TEST("b5     ==  -65534", b5, -65534);
  --b5;
  TEST("--b5   ==  -65535", b5, -65535);
  --b5;
  TEST("--b5   ==  -65536", b5, -65536);
  --b5;
  TEST("--b5   ==  -65537", b5, -65537);
  ++b5;
  TEST("++b5   ==  -65536", b5, -65536);
  ++b5;
  TEST("++b5   ==  -65535", b5, -65535);
  ++b5;
  TEST("++b5   ==  -65534", b5, -65534);
}


static void run_multiplication_tests()
{
  vcl_cout << "\nStarting multiplication tests:\n";

  vnl_decnum b0(0L), b1000(1000L), b1000000(1000000L),
  zillion("1000000000000000000");

  TEST("b0*b0 == b0", b0*b0, b0);
  TEST("b0*b1000 == b0", b0*b1000, b0);
  TEST("b1000*b0 == b0", b1000*b0, b0);
  TEST("b1000*b1000 == b1000000", b1000*b1000, b1000000);
  TEST("b1000*b1000000 == b1000000*b1000", b1000*b1000000, b1000000*b1000);
  TEST("-b1000000*b1000000*b1000000 == -zillion", -b1000000*b1000000*b1000000, -zillion);
  TEST("zillion*-b1000 == b1000*-zillion", zillion*-b1000, b1000*-zillion);

  TEST("10! = 3628800", factorial(10), 3628800);
  TEST("15! = 1307674368000", factorial(15), "1307674368000");
  TEST("44! = 2.65827157478844e54", factorial(44),
       "2658271574788448768043625811014615890319638528000000000");
}

void test_decnum()
{
  run_constructor_tests();
  run_conversion_operator_tests();
  run_assignment_tests();
  run_logical_comparison_tests();
  run_increment_tests();
  run_addition_subtraction_tests();
  run_multiplication_tests();
  run_division_tests();
  run_multiplication_division_tests();
  run_large_division_tests();
}

TESTMAIN(test_decnum);
