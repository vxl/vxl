//:
// \file
// Tests for vnl_finite_field_number<N>, written by Peter Vanroose, 6 May 2002.

#include <vcl_iostream.h>
#include <vnl/vnl_finite_field_number.h>
#include <vnl/vnl_test.h>

template <int N>
void test_field(vnl_finite_field_number<N>) {
  vcl_cout << "\n --- Testing vnl_finite_field_number<" << N << ">: ---\n";

  vcl_cout << "\nStarting constructor tests:\n";

  {vnl_finite_field_number<N> b(0); TEST("vnl_finite_field_number<N> b(0);", b, 0);}
  {vnl_finite_field_number<N> b(1); TEST("vnl_finite_field_number<N> b(1);", b, 1);}
  {vnl_finite_field_number<N> b(-1); TEST("vnl_finite_field_number<N> b(-1);", b, -1);}
  {vnl_finite_field_number<N> b(-1); TEST("b == N-1;", b, N-1);}
  {vnl_finite_field_number<N> b(111); TEST("vnl_finite_field_number<N> b(111);", b, 111);}
  {vnl_finite_field_number<N> b(-99); TEST("vnl_finite_field_number<N> b(-99);", b, -99);}

  vcl_cout << "\nStarting assignment tests:\n";
  vnl_finite_field_number<N> b1; b1 = 2;
  TEST("vnl_finite_field_number<N> b1; b1 = 2;", b1, 2);
  b1 = 10;
  TEST("vnl_finite_field_number<N> b1; b1 = 10;", b1, 10-N);
  b1 = -77;
  TEST("vnl_finite_field_number<N> b1; b1 = -77;", b1, 11*N-77);

  vnl_finite_field_number<N> b5 = 4321; b1 = b5;
  TEST("b1 = b5", b1, b5);

  vcl_cout << "b1 = " << b1 << vcl_endl;
  vcl_cout << "b5 = " << b5 << vcl_endl;
  TEST("<<", 1, 1);

  TEST("unary plus", +b5, b5);

  vcl_cout << "\nStarting logical comparison tests:\n";
  vnl_finite_field_number<N> b0=0;
  b1=1;
  vnl_finite_field_number<N> b2=2;
  vnl_finite_field_number<N> b3=-2;

  TEST("b0 == b0", b0 == b0, true);
  TEST("b0 == b1", b0 == b1, false);
  TEST("b0 == b2", b0 == b2, N==2);
  TEST("b0 == b3", b0 == b3, N==2);
  TEST("b1 == b2", b1 == b2, false);
  TEST("b1 == b3", b1 == b3, N==3);
  TEST("b2 == b3", b2 == b3, N==2 || N==4);

  TEST("b0 != b0", b0 != b0, false);
  TEST("b0 != b1", b0 != b1, true);
  TEST("b0 != b2", b0 != b2, N!=2);
  TEST("b0 != b3", b0 != b3, N!=2);
  TEST("b1 != b2", b1 != b2, true);
  TEST("b1 != b3", b1 != b3, N!=3);
  TEST("b2 != b3", b2 != b3, N!=2 && N!=4);

  TEST("b3 != b2", b3 != b2, N!=2 && N!=4);
  TEST("b3 != b3", b3 != b3, false);

  vcl_cout << "\nStarting addition, subtraction tests:\n";

  vnl_finite_field_number<N> bi,bj,bij;
  vcl_cout << " for (i = 1; i < 1000; i *= 3)\n";
  vcl_cout << "   for (j = 1; j < 1000; j *= 3)\n      ";

  for (int i = 1; i < 1000;  i *= 3) {
    for (int j = 1; j < 1000; j *= 3) {
      bi = i; bj = j;
      bij = vnl_finite_field_number<N>(i+j);
      if (bi + bj != bij) {
        TEST("bi + bj == vnl_finite_field_number<N>(i + j)", false, true);
        vcl_cout << "i = "<<i<<", j = "<<j<<'\n';
      }
      bij = vnl_finite_field_number<N>(i-j);
      if (bi - bj != bij) {
        TEST("bi - bj == vnl_finite_field_number<N>(i - j)", false, true);
        vcl_cout << "i = "<<i<<", j = "<<j<<'\n';
      }
    }
  }
  vcl_cout << vcl_endl;

  vnl_finite_field_number<N> b1000(1000);

  TEST("-b0 == b0", -b0, b0);
  TEST("-b1000 == (-1)*b1000", -b1000, (-1)*b1000);
  TEST("-(-b1000) == b1000", -(-b1000), b1000);
  TEST("b0 + b1000 == b1000", b0 + b1000, b1000);
  TEST("b1000 + b0 == b1000", b1000 + b0, b1000);
  TEST("b0 + (-b1000) == -b1000", b0 + (-b1000), -b1000);
  TEST("-b1000 + b0 == -b1000", -b1000 + b0, -b1000);
  TEST("-b1000 + (-b1000) == 2*(-b1000))", -b1000 + (-b1000), 2*(-b1000));
  TEST("b1000 + (-b1000) == b0", b1000 + (-b1000), b0);

  TEST("b0 - b1000 == -b1000", b0 - b1000, -b1000);
  TEST("b1000 - b0 == b1000", b1000 - b0, b1000);
  TEST("b0 - (-b1000) == b1000", b0 - (-b1000), b1000);
  TEST("-b1000 - b0 == -b1000", -b1000 - b0, -b1000);
  TEST("-b1000 - (-b1000) == b0", -b1000 - (-b1000), b0);
  TEST("-b1000 - b1000 == -2*b1000", -b1000 - b1000, -2*b1000);
  TEST("b1000 - (-b1000) == 2*b1000", b1000 - (-b1000), 2*b1000);

  b2 = 1237; // should not be divisible by 2, 3, 5, 7, 11
  b3 = 4321;
  TEST("(b2+b3) - b2 = b3", (b2+b3) - b2, b3);
  TEST("(b2+b3) - b3 = b2", (b2+b3) - b3, b2);

  vcl_cout << "\nStarting multiplication tests:\n";

  TEST("b0*b0 == b0", b0*b0, b0);
  TEST("b0*b1000 == b0", b0*b1000, b0);
  TEST("b1000*b0 == b0", b1000*b0, b0);
  vnl_finite_field_number<N> b1000000(1000000);
  TEST("b1000*b1000 == b1000000", b1000*b1000, b1000000);
  TEST("b1000*b1000000 == b1000000*b1000", b1000*b1000000, b1000000*b1000);

  // Do not continue for non-field
  if (!vnl_finite_field_number<N>::is_field()) return;

  // would take too long - this is not a fast implementation!
  if (N > 10000) return;

  vcl_cout << "\nStarting division tests:\n";

  TEST("b0/b1", b0/b1, 0);
  TEST("(-b1)/b1", (-b1)/b1, -1);

  TEST("(b2*b3) / b3 = b2", (b2*b3) / b3, b2);
  TEST("(b2*b3) / b2 = b3", (b2*b3) / b2, b3);

  vcl_cout << " for (i = 10000; i > 0; i /= 3) \n";
  vcl_cout << "   for (j = 10000; j > 0; j /= 3) \n";
  vcl_cout << "     for (k = 1; k < 17; ++k) \n";
  vcl_cout << "       for (l = 1; l < 17; ++l) \n         ";
  for (int i = 10000; i > 0; i /= 3) {
    for (int j = 10000; j > 0; j /= 3) {
      for (int k = 1; k < 17; ++k) {
        for (int l = 1; l < 17; ++l) {
          b1 = vnl_finite_field_number<N>(i+k);
          b2 = vnl_finite_field_number<N>(j+l);
          if (b2 == 0) continue; // should not divide by 0
          b3 = b1/b2;
          int r = int(b3); r = r*(j+l)-(i+k);
          if (r%N) {
            TEST("(vnl_finite_field_number<N>(i+k)/vnl_finite_field_number<N>(j+l))", false, true);
            vcl_cout<< "i="<<i<<", j="<<j<<", k="<<k<<", l="<<l<<'\n'
                    << "b1="<<int(b1)<<", b2="<<int(b2)<<", b3="<<int(b3)<<'\n';
          }
        }
      }
    }
  }
  vcl_cout << vcl_endl;
}

void test_finite_field() {
  test_field(vnl_finite_field_number<2>(0));
  test_field(vnl_finite_field_number<3>(0));
  test_field(vnl_finite_field_number<4>(0)); // not a field
  vnl_finite_field_number<4> b = 2; TEST("2*2=0", b*b, 0); // zero divisor
  test_field(vnl_finite_field_number<5>(0));
  test_field(vnl_finite_field_number<6>(0));
  test_field(vnl_finite_field_number<7>(0));
  test_field(vnl_finite_field_number<8>(0));
  test_field(vnl_finite_field_number<9>(0));
  test_field(vnl_finite_field_number<10>(0));
  test_field(vnl_finite_field_number<11>(0));
  test_field(vnl_finite_field_number<12>(0));
  test_field(vnl_finite_field_number<257>(0));
  test_field(vnl_finite_field_number<100000007>(0));
}

TESTMAIN(test_finite_field);
