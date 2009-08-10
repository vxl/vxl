//:
// \file
#include <testlib/testlib_test.h>
#include <bvpl/bvpl_edge2d_kernel_factory.h>
#include <bvpl/bvpl_edge3d_kernel_factory.h>
#include <bvpl/bvpl_gauss3d_xx_kernel_factory.h>

#include <vnl/vnl_math.h>

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_iomanip.h>

bool test_edge2d()
{
  vnl_vector_fixed<float,3> axis1(0.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis2(1.0, 0.0, 0.0);
  vnl_vector_fixed<float,3> axis3(1.0, 0.0, 1.0);
  vnl_vector_fixed<float,3> axis4(1.0, 1.0, 1.0);
  
  //Create the factory
  unsigned h1=5;
  bvpl_edge2d_kernel_factory factory1(h1, h1);
  
  for (int i = -1; i<1; i++)
    for (int j = -1; j<1; j++)
      for (int k = -1; k<1; k++)
      {
        factory1.set_rotation_axis(vnl_vector_fixed<float,3>(float(i), float(j), float(k)));
        
        vnl_vector_fixed<float,3> axis = factory1.axis();
        vcl_cout << axis << vcl_endl
        << "-----------------------------------\n"
        << "Rotating" <<i << j << k << '\n';
        float angle = 0.0f;
        
        // it is not clear how to verify the result in a concrete manner
        // the user can comment out the printing statements to verify the output
        // the test is just making sure that the methods are not crashing
        for (unsigned r=0; r<1; r++)
        {
          vcl_stringstream filename;
          vcl_cout << "angle " << angle << vcl_endl;
          filename << "rotation_" <<i << j << k << '_' << r <<".raw";
          factory1.set_angle(angle);
          bvpl_kernel kernel = factory1.create();
          //kernel.print();
          //kernel.save_raw(filename.str());
          angle = angle +  float(vnl_math::pi/2.0);
        }
      }
  return true;
}


bool test_edge3d()
{
  unsigned h=31;
  bvpl_edge3d_kernel_factory kernel_3d(h,h,h);
  
  bvpl_kernel_vector_sptr kernel_3d_vecs=kernel_3d.create_kernel_vector();
  vcl_vector< vcl_pair<vnl_vector_fixed<float,3>, bvpl_kernel_sptr > >::iterator iter=kernel_3d_vecs->begin();
  
  for (;iter!=kernel_3d_vecs->end();iter++)
  {
    vnl_vector_fixed<float,3> axis=iter->first;
#if 0
    vcl_ostringstream s;
    s.precision(2);
    s << "d:/vj/scripts/Lidar_edges/test_masks/"
    << axis[0] << '_'
    << axis[1] << '_'
    << axis[2] << ".raw";
    
    iter->second->save_raw(s.str());
#endif
  }
  return true;

}
bool test_gaussian()
{
  float sigma1 = 10;
  float sigma2 = 11;
  float sigma3 = 12;
  bvpl_gauss3d_xx_kernel_factory factory(sigma1, sigma2, sigma3);
  bvpl_kernel kernel = factory.create();
#if 0 
  kernel.print_to_file("gauss_kernel.txt");
  kernel.save_raw("gauss_kernel.raw");
#endif
  return true;
}

MAIN(test_bvpl_kernels)
{
  TEST("Test edge2d kernel", true, test_edge2d());
  TEST("Test edge3d kernel", true, test_edge3d());
  TEST("Test gauss kernel", true,  test_gaussian());
  SUMMARY();
}



