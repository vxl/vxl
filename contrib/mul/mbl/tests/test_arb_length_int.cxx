#include <mbl/mbl_arb_length_int.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vnl/vnl_test.h>

void test_arb_length_int() {
  {
    mbl_arb_length_int a(-5);
    mbl_arb_length_int b(-7l), c, d(256ul), e(65536u);
    TEST("==", a==-5, true);
    TEST("==", 5==(-a), true);
    TEST("==", 5ul==(-a), true);
    TEST("==", b==-7, true);
    TEST("==", b==-7l, true);
    TEST("==", -7==b, true);
    TEST("==", -7l==b, true);
    TEST("!=", -6l!=b, true);
    TEST("!=", b!=-8l, true);
    TEST("!=", -6!=b, true);
    TEST("!=", b!=-8, true);
    TEST("<", a<d, true);
    TEST("<", a<1L, true);
    TEST("<", a<-1l, true);
    TEST(">", -b<d, true);
    c = e; c*=7; TEST("<=", c<=e, false);
    TEST(">=", b>=-7L, true);
    c = a; c+=b; TEST("+=", c, -12);
    c = a; c+=(-a); TEST("+=", c, 0);
    c = a; c+=(-b); TEST("+=", c, 2);
    c = b; c+=(-a); TEST("+=", c, -2);
    c = a; c-=b; TEST("-=", c, 2);
    c = b; c-=a; TEST("-=", c, -2);
    c = a; c-=a; TEST("-=", c, 0);
    c = d; c+=e; TEST("+=", c, 0x10100);
    c = e; c+=d; TEST("+=", c, 0x10100);
    c = e; c-=(-e); TEST("+=", c, 0x20000);
    c = a; c*=7; TEST("*=", c, -35l);
    c = d; c/=7; TEST("/=", c, 36ul);
    c = d; TEST("%", d%(unsigned char)7, (unsigned char)4);
    c = e; c*=7; TEST("*=", c, 0x70000ul);
    c = (unsigned long) -1l;
    TEST("(long)", c.fits_in_unsigned_long(), true);
    c *= 255; c*= 255;
    TEST("(long)", c.fits_in_unsigned_long(), false);
    TEST("(long)", (long)(d) , 256);
    TEST("(short)", (short)(d) , 256);
    c = 0xffffffff; c*= 255; c*=255;
    vcl_cout << vcl_endl
      << "a=" << a << vcl_endl
      << "b=" << b << vcl_endl
      << "c=" << c << vcl_endl
      << "d=" << d << vcl_endl
      << "e=" << e << vcl_endl;

    vcl_ostringstream ss;
    ss << a << ' ' << b << ' ' << c << ' ' << d << ' ' << e;
    TEST("Printing correct", ss.str(), "-5 -7 279280248357375 256 65536");
  }
  mbl_arb_length_int d(-4l);
  d++; d++; d++; d++;
  TEST("++", d , 0ul);
  for (unsigned i = 0; i < 70000; ++i) ++d;
  vcl_cout << "d= " << d << vcl_endl;
  TEST("++", d, 70000l);
  bool fail = false;
  for (long i = 0; i < 80000; ++i)
  {
    if (d != 70000l-i) { fail=true; break; }
    --d;
  }
  TEST("--", fail, false);
  vcl_cout << "d= " << d << vcl_endl;
  TEST("--", d, -10000l);

  vcl_cout << "===============Testing IO============" <<vcl_endl;

  mbl_arb_length_int c ((unsigned long) -1l);
  c *= 255; c*= 255;

  const vcl_string path = "test_arb_length_int.bvl.tmp";
  vsl_b_ofstream bfs_out(path);
  TEST (("Created " + path +" for writing").c_str(),
             (!bfs_out), false);
  vsl_b_write(bfs_out, c);
  bfs_out.close();

  mbl_arb_length_int c_in;
  vsl_b_ifstream bfs_in(path);
  TEST (("Opened " + path +" for reading").c_str(),
           (!bfs_in), false);
  vsl_b_read(bfs_in, c_in);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("Loaded integer == saved integer", c, c_in);
}

TESTMAIN(test_arb_length_int);
