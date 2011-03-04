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

static inline void print_binary_image(const vil_image_view<bool>& im)
{
  for (unsigned j=0;j<im.nj();++j)
  {
    for (unsigned i=0;i<im.ni();++i)
      if (im(i,j)) vcl_cout<<"X";
      else         vcl_cout<<".";
    vcl_cout<<vcl_endl;
  }
}

vil_image_view<bool> generate_test_image();

static inline unsigned blob_size( const vcl_vector<int> &bi,
                                  const vcl_vector<int> &bj )
{
  unsigned n_pixel = 0;

  vcl_vector<int>::const_iterator bi_iter;
  vcl_vector<int>::const_iterator bj_iter = bj.begin();
  for( bi_iter = bi.begin(); bi_iter != bi.end(); ++bi_iter, ++bj_iter )
    ++n_pixel;

  return n_pixel;
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

  vcl_cout<<"Test nested blobs."<<vcl_endl;
  image.fill(false);
  vil_crop(image, 3,6, 3,6).fill(true);
  vil_crop(image, 4,4, 4,4).fill(false);
  vil_crop(image, 5,2, 5,2).fill(true);
  finder.set_image(image);
  print_binary_image(image);
  TEST("Got a blob",finder.next_4con_region(bi,bj),true);
  TEST("Length of boundary (Square)",bi.size(),20);
  TEST("Got internal blob",finder.next_4con_region(bi,bj),true);
  TEST("Length of boundary (Square)",bi.size(),4);

  // Generic image
  {
    vil_image_view<bool> c_shape_image = generate_test_image();
    finder.set_image(c_shape_image);
    print_binary_image(c_shape_image);

    vcl_vector<int> bi, bj;
    vcl_vector<unsigned> n_pixel;

    while (finder.next_8con_region(bi,bj))
      n_pixel.push_back( blob_size( bi, bj ) );

    TEST("Number of blobs", n_pixel.size() == 1, true );
    bool size_ok = false;
    if( !n_pixel.empty() )
      size_ok = ( n_pixel[0] == 64 );

    TEST("Size of blob", size_ok, true );

  }
}

vil_image_view<bool> generate_test_image()
{
  vil_image_view<bool> image(15,25);
  image.fill(false);
  image( 3, 10 ) = true;
  image( 3, 11 ) = true;
  image( 3, 12 ) = true;
  image( 3, 13 ) = true;
  image( 3, 14 ) = true;
  image( 3, 15 ) = true;
  image( 3, 16 ) = true;
  image( 4, 8 ) = true;
  image( 4, 9 ) = true;
  image( 4, 10 ) = true;
  image( 4, 11 ) = true;
  image( 4, 12 ) = true;
  image( 4, 13 ) = true;
  image( 4, 14 ) = true;
  image( 4, 15 ) = true;
  image( 4, 16 ) = true;
  image( 4, 17 ) = true;
  image( 4, 18 ) = true;
  image( 5, 7 ) = true;
  image( 5, 8 ) = true;
  image( 5, 9 ) = true;
  image( 5, 10 ) = true;
  image( 5, 11 ) = true;
  image( 5, 12 ) = true;
  image( 5, 13 ) = true;
  image( 5, 14 ) = true;
  image( 5, 15 ) = true;
  image( 5, 16 ) = true;
  image( 5, 17 ) = true;
  image( 5, 18 ) = true;
  image( 5, 19 ) = true;
  image( 6, 6 ) = true;
  image( 6, 7 ) = true;
  image( 6, 8 ) = true;
  image( 6, 9 ) = true;
  image( 6, 10 ) = true;
  image( 6, 11 ) = true;
  image( 6, 12 ) = true;
  image( 6, 13 ) = true;
  image( 6, 14 ) = true;
  image( 6, 15 ) = true;
  image( 6, 16 ) = true;
  image( 6, 17 ) = true;
  image( 6, 18 ) = true;
  image( 6, 19 ) = true;
  image( 7, 5 ) = true;
  image( 7, 6 ) = true;
  image( 7, 7 ) = true;
  image( 7, 8 ) = true;
  image( 7, 18 ) = true;
  image( 7, 19 ) = true;
  image( 7, 20 ) = true;
  image( 8, 5 ) = true;
  image( 8, 6 ) = true;
  image( 8, 7 ) = true;
  image( 8, 20 ) = true;
  image( 8, 21 ) = true;
  image( 9, 5 ) = true;
  image( 9, 6 ) = true;
  image( 9, 21 ) = true;
  image( 10, 4 ) = true;
  image( 10, 5 ) = true;
  image( 11, 4 ) = true;
  image( 12, 4 ) = true;
  return image;
}

TESTMAIN(test_algo_blob_finder);
