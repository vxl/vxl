// This is core/vsl/tests/test_arbitrary_length_int_conversion.cxx
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_config_compiler.h>
#include <vsl/vsl_binary_explicit_io.h>
#include <testlib/testlib_test.h>
#include <vpl/vpl.h>

void test_arbitrary_length_int_conversion_int()
{
  vcl_cout << "*********************************************************\n"
           << "Testing arbitrary length int conversion for unsigned ints\n"
           << "*********************************************************\n";

  signed int  * a = new signed int [25000000];
  signed int  * b = new signed int [25000000];

  int i;
  for (i = 0; i < 25000000; ++i)
    a[i] = ((i-12500000)*160);

  unsigned maxbuf =  VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(signed int)) *
    25000000;

  unsigned char * buf = new unsigned char[maxbuf];

  vcl_clock_t t1, t2;

  vcl_cout << " Starting encode\n";

  t1 = vcl_clock();
  unsigned long len = vsl_convert_to_arbitrary_length(a, buf, 25000000);
  t2 = vcl_clock();

  vcl_cout << " Required " << (double)(t2-t1) / CLOCKS_PER_SEC
           << " seconds to encode 25M ints.\n"

           << " Max required buffer size is " << maxbuf << " bytes. Used "
           << len << vcl_endl;

  TEST("Checking that the buffer didn't overflow", len < maxbuf, true);

  vcl_cout << " Starting decode\n";
  t1 = vcl_clock();
  unsigned long len2 = vsl_convert_from_arbitrary_length(buf, b, 25000000);
  t2 = vcl_clock();
  vcl_cout << " Required " << (double)(t2-t1) / CLOCKS_PER_SEC
           << " seconds to decode and test 25M ints.\n";

  TEST("Checking len == len2", len, len2);

  for (i=0; i <25000000; ++i)
    if (b[i] != (i-12500000)*160) break;

  TEST("Checking that the results are correct", i, 25000000);
  if (i != 25000000)
    vcl_cout << "Failed at number " << i <<vcl_endl;
  delete a;
  delete b;
  delete buf;
}


void test_arbitrary_length_int_conversion_short()
{
  vcl_cout << "*********************************************************\n"
           << "Testing arbitrary length int conversion for signed shorts\n"
           << "*********************************************************\n";

  signed short  a[65538];
  signed short  b[65540];
  signed short * c = &b[1];

  int i;
  for (i = 0; i < 65536; ++i)
    a[i] = i-32768;
  a[65536] = 0;
  a[65537] = 1;

  unsigned maxbuf =
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short))
    * 65538;

  unsigned char * buf = new unsigned char[maxbuf];
  unsigned long len = vsl_convert_to_arbitrary_length(a, buf, 65538);
  vcl_cout << " Max required buffer size is " << maxbuf << ". Used " << len
           << vcl_endl;

  TEST("Checking that the buffer didn't overflow", len < maxbuf, true);

  b[0] = (short) (0xc5c5);
  b[65539] = 0x5c5c;
  unsigned long len2 = vsl_convert_from_arbitrary_length(buf, c, 65538);

  TEST("Checking that the result buffer didn't overflow",
       (b[0] == (short)0xc5c5) && (b[65539] == 0x5c5c), true);

  TEST("Checking len == len2", len, len2);

  for (i=0; i <65536; ++i)
    if (c[i] != i-32768) break;
  TEST("Checking that the results are correct", i, 65536);
  if (i != 65536)
    vcl_cout << "Failed at number " << i <<vcl_endl;

  TEST("Checking the end conditions", c[65536] == 0 && c[65537] == 1, true);

  delete[] buf;
}


void test_arbitrary_length_int_conversion_ushort()
{
  vcl_cout << "***********************************************************\n"
           << "Testing arbitrary length int conversion for unsigned shorts\n"
           << "***********************************************************\n";

  unsigned short  a[65538];
  unsigned short  b[65540];
  unsigned short * c = &b[1];

  int i;
  for (i = 0; i < 65536; ++i)
    a[i] = i;
  a[65536] = 0;
  a[65537] = 1;

  unsigned maxbuf =
    VSL_MAX_ARBITRARY_INT_BUFFER_LENGTH(sizeof(unsigned short))
    * 65538;

  unsigned char * buf = new unsigned char[maxbuf];
  unsigned long len = vsl_convert_to_arbitrary_length(a, buf, 65538);
  vcl_cout << " Max required buffer size is " << maxbuf << ". Used " << len
           << vcl_endl;

  TEST("Checking that the buffer didn't overflow", len < maxbuf, true);

  b[0] = 0xc5c5;
  b[65539] = 0x5c5c;
  unsigned len2 = vsl_convert_from_arbitrary_length(buf, c, 65538);

  TEST("Checking that the result buffer didn't overflow",
       (b[0] == 0xc5c5) && (b[65539] == 0x5c5c), true);
  TEST("Checking len == len2", len, len2);

  for (i=0; i <65536; ++i)
    if (c[i] != i) break;
  TEST("Checking that the results are correct", i, 65536);
  if (i != 65536)
    vcl_cout << "Failed at number " << i <<vcl_endl;

  TEST("Checking the end conditions", c[65536] == 0 && c[65537] == 1, true);
  delete[] buf;
}


void test_explicit_int_io()
{
  vcl_cout << "**********************************\n"
           << "Testing explicit length integer io\n"
           << "**********************************\n";

  unsigned long i;

  vsl_b_ofstream bfs_out("vsl_explicit_int_io_test.bvl.tmp");
  TEST("Created vsl_explicit_int_io_test.bvl.tmp for writing",
       (!bfs_out), false);
  for (i = 0; i < 65536; ++i)
    vsl_b_write_uint_16(bfs_out, i);
  bfs_out.close();

  vsl_b_ifstream bfs_in("vsl_explicit_int_io_test.bvl.tmp");
  TEST("Opened vsl_explicit_int_io_test.bvl.tmp for reading",
       (!bfs_in), false);
  for (i = 0; i < 65536; ++i)
  {
    unsigned long n;
    vsl_b_read_uint_16(bfs_in, n);
    if (n != i) break;
  }

// stringstream not yet properly supported on gcc-2.95.2 (see vcl/vcl_sstream.h) and SGI
#if VCL_HAS_WORKING_STRINGSTREAM
  vcl_stringstream ss(vcl_ios_in | vcl_ios_out | vcl_ios_binary);
  //const char *b= ss.str().c_str();
  //Is the above simply to make a call to c_str? b is otherwise unused. Removing it
  //to avoid compiler warnings.
  ss.str().c_str();
  {
    vsl_b_ostream bss(&ss);
    TEST("Created stringstream for writing", (!bss), false);
    for (i = 0; i < 65536; ++i)
      vsl_b_write_uint_16(bss, i);
  }

  vcl_stringstream ss2(ss.str());
  {
    vsl_b_istream bss(&ss2);
    TEST("Opened stringstream for reading", (!bss), false);
    for (i = 0; i < 65536; ++i)
    {
      unsigned long n;
      vsl_b_read_uint_16(bss, n);
      if (n != i) break;
    }
    TEST("Finished reading file successfully", (!bss), false);
  }
#endif

  TEST("Checking that the results are correct", i, 65536);
  if (i != 65536)
    vcl_cout << "Failed at number " << i <<vcl_endl;
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_explicit_int_io_test.bvl.tmp");
}


void test_extreme_int_io()
{
  vcl_cout << "************************************\n"
           << "Testing largest/smallest integer I/O\n"
           << "************************************\n";

  // Some fudges to get the max values
  // vcl_numeric_limits doesn't seem to work yet
  long min_long = 1L<<(8*sizeof(long)-1);
  long max_long = ~min_long;
  unsigned long max_ulong = ~0;

  vsl_b_ofstream bfs_out("vsl_extreme_int_io_test.bvl.tmp");
  TEST("Created vsl_extreme_int_io_test.bvl.tmp for writing",(!bfs_out),false);

  vsl_b_write(bfs_out,min_long);
  vsl_b_write(bfs_out,max_long);
  vsl_b_write(bfs_out,max_ulong);
  bfs_out.close();

  long min_long_in = 77;
  long max_long_in = 77;
  unsigned long max_ulong_in = 77;


  vsl_b_ifstream bfs_in("vsl_extreme_int_io_test.bvl.tmp");
  TEST("Opened vsl_extreme_int_io_test.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in,min_long_in);
  vsl_b_read(bfs_in,max_long_in);
  vsl_b_read(bfs_in,max_ulong_in);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  vpl_unlink ("vsl_extreme_int_io_test.bvl.tmp");

  TEST("min_long == min_long_in", min_long, min_long_in);
  TEST("max_long == max_long_in", max_long, max_long_in);
  TEST("max_ulong == max_ulong_in", max_ulong, max_ulong_in);
}


void test_arbitrary_length_int_conversion()
{
  test_arbitrary_length_int_conversion_ushort();
  test_arbitrary_length_int_conversion_short();
  test_explicit_int_io();
  test_extreme_int_io();
//test_arbitrary_length_int_conversion_int();
}

TESTMAIN(test_arbitrary_length_int_conversion);
