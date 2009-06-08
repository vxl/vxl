//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel.h>
#include <vnl/vnl_math.h>

#include <vul/vul_file.h>

// True rotation matrices
static double R1_ [] =
{
1.0, 0.0, 0.0,
0.0, -1.0, 0.0,
0.0, 0.0, -1.0,
};

static double R2_ [] =
{
0.0, -0.5, 0.8660,
-0.7071, -0.6124, -0.3536,
0.7071, -0.6124, -0.3536,
};

static double R3_ [] =
{
-0.03536, -0.7071, 0.6124,
0.03536, -0.7071, -0.6124,
0.8660, 0.0, 0.05,
};

static double R4_ [] =
{
0.25, -0.8080, 0.5335,
0.4330, -0.39951, -0.8080,
0.8660, 0.4330, 0.25,
};

//: Test changes
bool test_edge2d(vcl_string grid_path,unsigned height, unsigned width,
                 vnl_vector_fixed<double,3> axis,
                 double angle)
{
 //create a kernel and save it to a grid
  bvpl_edge2d_kernel kernel(height, width, axis, angle); 
  vcl_cout<< "alive \n";
  kernel.create();
  kernel.save_raw(grid_path);
  
  //Compare rotations
  vnl_matrix_fixed<double,3,3> temp_R = kernel.rotation();

  return true;
  
}

MAIN(test_bvpl_kernels)
{
  //define variaty of heights, widths, and rotations
  unsigned h1=51, h2=3, h3 = 12;
  double a1=2.0*vnl_math::pi/3.0, a2=vnl_math::pi/4.0, a3=vnl_math::pi/2.0, a4=vnl_math::pi;
  
  vnl_vector_fixed<double,3> axis1(0.0, 0.0, 0.0);
  vnl_vector_fixed<double,3> axis2(1.0, 0.0, 0.0);
  vnl_vector_fixed<double,3> axis3(1.0, 0.0, 1.0);
  vnl_vector_fixed<double,3> axis4(1.0, 1.0 , 1.0);
  
//  //True matrices  
//  vnl_matrix_fixed<double,3,3> R1(R1_);
//  vnl_matrix_fixed<double,3,3> R2(R2_);
//  vnl_matrix_fixed<double,3,3> R3(R3_);
//  vnl_matrix_fixed<double,3,3> R4(R4_);
  
  vcl_cout <<"---------------------------------------- \n";
  vcl_cout << "Running grid1 \n";
  bool result = test_edge2d("grid1.raw", h1,h2,axis1, a1);
  TEST("grid 1", result, true);
  
  vcl_cout <<"---------------------------------------- \n";
  vcl_cout << "Running grid2 \n";
  result = test_edge2d("grid2.raw", h1,h2,axis2, a2);
  TEST("grid 2", result, true);
  
  vcl_cout <<"---------------------------------------- \n";
  vcl_cout << "Running grid3 \n";
  result = test_edge2d("grid3.raw", h1,h2, axis3, a3);
  TEST("grid 3", result, true);
  
  vcl_cout <<"---------------------------------------- \n";
  vcl_cout << "Running grid4 \n";
  result = test_edge2d("grid4.raw", h1,h2, axis4 , a4);
  TEST("grid 4", result, true);

  SUMMARY();
}



