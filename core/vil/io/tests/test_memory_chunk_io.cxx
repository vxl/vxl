// This is mul/vil2/io/tests/test_memory_chunk_io.cxx
#include <testlib/testlib_test.h>

#include <vil2/vil2_memory_chunk.h>
#include <vil2/io/vil2_io_memory_chunk.h>
#include <vil2/io/vil2_io_smart_ptr.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // for memset()
#include <vxl_config.h>
#include <vpl/vpl.h> // vpl_unlink()

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

template<class T>
inline void test_memory_chunk_io_as(T value)
{
  vcl_cout<<"Testing IO as type "<<vil2_pixel_format_of(T())<<vcl_endl;
  vil2_memory_chunk chunk1(35*sizeof(T),
    vil2_pixel_format_component_format(vil2_pixel_format_of(T())));
  T* data1 = (T*) chunk1.data();
  vcl_memset(data1,0,35*sizeof(T)); // avoid "UMR" on subsequent vsl_b_write()
  data1[3]= value;
  vil2_memory_chunk_sptr chunk_sptr1 = new vil2_memory_chunk(chunk1);

  vsl_b_ofstream bfs_out("vil2_memory_chunk_test_io.bvl.tmp");
  TEST("Created vil2_memory_chunk_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, chunk1);
  vsl_b_write(bfs_out, chunk_sptr1);
  bfs_out.close();

  vil2_memory_chunk chunk2;
  vil2_memory_chunk_sptr chunk_sptr2;

  vsl_b_ifstream bfs_in("vil2_memory_chunk_test_io.bvl.tmp");
  TEST("Opened vil2_memory_chunk_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, chunk2); vcl_cout<<"Read in chunk2"<<vcl_endl;
  vsl_b_read(bfs_in, chunk_sptr2);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("vil2_memory_chunk_test_io.bvl.tmp");
#endif

  T* data2 = (T*) chunk2.data();

  TEST("Size OK",chunk2.size()==chunk1.size(),true);
  TEST("Type OK", chunk1.pixel_format(),chunk2.pixel_format());
  TEST_NEAR("Data",data1[3],data2[3],1e-6);

  vcl_cout<<"Testing IO using smart pointer"<<vcl_endl;
  TEST("Size OK",chunk_sptr2->size()==chunk1.size(),true);
  TEST("Type OK", chunk_sptr2->pixel_format(),chunk1.pixel_format());
}

MAIN( test_memory_chunk_io )
{
  START( "vil2_memory_chunk" );

  vcl_cout << "**********************************\n"
           << " Testing IO for vil2_memory_chunk\n"
           << "**********************************\n";

  test_memory_chunk_io_as(vxl_uint_32(17));
  test_memory_chunk_io_as(vxl_int_32(-17));
  test_memory_chunk_io_as(vxl_uint_16(19));
  test_memory_chunk_io_as(vxl_int_16(-23));
  test_memory_chunk_io_as(vxl_byte(17));
  test_memory_chunk_io_as(vxl_sbyte(153));
  test_memory_chunk_io_as(float(13.5f));
  test_memory_chunk_io_as(double(123.456));
  test_memory_chunk_io_as(bool(true));

  SUMMARY();
}
