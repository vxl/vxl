// This is core/vil/algo/tests/test_algo_flood_fill.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/algo/vil_flood_fill.h>

// Checks that every pixel in im1 with value v1 has value v2 in image v2
bool compare(const vil_image_view<vxl_byte>& im1,
             const vil_image_view<vxl_byte>& im2,
             vxl_byte v1, vxl_byte v2)
{
  if (im1.ni()!=im2.ni()) return false;
  if (im1.nj()!=im2.nj()) return false;
  for (unsigned j=0;j<im1.nj();++j)
    for (unsigned i=0;i<im1.ni();++i)
    {
      if (im1(i,j)==v1 && im2(i,j)!=v2) return false;
      if (im1(i,j)!=v1 && im2(i,j)==v2) return false;
    }

  return true;
}

static void test_flood_fill4_byte()
{
  std::cout << "*************************\n"
           << " Testing vil_flood_fill4\n"
           << "*************************\n";

  unsigned n=10;
  vil_image_view<vxl_byte> image0,image1;
  image0.set_size(n,n);
  image0.fill(0);

  // Create a connected square
  for (unsigned j=3;j<=7;++j)
    for (unsigned i=3;i<=7;++i)
      image0(i,j)=1;

  image1.deep_copy(image0);

  // Replace all elements with value 1, connected to (5,5) with value 2
  vil_flood_fill4(image1,5,5, vxl_byte(1), vxl_byte(2));

  TEST("Flood fill square",compare(image0,image1,1,2),true);

  // Create indent
  image0(3,4)=0;
  image0(4,4)=0;

  image1.deep_copy(image0);

  // Replace all elements with value 1, connected to (5,5) with value 2
  vil_flood_fill4(image1,5,5, vxl_byte(1), vxl_byte(2));

  TEST("Flood fill square+indent",compare(image0,image1,1,2),true);

  // Create hole
  image0(6,6)=0;

  image1.deep_copy(image0);

  // Replace all elements with value 1, connected to (5,5) with value 2
  vil_flood_fill4(image1,5,5, vxl_byte(1), vxl_byte(2));

  TEST("Flood fill square+hole",compare(image0,image1,1,2),true);
}

static void test_flood_fill8_byte()
{
  std::cout << "*************************\n"
           << " Testing vil_flood_fill8\n"
           << "*************************\n";

  unsigned n=10;
  vil_image_view<vxl_byte> image0,image1;
  image0.set_size(n,n);
  image0.fill(0);

  // Create a connected square
  for (unsigned j=3;j<=7;++j)
    for (unsigned i=3;i<=7;++i)
      if ((i+j)%2==0) image0(i,j)=1;

  image1.deep_copy(image0);

  // Replace all elements with value 1, connected to (5,5) with value 2
  vil_flood_fill8(image1,5,5, vxl_byte(1), vxl_byte(2));

  TEST("Flood fill square",compare(image0,image1,1,2),true);

  // Create hole
  image0(5,5)=0;

  image1.deep_copy(image0);

  // Replace all elements with value 1, connected to (5,5) with value 2
  vil_flood_fill8(image1,4,4, vxl_byte(1), vxl_byte(2));

  TEST("Flood fill square+hole",compare(image0,image1,1,2),true);
}

static void test_algo_flood_fill()
{
  test_flood_fill4_byte();
  test_flood_fill8_byte();
}

TESTMAIN(test_algo_flood_fill);
