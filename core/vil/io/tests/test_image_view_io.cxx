// This is core/vil/io/tests/test_image_view_io.cxx
#include <testlib/testlib_test.h>

#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vil/vil_image_view.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_plane.h>

#ifndef LEAVE_FILES_BEHIND
#define LEAVE_FILES_BEHIND 0
#endif

template<class T>
inline void test_image_view_io_as(T value1, T value2)
{
  vcl_cout<<"Testing IO of images of type "<<vil_pixel_format_of(T())<<vcl_endl;
  vil_image_view<T> image1(15,17,3);
  image1.fill(value1);
  image1(3,2,1) = value2;
  vil_image_view<T> image1p = vil_plane(image1,1);

  vsl_b_ofstream bfs_out("vil_image_view_test_io.bvl.tmp");
  TEST("Created vil_image_view_test_io.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image1);
  vsl_b_write(bfs_out, image1p);
  bfs_out.close();

  vil_image_view<T> image2, image2p;
  vsl_b_ifstream bfs_in("vil_image_view_test_io.bvl.tmp");
  TEST("Opened vil_image_view_test_io.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, image2);
  vsl_b_read(bfs_in, image2p);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();
#if !LEAVE_FILES_BEHIND
  vpl_unlink("vil_image_view_test_io.bvl.tmp");
#endif

  TEST("ni()",image2.ni(),image1.ni());
  TEST("ni()",image2p.ni(),image2.ni());
  TEST("nj()",image2.nj(),image1.nj());
  TEST("nj()",image2p.nj(),image2.nj());
  TEST("nplanes()",image2.nplanes(),image1.nplanes());
  TEST("nplanes()",image2p.nplanes(),1);
  TEST("istep()",image2.istep(),image1.istep());
  TEST("jstep()",image2.jstep(),image1.jstep());
  TEST("planestep()",image2.planestep(),image1.planestep());
  TEST_NEAR("Data(0,0,0)",image1(0,0,0),image2(0,0,0),1e-6);
  TEST_NEAR("Data(3,2,1)",image1(3,2,1),image2(3,2,1),1e-6);
  TEST("Smart ptr", &image2p(0,0), &image2(0,0,1));
}

static void test_image_view_io()
{
  vcl_cout << "*******************************\n"
           << " Testing IO for vil_image_view\n"
           << "*******************************\n";

#if VXL_HAS_INT_64
  test_image_view_io_as(vxl_uint_64(3),vxl_uint_64(17));
  test_image_view_io_as(vxl_int_64(5),vxl_int_64(-17));
#endif
  test_image_view_io_as(vxl_uint_32(3),vxl_uint_32(17));
  test_image_view_io_as(vxl_int_32(5),vxl_int_32(-17));
  test_image_view_io_as(vxl_uint_16(4),vxl_uint_16(19));
  test_image_view_io_as(vxl_int_16(11),vxl_int_16(-23));
  test_image_view_io_as(vxl_byte(3),vxl_byte(17));
  test_image_view_io_as(vxl_sbyte(14),vxl_sbyte(153));
  test_image_view_io_as(float(-0.6f),float(13.5f));
  test_image_view_io_as(double(12.1),double(123.456));
  test_image_view_io_as(bool(false),bool(true));
}

TESTMAIN(test_image_view_io);
