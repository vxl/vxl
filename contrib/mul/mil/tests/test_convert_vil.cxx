// This is mul/mil/tests/test_convert_vil.cxx
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vil/vil_rgb_byte.h>
#include <vil/vil_memory_image_of.h>
#include <mil/mil_convert_vil.h>
#include <testlib/testlib_test.h>

void test_convert_vil()
{
  vcl_cout << "*************************\n"
           << " Testing mil_convert_vil\n"
           << "*************************\n";

  // test grey conversions

  int nx=20;
  int ny=20;
  mil_image_2d_of<float> in_image(nx,ny),out_image;
  // Fill image with shaded squares
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      in_image(x,y)=0.1f*((x%10)+(y%10));

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
  TEST_NEAR("Diff grey input/output",diff1,0,1e-6);

  double diff2=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff2+=vcl_fabs( in_image(x,y)-vil_image(x,y) );
  TEST_NEAR("Diff grey input/temp",diff2,0,1e-6);

  double diff3=0;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      diff3+=vcl_fabs( vil_image(x,y)-out_image(x,y) );
  TEST_NEAR("Diff grey temp/output",diff3,0,1e-6);


  // test colour conversions

  mil_image_2d_of<float> c_in_image,c_out_image;
  c_in_image.resize(nx,ny,3);

  // Fill image with shaded squares
  for (int p=0;p<3;++p)
    for (int y=0;y<ny;++y)
      for (int x=0;x<nx;++x)
        c_in_image(x,y,p)=0.1f*((x%10)+(y%10));

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
  //TEST("Diff colour input/output",c_in_image,c_out_image);
  TEST_NEAR("Diff colour input/output",diff1,0,1e-6);
  //TEST_NEAR("Diff colour input/temp",diff2,0,1e-6);
  //TEST_NEAR("Diff colour temp/output",diff3,0,1e-6);
}

TESTLIB_DEFINE_MAIN(test_convert_vil);
