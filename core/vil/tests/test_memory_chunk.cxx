// This is mul/vil2/tests/test_memory_chunk.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil2/vil2_memory_chunk.h>
#include <vil2/io/vil2_io_memory_chunk.h>

void test_memory_chunk1()
{
  vcl_cout << "***************************\n"
           << " Testing vil2_memory_chunk\n"
           << "***************************\n";

  vil2_memory_chunk chunk1(35,VIL2_PIXEL_FORMAT_BYTE);
  TEST("size()",chunk1.size(),35);
  TEST("format",chunk1.pixel_format(),VIL2_PIXEL_FORMAT_BYTE);

  chunk1.set_size(25*sizeof(double),VIL2_PIXEL_FORMAT_DOUBLE);
  TEST("size()",chunk1.size(),25*sizeof(double));
  TEST("format",chunk1.pixel_format(),VIL2_PIXEL_FORMAT_DOUBLE);

  double* data1 = reinterpret_cast<double*>(chunk1.data());
  data1[3]= 17;

  vil2_memory_chunk chunk2 = chunk1;
  TEST("size()",chunk2.size(),25*sizeof(double));
  TEST("format",chunk2.pixel_format(),VIL2_PIXEL_FORMAT_DOUBLE);
  double* data2 = reinterpret_cast<double*>(chunk2.data());
  TEST_NEAR("Deep Copy",data1[3],data2[3],1e-8);
}


MAIN( test_memory_chunk )
{
  START( "vil2_memory_chunk" );
  test_memory_chunk1();

  SUMMARY();
}
