//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <vnl/vnl_math.h>

#include <vul/vul_file.h>
#include <vcl_sstream.h>

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
//
////: Test changes
//bool test_edge2d(vcl_string grid_path,unsigned height, unsigned width,
//                 vnl_vector_fixed<double,3> axis,
//                 double angle)
//{
// //create a kernel and save it to a grid
//  bvpl_edge2d_kernel_factory kernel_factory(height, width); 
//  kernel_factory.create(axis)
//  kernel_factory.save_raw(grid_path);
//
//  return true;
//  
//}

MAIN(test_bvpl_kernels)
{
  //define variaty of heights, widths, and rotations
  unsigned h1=5, h2=3, h3 = 12;
  float a1=2.0*float(vnl_math::pi/3.0), a2=float(vnl_math::pi/4.0), a3=float(vnl_math::pi/2.0), a4=float(vnl_math::pi);
  
  vnl_vector_fixed<float,3> axis1(0.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis2(1.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis3(1.0, 0.0, 1.0);
  vnl_vector_fixed<float,3> axis4(1.0, 1.0 , 1.0);

  //Create the factory
  bvpl_edge2d_kernel_factory factory1(h1, h1); 



  for(int i = -1; i<1; i++)
    for(int j = 0; j<1; j++)
      for(int k = 0; k<1; k++)
      {
        //this should leave rotation axis untouched
        factory1.set_rotation_axis(vnl_vector_fixed<float,3>(float(i), float(j), float(k)));

        vnl_vector_fixed<float,3> axis = factory1.axis();
        vcl_cout <<axis << vcl_endl;
        //bool result = (vnl_vector_fixed<float,3>(float(i), float(j), float(k))== axis);
        //TEST("Zero axis", result, true);

        vcl_cout << "-----------------------------------\n";
        vcl_cout << "Rotating" <<i << j << k <<"\n";
        float angle = 0.0f;

        for(unsigned r=0; r<1; r++)
        {
          vcl_stringstream filename;
          vcl_cout << "angle " << angle << vcl_endl;
          filename << "rotation_" <<i << j << k << "_" << r <<".raw";
          factory1.set_angle(angle);
          bvpl_kernel kernel = factory1.create();
          kernel.print();
          kernel.save_raw(filename.str());
          angle = angle +  float(vnl_math::pi/2.0);
        }
      }
 



  
  //check the other creation method
  
 // 
//  vcl_cout <<"---------------------------------------- \n";
//  vcl_cout << "Running grid1 \n";
//  bool result = test_edge2d("grid1.raw", h1,h2,axis1, a1);
//  TEST("grid 1", result, true);
//  
//  vcl_cout <<"---------------------------------------- \n";
//  vcl_cout << "Running grid2 \n";
//  result = test_edge2d("grid2.raw", h1,h2,axis2, a2);
//  TEST("grid 2", result, true);
//  
//  vcl_cout <<"---------------------------------------- \n";
//  vcl_cout << "Running grid3 \n";
//  result = test_edge2d("grid3.raw", h1,h2, axis3, a3);
//  TEST("grid 3", result, true);
//  
//  vcl_cout <<"---------------------------------------- \n";
//  vcl_cout << "Running grid4 \n";
//  result = test_edge2d("grid4.raw", h1,h2, axis4 , a4);
//  TEST("grid 4", result, true);
//
   SUMMARY();
}



