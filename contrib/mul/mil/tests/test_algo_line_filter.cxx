#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>

#include <vnl/vnl_test.h>
#include <mil/algo/mil_algo_line_filter.h>
#include <mil/mil_image_2d_of.h>

void test_algo_line_filter_byte()
{
  vcl_cout << "***********************" << vcl_endl;
  vcl_cout << " Testing mil_algo_line_filter (byte)" << vcl_endl;
  vcl_cout << "***********************" << vcl_endl;

  mil_algo_line_filter<unsigned char> filter;

  // Create test image
  int n = 10;
  mil_image_2d_of<unsigned char> image(n,n);
  image.fill(0);
  for (int i=0;i<n;++i) image(i,5)= 10;

  mil_image_2d_of<unsigned char> line_dir;
  mil_image_2d_of<float> line_str;

  filter.light_lines_3x3(line_dir,line_str,image);

  TEST("Size of line_dir",line_dir.nx()==n && line_dir.ny()==n,true);
  TEST("Size of line_str",line_str.nx()==n && line_str.ny()==n,true);
  TEST("Horizontal line",line_dir(5,5)==1,true);
  TEST("Horizontal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);
  TEST("Nearby points",line_dir(5,4)==0 && line_dir(5,6)==0,true);

  image.fill(0);
  line_dir.fill(77);
  for (int i=0;i<n;++i) image(5,i)= 10;
  filter.light_lines_3x3(line_dir,line_str,image);
  TEST("Vertical line",line_dir(5,5)==3,true);
  TEST("Vertical line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);
  TEST("Nearby points",line_dir(4,5)==0 && line_dir(6,5)==0,true);

  TEST("Left border",line_dir(0,5)==0,true);
  TEST("Right border",line_dir(n-1,5)==0,true);
  TEST("Bottom border",line_dir(5,0)==0,true);
  TEST("Left border",line_dir(5,n-1)==0,true);

  image.fill(0);
  for (int i=0;i<n;++i) image(i,i)= 10;
  filter.light_lines_3x3(line_dir,line_str,image);
  TEST("Diagonal line",line_dir(5,5)==2,true);
  TEST("Diagonal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(0);
  for (int i=0;i<n;++i) image(i,n-1-i)= 10;
  filter.light_lines_3x3(line_dir,line_str,image);
  TEST("Reverse Diagonal line",line_dir(5,4)==4,true);
  TEST("Reverse line str",vcl_fabs(line_str(5,4)-10)<1e-4,true);


  // ======================= dark_lines_3x3 ================
  image.fill(10);
  for (int i=0;i<n;++i) image(i,5)= 0;
  filter.dark_lines_3x3(line_dir,line_str,image);

  TEST("Size of line_dir",line_dir.nx()==n && line_dir.ny()==n,true);
  TEST("Size of line_str",line_str.nx()==n && line_str.ny()==n,true);
  TEST("Horizontal line",line_dir(5,5)==1,true);
  TEST("Horizontal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);
  TEST("Nearby points",line_dir(5,4)==0 && line_dir(5,6)==0,true);

  image.fill(10);
  line_dir.fill(77);
  for (int i=0;i<n;++i) image(5,i)= 00;
  filter.dark_lines_3x3(line_dir,line_str,image);
  TEST("Vertical line",line_dir(5,5)==3,true);
  TEST("Vertical line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);
  TEST("Nearby points",line_dir(4,5)==0 && line_dir(6,5)==0,true);

  TEST("Left border",line_dir(0,5)==0,true);
  TEST("Right border",line_dir(n-1,5)==0,true);
  TEST("Bottom border",line_dir(5,0)==0,true);
  TEST("Left border",line_dir(5,n-1)==0,true);

  image.fill(10);
  for (int i=0;i<n;++i) image(i,i)= 0;
  filter.dark_lines_3x3(line_dir,line_str,image);
  TEST("Diagonal line",line_dir(5,5)==2,true);
  TEST("Diagonal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(10);
  for (int i=0;i<n;++i) image(i,n-1-i)= 0;
  filter.dark_lines_3x3(line_dir,line_str,image);
  TEST("Reverse Diagonal line",line_dir(5,4)==4,true);
  TEST("Reverse line str",vcl_fabs(line_str(5,4)-10)<1e-4,true);

  // ======================= light_lines_5x5 ================
  image.fill(0);
  for (int i=0;i<n;++i) image(i,5)= 10;

  filter.light_lines_5x5(line_dir,line_str,image);

  TEST("Size of line_dir",line_dir.nx()==n && line_dir.ny()==n,true);
  TEST("Size of line_str",line_str.nx()==n && line_str.ny()==n,true);
  TEST("Horizontal line",line_dir(5,5)==1,true);
  TEST("Horizontal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(0);
  line_dir.fill(77);
  for (int i=0;i<n;++i) image(5,i)= 10;
  filter.light_lines_5x5(line_dir,line_str,image);
  TEST("Vertical line",line_dir(5,5)==3,true);
  TEST("Vertical line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  TEST("Left border",line_dir(1,5)==0,true);
  TEST("Right border",line_dir(n-2,5)==0,true);
  TEST("Bottom border",line_dir(5,1)==0,true);
  TEST("Left border",line_dir(5,n-2)==0,true);

  image.fill(0);
  for (int i=0;i<n;++i) image(i,i)= 10;
  filter.light_lines_5x5(line_dir,line_str,image);
  TEST("Diagonal line",line_dir(5,5)==2,true);
  TEST("Diagonal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(0);
  for (int i=0;i<n;++i) image(i,n-1-i)= 10;
  filter.light_lines_5x5(line_dir,line_str,image);
  TEST("Reverse Diagonal line",line_dir(5,4)==4,true);
  TEST("Reverse line str",vcl_fabs(line_str(5,4)-10)<1e-4,true);

  // ======================= dark_lines_5x5 ================
  image.fill(10);
  for (int i=0;i<n;++i) image(i,5)= 0;

  filter.dark_lines_5x5(line_dir,line_str,image);

  TEST("Size of line_dir",line_dir.nx()==n && line_dir.ny()==n,true);
  TEST("Size of line_str",line_str.nx()==n && line_str.ny()==n,true);
  TEST("Horizontal line",line_dir(5,5)==1,true);
  TEST("Horizontal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(10);
  line_dir.fill(77);
  for (int i=0;i<n;++i) image(5,i)= 0;
  filter.dark_lines_5x5(line_dir,line_str,image);
  TEST("Vertical line",line_dir(5,5)==3,true);
  TEST("Vertical line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  TEST("Left border",line_dir(1,5)==0,true);
  TEST("Right border",line_dir(n-2,5)==0,true);
  TEST("Bottom border",line_dir(5,1)==0,true);
  TEST("Left border",line_dir(5,n-2)==0,true);

  image.fill(10);
  for (int i=0;i<n;++i) image(i,i)= 0;
  filter.dark_lines_5x5(line_dir,line_str,image);
  TEST("Diagonal line",line_dir(5,5)==2,true);
  TEST("Diagonal line str",vcl_fabs(line_str(5,5)-10)<1e-4,true);

  image.fill(10);
  for (int i=0;i<n;++i) image(i,n-1-i)= 0;
  filter.dark_lines_5x5(line_dir,line_str,image);
  TEST("Reverse Diagonal line",line_dir(5,4)==4,true);
  TEST("Reverse line str",vcl_fabs(line_str(5,4)-10)<1e-4,true);

}

void test_algo_line_filter()
{
  test_algo_line_filter_byte();
}

TESTMAIN(test_algo_line_filter);
