// This is mul/vil2/tests/test_memory_chunk.cxx
#include <vcl_iostream.h>
#include <vxl_config.h>
#include <testlib/testlib_test.h>
#include <vil2/vil2_memory_chunk.h>
#include <vil2/io/vil2_io_memory_chunk.h>

void test_memory_chunk1()
{
  vcl_cout << "*******************************\n"
           << " Testing vil2_memory_chunk\n"
           << "*******************************\n";

  vil2_memory_chunk chunk1(35,VIL2_PIXEL_FORMAT_BYTE);
  TEST("size()",chunk1.size(),35);
  TEST("format",chunk1.pixel_format(),VIL2_PIXEL_FORMAT_BYTE);

  chunk1.resize(25*sizeof(double),VIL2_PIXEL_FORMAT_DOUBLE);
  TEST("size()",chunk1.size(),25*sizeof(double));
  TEST("format",chunk1.pixel_format(),VIL2_PIXEL_FORMAT_DOUBLE);

  double* data1 = (double*) chunk1.data();
  data1[3]= 17;

  vil2_memory_chunk chunk2 = chunk1;
  TEST("size()",chunk2.size(),25*sizeof(double));
  TEST("format",chunk2.pixel_format(),VIL2_PIXEL_FORMAT_DOUBLE);
  double* data2 = (double*) chunk2.data();
  TEST_NEAR("Deep Copy",data1[3],data2[3],1e-8);

}

template<class T>
inline void test_memory_chunk_io_as(T value)
{
  vcl_cout<<"Testing IO as type "<<vil2_pixel_format_of(T())<<vcl_endl;
  vil2_memory_chunk chunk1(35*sizeof(T),
    vil2_pixel_format_component_format(vil2_pixel_format_of(T())));
  T* data1 = (T*) chunk1.data();
  data1[3]= value;

  vsl_b_ofstream bfs_out("vil2_memory_chunk_test_io.bvl.tmp");
  TEST ("Created vil2_memory_chunk_test_io.bvl.tmp for writing",
        (!bfs_out), false);
  vsl_b_write(bfs_out, chunk1);
  bfs_out.close();

  vil2_memory_chunk chunk2;
  vsl_b_ifstream bfs_in("vil2_memory_chunk_test_io.bvl.tmp");
  TEST ("Opened vil2_memory_chunk_test_io.bvl.tmp for reading",
        (!bfs_in), false);
  vsl_b_read(bfs_in, chunk2);
  TEST ("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  T* data2 = (T*) chunk2.data();

  TEST("Size OK",chunk2.size()==chunk1.size(),true);
  TEST("Type OK", chunk1.pixel_format(),chunk2.pixel_format());
  TEST_NEAR("Data",data1[3],data2[3],1e-6);

}

void test_memory_chunk_io()
{
  vcl_cout << "*********************************\n"
           << " Testing IO for vil2_memory_chunk\n"
           << "*********************************\n";

  test_memory_chunk_io_as(vxl_uint_32(17));
  test_memory_chunk_io_as(vxl_int_32(-17));
  test_memory_chunk_io_as(vxl_uint_16(19));
  test_memory_chunk_io_as(vxl_int_16(-23));
  test_memory_chunk_io_as(vxl_byte(17));
  test_memory_chunk_io_as(vxl_sbyte(153));
  test_memory_chunk_io_as(float(13.5f));
  test_memory_chunk_io_as(double(123.456));
  test_memory_chunk_io_as(bool(true));
}

MAIN( test_memory_chunk )
{
  START( "vil2_memory_chunk" );
  test_memory_chunk1();
  test_memory_chunk_io();

  SUMMARY();
}
