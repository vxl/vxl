// This is core/vil/tests/test_resample_bicub.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_image_view.h>
#include <vil/vil_resample_bicub.h>

static void test_resample_bicub_byte()
{
  vcl_cout << "****************************\n"
           << " Testing vil_resample_bicub\n"
           << "****************************\n";

  vil_image_view<vxl_byte> image0;
  vil_image_view<double> dest_im;
  image0.set_size(10,10);

  vcl_cout<<"Testing one plane image\n";

  for (unsigned int j=0;j<image0.nj();++j)
     for (unsigned int i=0;i<image0.ni();++i)
       image0(i,j) = i+j*10;

  double x0 = 5.0, y0= 5.0;
  double dx1 = 1.0, dy1 = 0.0;
  double dx2 = 0.0, dy2 = 1.0;

  vcl_cout<<"Fully in image\n";
  vil_resample_bicub(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("Width",dest_im.ni(),4);
  TEST("Height",dest_im.nj(),3);
  TEST_NEAR("dest(0,0)",dest_im(0,0),55,1e-6);
  TEST_NEAR("dest(0,2)",dest_im(0,2),75,1e-6);
  TEST_NEAR("dest(3,2)",dest_im(3,2),78,1e-6);

  vil_resample_bicub(image0,dest_im,x0,y0,dx2,dy2,dx1,dy1,4,3);
  TEST_NEAR("dest(0,0)",dest_im(0,0),55,1e-6);
  TEST_NEAR("dest(0,2)",dest_im(0,2),57,1e-6);
  TEST_NEAR("dest(3,2)",dest_im(3,2),87,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_resample_bicub(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("dest_im(0,0)",dest_im(0,0),58,1e-6);
  TEST_NEAR("dest_im(3,2)",dest_im(3,2),0,1e-6);

  vcl_cout<<"Testing three plane image\n";

  image0.set_size(10,10,2);
  for (unsigned int j=0;j<image0.nj();++j)
    for (unsigned int i=0;i<image0.ni();++i)
      for (unsigned int p=0;p<2;++p)
       image0(i,j,p) = i+j*10+p*100;

  vcl_cout<<"Fully in image\n";
  x0 = 5.0;
  vil_resample_bicub(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST("Width",dest_im.ni(),4);
  TEST("Height",dest_im.nj(),3);
  TEST("nplanes",dest_im.nplanes(),2);
  TEST_NEAR("dest_im(0,0,0)",dest_im(0,0,0),55,1e-6);
  TEST_NEAR("dest_im(0,0,1)",dest_im(0,0,1),155,1e-6);
  TEST_NEAR("dest_im(1,0,0)",dest_im(1,0,0),56,1e-6);
  TEST_NEAR("dest_im(1,2,0)",dest_im(1,2,0),76,1e-6);
  TEST_NEAR("dest_im(3,2,1)",dest_im(3,2,1),178,1e-6);

  vcl_cout<<"At half pixel interval...\n";
  x0 = 5.5;
  vil_resample_bicub(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("dest_im(0,0,0)",dest_im(0,0,0),55.5,1e-6);
  TEST_NEAR("dest_im(0,0,1)",dest_im(0,0,1),155.5,1e-6);
  TEST_NEAR("dest_im(1,0,0)",dest_im(1,0,0),56.5,1e-6);
  TEST_NEAR("dest_im(1,2,0)",dest_im(1,2,0),76.5,1e-6);
  TEST_NEAR("dest_im(2,2,1)",dest_im(2,2,1),177.5,1e-6);

  vcl_cout<<"Beyond edge of image\n";
  x0 = 8;
  vil_resample_bicub(image0,dest_im,x0,y0,dx1,dy1,dx2,dy2,4,3);
  TEST_NEAR("dest_im(0,0,0)",dest_im(0,0,0),58,1e-6);
  TEST_NEAR("dest_im(3,2,1)",dest_im(3,2,1),0,1e-6);
}

static void test_resample_bicub()
{
  test_resample_bicub_byte();
}

TESTMAIN(test_resample_bicub);
