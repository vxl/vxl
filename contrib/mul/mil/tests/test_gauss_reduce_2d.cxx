#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>

#include <vnl/vnl_test.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>
#include <mil/mil_image_2d_of.h>
#include <mil/mil_gauss_reduce_2d.h>
#include <vil/vil_byte.h>


void test_gauss_reduce_2d_byte(int nx)
{
  vcl_cout << "*************************" << vcl_endl;
  vcl_cout << " Testing mil_gauss_reduce_2d (byte)(nx="<<nx<<")" << vcl_endl;
  vcl_cout << "*************************" << vcl_endl;

  mil_image_2d_of<vil_byte> image0;
  image0.resize(nx,3);
  mil_image_2d_of<vil_byte> reduced;
  reduced.resize((nx+1)/2,3);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
	 {
	   image0(x,y) = x+y*10;
	 }

  mil_gauss_reduce_2d(reduced.plane(0),reduced.xstep(),reduced.ystep(),
                    image0.plane(0),image0.nx(),image0.ny(),
					  image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"Reduced : "; reduced.print_all(vcl_cout);

  TEST("First element",reduced(0,1)==image0(0,1),true);
  TEST("Next element",reduced(1,1)==image0(2,1),true);
  int L = (nx-1)/2;
  TEST("Last element",reduced(L,1)==image0(2*L,1),true);


  mil_image_2d_of<vil_byte> test2;
  test2.resize(nx,3);
  test2.fill(222);
  mil_gauss_reduce_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                    image0.plane(0),image0.nx(),image0.ny(),
					  image0.xstep(),image0.ystep());
  TEST("No overrun",test2(L+1,1)==222,true);

}

void test_gauss_reduce_2d_float(int nx)
{
  vcl_cout << "*************************" << vcl_endl;
  vcl_cout << " Testing mil_gauss_reduce_2d (float)(nx="<<nx<<")" << vcl_endl;
  vcl_cout << "*************************" << vcl_endl;

  mil_image_2d_of<float> image0;
  image0.resize(nx,3);
  mil_image_2d_of<float> reduced;
  reduced.resize((nx+1)/2,3);

  for (int y=0;y<image0.ny();++y)
     for (int x=0;x<image0.nx();++x)
	 {
	   image0(x,y) = x+y*10;
	 }

  mil_gauss_reduce_2d(reduced.plane(0),reduced.xstep(),reduced.ystep(),
                    image0.plane(0),image0.nx(),image0.ny(),
					  image0.xstep(),image0.ystep());

  vcl_cout<<"Original: "; image0.print_all(vcl_cout);
  vcl_cout<<"Reduced : "; reduced.print_all(vcl_cout);

  TEST("First element",fabs(reduced(0,1)-image0(0,1))<1e-6,true);
  TEST("Next element",fabs(reduced(1,1)-image0(2,1))<1e-6,true);
  int L = (nx-1)/2;
  TEST("Last element",fabs(reduced(L,1)-image0(2*L,1))<1e-6,true);


  mil_image_2d_of<float> test2;
  test2.resize(nx,3);
  test2.fill(222);
  mil_gauss_reduce_2d(test2.plane(0),test2.xstep(),test2.ystep(),
                    image0.plane(0),image0.nx(),image0.ny(),
					  image0.xstep(),image0.ystep());
  TEST("No overrun",fabs(test2(L+1,1)-222)<1e-6,true);

}

void test_gauss_reduce_2d()
{
  test_gauss_reduce_2d_byte(7);
  test_gauss_reduce_2d_byte(6);
  test_gauss_reduce_2d_float(7);
  test_gauss_reduce_2d_float(6);
}


TESTMAIN(test_gauss_reduce_2d);
