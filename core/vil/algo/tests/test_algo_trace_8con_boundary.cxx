#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <vil/algo/vil_trace_8con_boundary.h>
#include <vil/vil_crop.h>

static void show_boundary(const vcl_vector<int>& bi,const vcl_vector<int>& bj)
{
  for (unsigned int i=0;i<bi.size();++i)
    vcl_cout<<" ("<<bi[i]<<','<<bj[i]<<')';
  vcl_cout<<vcl_endl;
}

static void test_algo_trace_8con_boundary()
{
  vcl_cout<<"=== Testing vil_trace_8con_boundary ===\n";
  vil_image_view<bool> image(10,11);

  // Create 3 x 3 square
  image.fill(false);
  vil_crop(image, 4,3, 5,3).fill(true);

  vcl_vector<int> bi,bj;
  vil_trace_8con_boundary(bi,bj,image,5,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (3x3)",bi.size(),8);

  // Create 4 x 4 square
  image.fill(false);
  vil_crop(image, 4,4, 5,4).fill(true);

  vil_trace_8con_boundary(bi,bj,image,4,5);

  show_boundary(bi,bj);

  TEST("Length of boundary (4x4)",bi.size(),12);

  // Create 4 x 1 line
  image.fill(false);
  vil_crop(image, 4,4, 5,1).fill(true);

  vil_trace_8con_boundary(bi,bj,image,4,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (4x1)",bi.size(),6);


  // Check we get same length result if starting in middle
  vil_trace_8con_boundary(bi,bj,image,6,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (4x1) (middle)",bi.size(),6);

  // Create 1 x 5 line
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);

  vil_trace_8con_boundary(bi,bj,image,5,3);
  show_boundary(bi,bj);
  TEST("Length of boundary (1x5)",bi.size(),8);

  // Make an L shape
  image(6,3)=true;
  vil_trace_8con_boundary(bi,bj,image,6,3);
  show_boundary(bi,bj);
  TEST("Length of boundary (L shape)",bi.size(),9);

  // Make a T shape
  image.fill(false);
  vil_crop(image, 5,1, 3,5).fill(true);
  image(6,5)=true;
  vil_trace_8con_boundary(bi,bj,image,6,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (T shape)",bi.size(),8);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 5,1, 0,10).fill(true);
  vil_trace_8con_boundary(bi,bj,image,5,0);
  show_boundary(bi,bj);
  TEST("Length of boundary (Vertical line)",bi.size(),18);

  // Line up to edge
  image.fill(false);
  vil_crop(image, 0,10, 5,1).fill(true);
  vil_trace_8con_boundary(bi,bj,image,0,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Horizontal line)",bi.size(),18);

  // V shape
  image.fill(false);
  for (unsigned i=3;i<=5;++i) { image(i,i)=true; image(i,10-i)=true; }
  vil_trace_8con_boundary(bi,bj,image,5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (V)",bi.size(),8);

  // Worst case - diagonal cross, starting in centre
  image.fill(false);
  for (unsigned i=3;i<=7;++i) { image(i,i)=true; image(i,10-i)=true; }
  vil_trace_8con_boundary(bi,bj,image,5,5);
  show_boundary(bi,bj);
  TEST("Length of boundary (Diagonal cross)",bi.size(),16);
}


TESTMAIN(test_algo_trace_8con_boundary);
