#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vil/vil_rgb_byte.h>

#include <testlib/testlib_test.h>
#include <mil/mil_convert_vil.h>
#include <vil/vil_memory_image_of.h>


void test_convert_vil()
{
  vcl_cout << "**********************" << vcl_endl;
  vcl_cout << " Testing mil_convert_vil" << vcl_endl;
  vcl_cout << "**********************" << vcl_endl;

  // test grey conversions

  int nx=20;
  int ny=20;
  mil_image_2d_of<float> in_image(nx,ny),out_image;
  // Fill image with shaded squares
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      in_image(x,y)=(x%10)+(y%10);

  //out_image.deepCopy(image);
  //out_image.print_summary(vcl_cout);

  // Convert Image
  vil_memory_image_of<float> vil_image;
  mil_convert_vil_gm2gv(vil_image, in_image);
  mil_convert_vil_gv2gm(out_image, vil_image);


  // Calc Total difference over all pixels
  double diff1=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff1+=vcl_fabs( in_image(x,y)-out_image(x,y) );

  double diff2=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff2+=vcl_fabs( in_image(x,y)-vil_image(x,y) );

  double diff3=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff3+=vcl_fabs( vil_image(x,y)-out_image(x,y) );


  TEST("Diff grey input/output",diff1<1e-6,true);
  TEST("Diff grey input/temp",diff2<1e-6,true);
  TEST("Diff grey temp/output",diff3<1e-6,true);


  // test colour conversions

  mil_image_2d_of<float> c_in_image,c_out_image;
  c_in_image.resize(nx,ny,3);

  // Fill image with shaded squares
  for (int p=0;p<3;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        c_in_image(x,y,p)=(x%10)+(y%10);

  //out_image.deepCopy(image);
  //out_image.print_summary(vcl_cout);

  // Convert Image
  vil_memory_image_of<vil_rgb_byte> c_vil_image;
  mil_convert_vil_cm2cv(c_vil_image, c_in_image);
  mil_convert_vil_cv2cm(c_out_image, c_vil_image);

  // Calc Total difference over all pixels
  double diff4=0;
  for (int p=0;p<3;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        diff4+=vcl_fabs( c_in_image(x,y,p)-c_out_image(x,y,p) );

#if 0
  double diff5=0;
  for (int p=0;p<3;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        diff5+=vcl_fabs( c_in_image(x,y,p)-c_vil_image(x,y,p) );

  double diff6=0;
  for (int p=0;p<3;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        diff6+=vcl_fabs( c_vil_image(x,y,p)-c_out_image(x,y,p) );
#endif

  // nb can't use == operator because only valid for shallow copies
  // or same data
  //TEST("Diff colour input/output",c_in_image==c_out_image,true);
  TEST("Diff colour input/output",diff1<1e-6,true);
  //TEST("Diff colour input/temp",diff2<1e-6,true);
  //TEST("Diff colour temp/output",diff3<1e-6,true);
}

TESTLIB_DEFINE_MAIN(test_convert_vil);
