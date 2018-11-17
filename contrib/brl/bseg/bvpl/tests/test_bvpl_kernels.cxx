//:
// \file
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <testlib/testlib_test.h>
#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>
#include <bvpl/kernels/bvpl_corner2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_xx_kernel_factory.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>

#include <vnl/vnl_math.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool test_edge2d()
{
  vnl_float_3 axis1(0.0, 0.0, 0.0);
  vnl_float_3 axis2(1.0, 0.0, 0.0);
  vnl_float_3 axis3(1.0, 0.0, 1.0);
  vnl_float_3 axis4(1.0, 1.0, 1.0);

  //Create the factory
  unsigned h1=5;
  bvpl_edge2d_kernel_factory factory1(h1, h1);

  for (int i = -1; i<1; i++)
    for (int j = -1; j<1; j++)
      for (int k = -1; k<1; k++)
      {
        factory1.set_rotation_axis(vnl_float_3(float(i), float(j), float(k)));

        vnl_float_3 axis = factory1.axis();
        std::cout << axis << std::endl
                 << "-----------------------------------\n"
                 << "Rotating" <<i << j << k << '\n';
        float angle = 0.0f;

        // it is not clear how to verify the result in a concrete manner
        // the user can comment out the printing statements to verify the output
        // the test is just making sure that the methods are not crashing
        for (unsigned r=0; r<1; r++)
        {
          std::stringstream filename;
          std::cout << "angle " << angle << std::endl;
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
  bvpl_edge3d_kernel_factory kernel_3d(-15,15,-15,15,-15,15);
  bvpl_create_directions_b dir;
  bvpl_kernel_vector_sptr kernel_3d_vecs = kernel_3d.create_kernel_vector(dir);

#if 0
  std::vector< bvpl_kernel_sptr >::iterator iter=kernel_3d_vecs->begin();

  for (;iter!=kernel_3d_vecs->end();iter++)
  {
    vnl_float_3 axis=(*iter)->axis();
    std::ostringstream s;
    s.precision(2);
    s << "D:/vj/scripts/Lidar_edges/test_masks/"
      << axis[0] << '_'
      << axis[1] << '_'
      << axis[2] << ".raw";

    (*iter)->save_raw(s.str());
  }
#endif
  return true;
}

//: A function to print some kernel to file and check whether they look ok
void print_gauss_xx_kernels()
{
  float sigma1 = 10;
  float sigma2 = 15;
  float sigma3 = 20;
  bvpl_gauss3d_xx_kernel_factory factory(sigma1, sigma2, sigma3);

  {
    factory.set_rotation_axis( vnl_float_3(1, 1, 1));
    bvpl_kernel kernel = factory.create();

    kernel.save_raw("gauss_111_kernel.raw");
    //kernel.print_to_file("gauss_111_kernel.txt");
    std::cout << "1 1 1 kernel " ;
    kernel.cum_sum();
  }

  {
    factory.set_rotation_axis( vnl_float_3(0, 0, 1));
    bvpl_kernel kernel = factory.create();
    kernel.save_raw("gauss_001_kernel.raw");
    //kernel.print_to_file("gauss_001_kernel.txt");
    std::cout << "0 0 1 kernel " ;
    kernel.cum_sum();
  }

  {
    factory.set_rotation_axis( vnl_float_3(0, 0, 1));
    factory.set_angle(float(vnl_math::pi_over_2));
    bvpl_kernel kernel = factory.create();
    kernel.save_raw("gauss_001_90_kernel.raw");
    //kernel.print_to_file("gauss_001_90_kernel.txt");
    std::cout << "0 0 1 kernel, angle pi/2 " ;
    kernel.cum_sum();
  }
}

//: A function to print some kernel to file and check whether they look ok
void print_gauss_x_kernels()
{
  float sigma1 = 10;
  float sigma2 = 15;
  float sigma3 = 20;
  bvpl_gauss3d_x_kernel_factory factory(sigma1, sigma2, sigma3);

  {
    factory.set_rotation_axis( vnl_float_3(1, 1, 1));
    bvpl_kernel kernel = factory.create();

    kernel.save_raw("gauss_111_kernel.raw");
    //kernel.print_to_file("gauss_111_kernel.txt");
    std::cout << "1 1 1 kernel " ;
    kernel.cum_sum();
  }

  {
    factory.set_rotation_axis( vnl_float_3(0, 0, 1));
    bvpl_kernel kernel = factory.create();
    kernel.save_raw("gauss_001_kernel.raw");
    //kernel.print_to_file("gauss_001_kernel.txt");
    std::cout << "0 0 1 kernel " ;
    kernel.cum_sum();
  }

  {
    factory.set_rotation_axis( vnl_float_3(0, 0, 1));
    factory.set_angle(float(vnl_math::pi_over_2));
    bvpl_kernel kernel = factory.create();
    kernel.save_raw("gauss_001_90_kernel.raw");
    //kernel.print_to_file("gauss_001_90_kernel.txt");
    std::cout << "0 0 1 kernel, angle pi/2 " ;
    kernel.cum_sum();
  }
}

//: Prints to file the directions of the kernels
//  The file can be read in MATLAB for visualization
void print_directions( const bvpl_kernel_vector_sptr& kernel_vector)
{
  auto vit = kernel_vector->kernels_.begin();

  std::cout <<"Writing to file axes in vector:" << std::endl;

  std::string filename = "kernel_axes.txt";
  std::fstream ofs(filename.c_str(), std::ios::out);

  if (!ofs.is_open()) {
    std::cerr << "error opening filefor write!\n";
  }


  for (; vit!=kernel_vector->kernels_.end(); ++vit)
  {
    vnl_float_3 coord = (*vit)->axis();
    ofs.precision(2);
    ofs << coord[0] << ' ' << coord[1] << ' ' << coord[2] << '\n';
  }
  ofs.close();
}

bool test_gaussian_xx()
{
  float sigma1 = 1;
  float sigma2 = 1.5;
  float sigma3 = 2;
  bvpl_gauss3d_xx_kernel_factory factory(sigma1, sigma2, sigma3);
  {
    bvpl_kernel kernel = factory.create();
    std::cout << "Canonical kernel " ;
    kernel.cum_sum();
  }

#if 0   //Comment this out if you whish to print kernels to raw file for visualization
  print_kernels();
#endif

  // test the kernel vector

  bvpl_create_directions_b dir;
  bvpl_kernel_vector_sptr kernel_3d_vecs = factory.create_kernel_vector(dir);

#if 0   //Comment this out if you whish to print kernels directions for visualization
  print_directions(kernel_vector);
#endif

  //check symmetry assumptions.
  // 1. if kernel has two equal sigmas and it is aligned in the
  // direction of unequal sigma, then it is symmetric around that axis.
  bvpl_gauss3d_xx_kernel_factory factory2(1, 1.5);
  factory2.set_rotation_axis( vnl_float_3(1, 0, 0));
  bvpl_kernel kernel1 = factory2.create();

  factory2.set_rotation_axis( vnl_float_3(1, 0, 0));
  factory2.set_angle(float(vnl_math::pi_over_2));
  bvpl_kernel kernel2 = factory2.create();

  factory2.set_rotation_axis( vnl_float_3(-1, 0, 0));
  bvpl_kernel kernel3 = factory2.create();

  //kernel1.save_raw("kernel1.raw");
  //kernel2.save_raw("kernel2.raw");
  //kernel1.print_to_file("kernel1.txt");
  //kernel2.print_to_file("kernel2.txt");

  //check equality
  bvpl_kernel_iterator kernel_iter1 = kernel1.iterator();
  bvpl_kernel_iterator kernel_iter2 = kernel2.iterator();
  bvpl_kernel_iterator kernel_iter3 = kernel3.iterator();

  //reset the iterator
  kernel_iter1.begin();
  kernel_iter2.begin();
  kernel_iter3.begin();

  bool symmetric = true;

  while (!kernel_iter1.isDone()) {
    vgl_point_3d<int> idx1 = kernel_iter1.index();
    while (!kernel_iter2.isDone()) {
      vgl_point_3d<int> idx2 = kernel_iter2.index();
      if (idx1 == idx2)
      {
        bvpl_kernel_dispatch d1 = *kernel_iter1;
        bvpl_kernel_dispatch d2 = *kernel_iter2;
        if (d1.c_ - d2.c_ > std::numeric_limits<float>::epsilon())
          symmetric = false;
      }
      ++kernel_iter2;
    }
    kernel_iter2.begin();
    ++kernel_iter1;
  }

  kernel_iter1.begin();
  while (!kernel_iter1.isDone()) {
    vgl_point_3d<int> idx1 = kernel_iter1.index();
    while (!kernel_iter3.isDone()) {
      vgl_point_3d<int> idx3= kernel_iter3.index();
      if (idx1 == idx3)
      {
        bvpl_kernel_dispatch d1 = *kernel_iter1;
        bvpl_kernel_dispatch d3 = *kernel_iter3;
        if (d1.c_ - d3.c_ > std::numeric_limits<float>::epsilon())
          symmetric = false;
      }
      ++kernel_iter3;
    }
    kernel_iter3.begin();
    ++kernel_iter1;
  }


  TEST("Symmetry test", symmetric, true);


  factory2.set_rotation_axis( vnl_float_3(-1, 0, 0));

  return true;
}

bool test_gaussian_x()
{
  float sigma1 = 10;
  float sigma2 = 15;
  float sigma3 = 20;
  bvpl_gauss3d_x_kernel_factory factory(sigma1, sigma2, sigma3);
  {
    bvpl_kernel kernel = factory.create();
    std::cout << "Canonical kernel " ;
    kernel.cum_sum();
    kernel.save_raw("canonical.raw");
  }

#if 1   //Comment this out if you whish to print kernels to raw file for visualization

  print_gauss_x_kernels();
#endif

  // test the kernel vector
//
//  bvpl_create_directions_b dir;
//  bvpl_kernel_vector_sptr kernel_3d_vecs = factory.create_kernel_vector(dir);
//
//#if 0   //Comment this out if you whish to print kernels directions for visualization
//  print_directions(kernel_vector);
//#endif

  //check symmetry assumptions.
  // 1. if kernel has two equal sigmas and it is aligned in the
  // direction of unequal sigma, then it is symmetric around that axis.
  bvpl_gauss3d_xx_kernel_factory factory2(1, 1.5);
  factory2.set_rotation_axis( vnl_float_3(1, 0, 0));
  bvpl_kernel kernel1 = factory2.create();

  factory2.set_rotation_axis( vnl_float_3(1, 0, 0));
  factory2.set_angle(float(vnl_math::pi_over_2));
  bvpl_kernel kernel2 = factory2.create();

  factory2.set_rotation_axis( vnl_float_3(-1, 0, 0));
  bvpl_kernel kernel3 = factory2.create();

  //kernel1.save_raw("kernel1.raw");
  //kernel2.save_raw("kernel2.raw");
  //kernel1.print_to_file("kernel1.txt");
  //kernel2.print_to_file("kernel2.txt");

  //check equality
  bvpl_kernel_iterator kernel_iter1 = kernel1.iterator();
  bvpl_kernel_iterator kernel_iter2 = kernel2.iterator();
  bvpl_kernel_iterator kernel_iter3 = kernel3.iterator();

  //reset the iterator
  kernel_iter1.begin();
  kernel_iter2.begin();
  kernel_iter3.begin();

  bool symmetric = true;

  while (!kernel_iter1.isDone()) {
    vgl_point_3d<int> idx1 = kernel_iter1.index();
    while (!kernel_iter2.isDone()) {
      vgl_point_3d<int> idx2 = kernel_iter2.index();
      if (idx1 == idx2)
      {
        bvpl_kernel_dispatch d1 = *kernel_iter1;
        bvpl_kernel_dispatch d2 = *kernel_iter2;
        if (d1.c_ - d2.c_ > std::numeric_limits<float>::epsilon())
          symmetric = false;
      }
      ++kernel_iter2;
    }
    kernel_iter2.begin();
    ++kernel_iter1;
  }

  kernel_iter1.begin();
  while (!kernel_iter1.isDone()) {
    vgl_point_3d<int> idx1 = kernel_iter1.index();
    while (!kernel_iter3.isDone()) {
      vgl_point_3d<int> idx3= kernel_iter3.index();
      if (idx1 == idx3)
      {
        bvpl_kernel_dispatch d1 = *kernel_iter1;
        bvpl_kernel_dispatch d3 = *kernel_iter3;
        if (d1.c_ - d3.c_ > std::numeric_limits<float>::epsilon())
          symmetric = false;
      }
      ++kernel_iter3;
    }
    kernel_iter3.begin();
    ++kernel_iter1;
  }


  TEST("Symmetry test", symmetric, true);


  factory2.set_rotation_axis( vnl_float_3(-1, 0, 0));

  return true;
}


bool test_corner2d()
{
  unsigned length = 5;
  unsigned width = 7;
  unsigned thickness = 3;

  {
    bvpl_corner2d_kernel_factory factory(length, width, thickness);
    factory.set_angle(float(vnl_math::pi_over_4));
    bvpl_kernel kernel = factory.create();
    std::cout << "Canonical kernel " ;
    kernel.cum_sum();
  }

#if 0  //Comment this out if you whish to print kernels to raw file for visualization
  {
    bvpl_corner2d_kernel_factory factory1(50,35,25);
    factory1.set_rotation_axis( vnl_float_3(0, 1, 0));
    bvpl_kernel kernel = factory1.create();

    kernel.save_raw("corner_010_kernel.raw");
    kernel.print_to_file("cornel_010_kernel.txt");
    std::cout << "0 1 0 kernel " ;
    kernel.cum_sum();
  }
#endif

  // test the kernel vector

//  bvpl_create_directions_b dir;
//  bvpl_kernel_vector_sptr kernel_3d_vecs = factory.create_kernel_vector(dir);

#if 0   //Comment this out if you whish to print kernels directions for visualization
  print_directions(kernel_vector);
#endif

  return true;
}


static void test_bvpl_kernels()
{
  //TEST("Test edge2d kernel",   true, test_edge2d());
  //TEST("Test edge3d kernel",   true, test_edge3d());
  TEST("Test gauss_x kernel",  true, test_gaussian_x());
 // TEST("Test gauss_xx kernel", true, test_gaussian_xx());
  //TEST("Test corner2d kernel", true, test_corner2d());
}

TESTMAIN(test_bvpl_kernels);
