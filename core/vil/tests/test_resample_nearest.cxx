// This is core/vil/tests/test_resample_nearest.cxx
#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_resample_nearest.h>

static void test_resample_nearest_byte()
{
  std::cout << "******************************\n"
           << " Testing vil_resample_nearest\n"
           << "******************************\n";

  vil_image_view<vxl_byte> image0;
  vil_image_view<double> dest_im;
  image0.set_size(10,10);

  std::cout<<"Testing one plane image\n";

  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      image0(i,j) = vxl_byte(i+j*10);

  double x0 = 5.0, y0= 5.0;
  double dx1 = 1.0, dy1 = 0.0;
  double dx2 = 0.0, dy2 = 1.0;

  std::cout<<"Fully in image\n";
  vil_resample_nearest(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("Width",dest_im.ni(),4);
  TEST("Height",dest_im.nj(),3);
  TEST("dest(0,0)",dest_im(0,0),55);
  TEST("dest(0,2)",dest_im(0,2),75);
  TEST("dest(3,2)",dest_im(3,2),78);

  vil_resample_nearest(image0,dest_im,x0,y0,dx2,dy2,dx1,dy1,4,3);
  TEST("dest(0,0)",dest_im(0,0),55);
  TEST("dest(0,2)",dest_im(0,2),57);
  TEST("dest(3,2)",dest_im(3,2),87);

  std::cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_resample_nearest(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("dest_im(0,0)",dest_im(0,0),58);
  TEST("dest_im(3,2)",dest_im(3,2),0);

  std::cout<<"Testing three plane image\n";

  image0.set_size(10,10,2);
  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      for (unsigned int p=0;p<2;++p)
        image0(i,j,p) = vxl_byte(i+j*10+p*100);

  std::cout<<"Fully in image\n";
  x0 = 5.0;
  vil_resample_nearest(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("Width",dest_im.ni(),4);
  TEST("Height",dest_im.nj(),3);
  TEST("nplanes",dest_im.nplanes(),2);
  TEST("dest_im(0,0,0)",dest_im(0,0,0),55);
  TEST("dest_im(0,0,1)",dest_im(0,0,1),155);
  TEST("dest_im(1,0,0)",dest_im(1,0,0),56);
  TEST("dest_im(1,2,0)",dest_im(1,2,0),76);
  TEST("dest_im(3,2,1)",dest_im(3,2,1),178);

  std::cout<<"At half pixel interval...\n";
  x0 = 5.5;
  vil_resample_nearest(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("dest_im(0,0,0)",dest_im(0,0,0),56);
  TEST("dest_im(0,0,1)",dest_im(0,0,1),156);
  TEST("dest_im(1,0,0)",dest_im(1,0,0),57);
  TEST("dest_im(1,2,0)",dest_im(1,2,0),77);
  TEST("dest_im(2,2,1)",dest_im(2,2,1),178);

  std::cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_resample_nearest(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("dest_im(0,0,0)",dest_im(0,0,0),58);
  TEST("dest_im(3,2,1)",dest_im(3,2,1),0);
}

static void test_resample_nearest()
{
  test_resample_nearest_byte();
}

TESTMAIN(test_resample_nearest);
