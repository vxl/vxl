// This is mul/vil3d/io/tests/test_image_view_io.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vil3d/vil3d_image_view.h>
#include <vil3d/io/vil3d_io_image_view.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

template<class T>
inline void test_image_view_io_as(T value1, T value2)
{
  vcl_cout<<"Testing IO of images of type "<<vil_pixel_format_of(T())<<vcl_endl;
  vil3d_image_view<T> image1(8,7,3,2);
  image1.fill(value1);
  image1(4,3,2,1) = value2;

  vsl_b_ofstream bfs_out("vil3d_image_view_test_io.bvl.tmp");
  TEST("Created vil3d_image_view_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image1);
  bfs_out.close();

  vil3d_image_view<T> image2;
  vsl_b_ifstream bfs_in("vil3d_image_view_test_io.bvl.tmp");
  TEST("Opened vil3d_image_view_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, image2);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("vil3d_image_view_test_io.bvl.tmp");
#endif

  TEST("ni()",image2.ni(),image1.ni());
  TEST("nj()",image2.nj(),image1.nj());
  TEST("ni()",image2.nplanes(),image1.nplanes());
  TEST("istep()",image2.istep(),image1.istep());
  TEST("jstep()",image2.jstep(),image1.jstep());
  TEST("planestep()",image2.planestep(),image1.planestep());
  TEST_NEAR("Data(0,0,0)",image1(0,0,0,0),image2(0,0,0,0),1e-6);
  TEST_NEAR("Data(3,2,1)",image1(4,3,2,1),image2(4,3,2,1),1e-6);
}

static void test_image_view_io()
{
  vcl_cout << "*********************************\n"
           << " Testing IO for vil3d_image_view\n"
           << "*********************************\n";

  test_image_view_io_as(vxl_int_32(5),vxl_int_32(-17));
  test_image_view_io_as(vxl_int_16(11),vxl_int_16(-23));
  test_image_view_io_as(vxl_byte(3),vxl_byte(17));
  test_image_view_io_as(float(-0.6f),float(13.5f));
  test_image_view_io_as(double(12.1),double(123.456));
}

TESTMAIN(test_image_view_io);
