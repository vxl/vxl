//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <vnl/vnl_math.h>

#include <vul/vul_file.h>
#include <vcl_sstream.h>

MAIN(test_bvpl_kernels)
{
  //define variaty of heights, widths, and rotations
  unsigned h1=5, h2=3, h3 = 12;
  float a1=2.0f*float(vnl_math::pi/3.0), a2=float(vnl_math::pi/4.0), a3=float(vnl_math::pi/2.0), a4=float(vnl_math::pi);
  
  vnl_vector_fixed<float,3> axis1(0.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis2(1.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis3(1.0, 0.0, 1.0);
  vnl_vector_fixed<float,3> axis4(1.0, 1.0 , 1.0);

  //Create the factory
  bvpl_edge2d_kernel_factory factory1(h1, h1); 



  for(int i = -1; i<1; i++)
    for(int j = -1; j<1; j++)
      for(int k = -1; k<1; k++)
      {
        factory1.set_rotation_axis(vnl_vector_fixed<float,3>(float(i), float(j), float(k)));

        vnl_vector_fixed<float,3> axis = factory1.axis();
        vcl_cout <<axis << vcl_endl;
       

        vcl_cout << "-----------------------------------\n";
        vcl_cout << "Rotating" <<i << j << k <<"\n";
        float angle = 0.0f;
        
        // it is not clear how to verify the result in a concrete manner
        // the user can comment out the printing statements to verify the output
        // the test is just making sure that the methods are not crashing
        for(unsigned r=0; r<1; r++)
        {
          vcl_stringstream filename;
          vcl_cout << "angle " << angle << vcl_endl;
          filename << "rotation_" <<i << j << k << "_" << r <<".raw";
          factory1.set_angle(angle);
          bvpl_kernel kernel = factory1.create();
          //kernel.print();
          //kernel.save_raw(filename.str());
          angle = angle +  float(vnl_math::pi/2.0);
        }
      }
 return 0;
}



