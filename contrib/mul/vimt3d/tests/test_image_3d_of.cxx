// This is mul/vimt3d/tests/test_image_3d_of.cxx
#include <vcl_iostream.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vimt3d/vimt3d_image_3d_of.h>
#include <testlib/testlib_test.h>

#ifndef LEAVE_IMAGES_BEHIND
#define LEAVE_IMAGES_BEHIND 0
#endif

static void test_image_3d_of_byte()
{
  vcl_cout << "****************************\n"
           << " Testing vimt3d_image_3d_of\n"
           << "****************************\n";

  vimt3d_image_3d_of<vxl_byte> image(8,9,10),image2;

  vimt3d_transform_3d t;
  t.set_zoom_only(1,2,3,0,0,0);
  image.set_world2im(t);
  image.image().fill(17);
  image.image()(1,2,3)=27;

  vsl_b_ofstream bfs_out("test_image_3d_of.bvl.tmp");
  TEST("Opened test_image_3d_of.bvl.tmp for writing", (!bfs_out), false);
  vsl_b_write(bfs_out, image);
  TEST("Finished writing file successfully", (!bfs_out), false);
  bfs_out.close();

  vsl_b_ifstream bfs_in("test_image_3d_of.bvl.tmp");
  TEST("Opened test_image_3d_of.bvl.tmp for reading", (!bfs_in), false);
  vsl_b_read(bfs_in, image2);
  TEST("Finished reading file successfully", (!bfs_in), false);
  bfs_in.close();

  TEST("Image data", vil3d_image_view_deep_equality(image.image(),image2.image()),true);

  vimt3d_image_3d_of<vxl_byte> image3;
  image3.deep_copy(image);
  TEST("deep_copy()", vil3d_image_view_deep_equality(image.image(),image3.image()),true);

  image.set_valid_region(1,5, 2,6, 3,7);
  TEST("set_valid_region() Image ni()",image.image().ni(),5);
  TEST("set_valid_region() Image nj()",image.image().nj(),6);
  TEST("set_valid_region() Image nk()",image.image().nk(),7);

#if !LEAVE_IMAGES_BEHIND
  vpl_unlink("test_image_3d_of.bvl.tmp");
#endif
}

static void test_image_3d_of()
{
  test_image_3d_of_byte();
}

TESTMAIN(test_image_3d_of);
