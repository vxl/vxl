// This is core/vil/tests/test_algo_line_filter.cxx
#include <vcl_iostream.h>
#include <vil/algo/vil_line_filter.h>
#include <vil/vil_image_view.h>
#include <testlib/testlib_test.h>

static unsigned char black = 0, white = 10;

static void print_out(unsigned int n,
                      const char* s1, vil_image_view<unsigned char> a,
                      const char* s2, vil_image_view<unsigned char> b,
                      const char* s3, vil_image_view<float> c)
{
  vcl_cout << '\n';
  vcl_cout.width(n+5); vcl_cout << s1;
  vcl_cout.width(n+5); vcl_cout << s2;
  vcl_cout.width(n+5); vcl_cout << s3;
  for (unsigned int j=0; j<n; ++j)
  {
    vcl_cout << "\n ";
    vcl_cout.width(1);
    for (unsigned int i=0; i<n; ++i) vcl_cout << (int)(a(i,j)*9.9/(white-black));
    vcl_cout << "     ";
    vcl_cout.width(1);
    for (unsigned int i=0; i<n; ++i) vcl_cout << (int)b(i,j);
    vcl_cout << "     ";
    vcl_cout.width(1);
    for (unsigned int i=0; i<n; ++i) vcl_cout << (int)(c(i,j)*9/(white-black)+0.5);
  }
  vcl_cout << '\n';
  vcl_cout.width(0);
}

static void test_line_filter_byte()
{
  vcl_cout << "********************************\n"
           << " Testing vil_line_filter (byte)\n"
           << "********************************\n";

  vil_line_filter<unsigned char> filter;

  // Create test image
  unsigned int n = 10;
  vil_image_view<unsigned char> image(n,n);
  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,5)= white;

  vil_image_view<unsigned char> line_dir;
  vil_image_view<float> line_str;

  vcl_cout<<"----- light_lines_3x3() -------\n";

  filter.light_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Size of line_dir",line_dir.ni()==n && line_dir.nj()==n,true);
  TEST("Size of line_str",line_str.ni()==n && line_str.nj()==n,true);
  TEST("Horizontal line",line_dir(5,5),1);
  TEST_NEAR("Horizontal line str",line_str(5,5),white,1e-4);
  TEST("Nearby points",line_dir(5,4)==0 && line_dir(5,6)==0,true);

  image.fill(black);
  line_dir.fill(77);
  for (unsigned int i=0;i<n;++i) image(5,i)= white;
  filter.light_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Vertical line",line_dir(5,5),3);
  TEST_NEAR("Vertical line str",line_str(5,5),white,1e-4);
  TEST("Nearby points",line_dir(4,5)==0 && line_dir(6,5)==0,true);

  TEST("Left border",line_dir(0,5),0);
  TEST("Right border",line_dir(n-1,5),0);
  TEST("Bottom border",line_dir(5,0),0);
  TEST("Left border",line_dir(5,n-1),0);

  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,i)= white;
  filter.light_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Diagonal line",line_dir(5,5),2);
  TEST_NEAR("Diagonal line str",line_str(5,5),white,1e-4);

  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,n-1-i)= white;
  filter.light_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Reverse Diagonal line",line_dir(5,4),4);
  TEST_NEAR("Reverse line str",line_str(5,4),white,1e-4);

  // ======================= dark_lines_3x3 ================
  vcl_cout<<"----- dark_lines_3x3() -------\n";

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,5)= black;
  filter.dark_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Size of line_dir",line_dir.ni()==n && line_dir.nj()==n,true);
  TEST("Size of line_str",line_str.ni()==n && line_str.nj()==n,true);
  TEST("Horizontal line",line_dir(5,5),1);
  TEST_NEAR("Horizontal line str",line_str(5,5),white,1e-4);
  TEST("Nearby points",line_dir(5,4)==0 && line_dir(5,6)==0,true);

  image.fill(white);
  line_dir.fill(77);
  for (unsigned int i=0;i<n;++i) image(5,i)= black;
  filter.dark_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Vertical line",line_dir(5,5),3);
  TEST_NEAR("Vertical line str",line_str(5,5),white,1e-4);
  TEST("Nearby points",line_dir(4,5)==0 && line_dir(6,5)==0,true);

  TEST("Left border",line_dir(0,5),0);
  TEST("Right border",line_dir(n-1,5),0);
  TEST("Bottom border",line_dir(5,0),0);
  TEST("Left border",line_dir(5,n-1),0);

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,i)= black;
  filter.dark_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Diagonal line",line_dir(5,5),2);
  TEST_NEAR("Diagonal line str",line_str(5,5),white,1e-4);

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,n-1-i)= black;
  filter.dark_lines_3x3(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Reverse Diagonal line",line_dir(5,4),4);
  TEST_NEAR("Reverse line str",line_str(5,4),white,1e-4);

  // ======================= light_lines_5x5 ================
  vcl_cout<<"----- light_lines_5x5() -------\n";

  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,5)= white;
  filter.light_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Size of line_dir",line_dir.ni()==n && line_dir.nj()==n,true);
  TEST("Size of line_str",line_str.ni()==n && line_str.nj()==n,true);
  TEST("Horizontal line",line_dir(5,5),1);
  TEST_NEAR("Horizontal line str",line_str(5,5),white,1e-4);

  image.fill(black);
  line_dir.fill(77);
  for (unsigned int i=0;i<n;++i) image(5,i)= white;
  filter.light_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Vertical line",line_dir(5,5),3);
  TEST_NEAR("Vertical line str",line_str(5,5),white,1e-4);

  TEST("Left border",line_dir(1,5),0);
  TEST("Right border",line_dir(n-2,5),0);
  TEST("Bottom border",line_dir(5,1),0);
  TEST("Left border",line_dir(5,n-2),0);

  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,i)= white;
  filter.light_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Diagonal line",line_dir(5,5),2);
  TEST_NEAR("Diagonal line str",line_str(5,5),white,1e-4);

  image.fill(black);
  for (unsigned int i=0;i<n;++i) image(i,n-1-i)= white;
  filter.light_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Reverse Diagonal line",line_dir(5,4),4);
  TEST_NEAR("Reverse line str",line_str(5,4),white,1e-4);

  // ======================= dark_lines_5x5 ================
  vcl_cout<<"----- dark_lines_5x5() -------\n";

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,5)= black;

  filter.dark_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Size of line_dir",line_dir.ni()==n && line_dir.nj()==n,true);
  TEST("Size of line_str",line_str.ni()==n && line_str.nj()==n,true);
  TEST("Horizontal line",line_dir(5,5),1);
  TEST_NEAR("Horizontal line str",line_str(5,5),white,1e-4);

  image.fill(white);
  line_dir.fill(77);
  for (unsigned int i=0;i<n;++i) image(5,i)= 0;
  filter.dark_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Vertical line",line_dir(5,5),3);
  TEST_NEAR("Vertical line str",line_str(5,5),white,1e-4);

  TEST("Left border",line_dir(1,5),0);
  TEST("Right border",line_dir(n-2,5),0);
  TEST("Bottom border",line_dir(5,1),0);
  TEST("Left border",line_dir(5,n-2),0);

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,i)= black;
  filter.dark_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Diagonal line",line_dir(5,5),2);
  TEST_NEAR("Diagonal line str",line_str(5,5),white,1e-4);

  image.fill(white);
  for (unsigned int i=0;i<n;++i) image(i,n-1-i)= 0;
  filter.dark_lines_5x5(line_dir,line_str,image);

  print_out(n, "image:", image, "line_dir:", line_dir, "line_str:", line_str);

  TEST("Reverse Diagonal line",line_dir(5,4),4);
  TEST_NEAR("Reverse line str",line_str(5,4),white,1e-4);
}

static void test_algo_line_filter()
{
  test_line_filter_byte();
}

TESTMAIN(test_algo_line_filter);
