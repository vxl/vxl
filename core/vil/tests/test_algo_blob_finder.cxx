#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil/algo/vil_blob_finder.h>
#include <vil/vil_crop.h>

static void show_boundary(const vcl_vector<int>& bi,const vcl_vector<int>& bj)
{
  for (unsigned int i=0;i<bi.size();++i)
    vcl_cout<<" ("<<bi[i]<<','<<bj[i]<<')';
  vcl_cout<<vcl_endl;
}

static void test_algo_blob_finder()
{
  vcl_cout<<"=== Testing vil_blob_finder ===\n";
  vil_image_view<bool> image(10,11);

  // Create 3 x 3 square
  image.fill(false);
  vil_crop(image, 4,3, 5,3).fill(true);

  vcl_vector<int> bi,bj;
  vil_blob_finder finder(image);
  finder.longest_4con_boundary(bi,bj);

  show_boundary(bi,bj);

  TEST("Length of boundary (3x3)",bi.size(),8);

  // Create 1 x 5 line
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);

  finder.set_image(image);
  finder.longest_4con_boundary(bi,bj);
  show_boundary(bi,bj);
  TEST("Length of boundary (1x5)",bi.size(),8);

  // Make an L shape
  image(6,3)=true;
  finder.set_image(image);
  finder.longest_4con_boundary(bi,bj);
  show_boundary(bi,bj);
  TEST("Length of boundary (L shape)",bi.size(),10);

  // Make a T shape
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);
  image(6,5)=true;
  finder.set_image(image);
  finder.longest_4con_boundary(bi,bj);
  show_boundary(bi,bj);
  TEST("Length of boundary (T shape)",bi.size(),10);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 5,1, 0,10).fill(true);
  finder.set_image(image);
  finder.longest_4con_boundary(bi,bj);
  show_boundary(bi,bj);
  TEST("Length of boundary (Vertical line)",bi.size(),18);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 0,10, 5,1).fill(true);
  finder.set_image(image);
  finder.longest_4con_boundary(bi,bj);
  show_boundary(bi,bj);
  TEST("Length of boundary (Horizontal line)",bi.size(),18);
}


TESTMAIN(test_algo_blob_finder);
