// This is core/vil/tests/test_memory_chunk.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil/vil_memory_chunk.h>

static void test_memory_chunk()
{
  vcl_cout << "**************************\n"
           << " Testing vil_memory_chunk\n"
           << "**************************\n";

  vil_memory_chunk chunk1(35,VIL_PIXEL_FORMAT_BYTE);
  TEST("size()",chunk1.size(),35);
  TEST("format",chunk1.pixel_format(),VIL_PIXEL_FORMAT_BYTE);

  chunk1.set_size(25*sizeof(double),VIL_PIXEL_FORMAT_DOUBLE);
  TEST("size()",chunk1.size(),25*sizeof(double));
  TEST("format",chunk1.pixel_format(),VIL_PIXEL_FORMAT_DOUBLE);

  double* data1 = reinterpret_cast<double*>(chunk1.data());
  data1[3]= 17;

  vil_memory_chunk chunk2 = chunk1;
  TEST("size()",chunk2.size(),25*sizeof(double));
  TEST("format",chunk2.pixel_format(),VIL_PIXEL_FORMAT_DOUBLE);
  double* data2 = reinterpret_cast<double*>(chunk2.data());
  TEST_NEAR("Deep Copy",data1[3],data2[3],1e-8);
}

TESTMAIN(test_memory_chunk);
