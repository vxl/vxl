//:
// \file
// \brief Test program using elements of several core libraries.
// \author Tim Cootes

// Aims to check that everything links properly from a
// single library containing most vxl/core libraries.
// (see associated CMakeLists.txt)

#include <iostream>
#include <vector>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_triangle_3d.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>

#include <vil/vil_image_view.h>
#include <vil/vil_transpose.h>
#include <vxl_config.h>
#include <vil/algo/vil_histogram.h>


int main(int argc, char** argv)
{
  std::cout<<"Test elements from vcl (std)"<<std::endl;
  std::vector<int> a(10);
  a[3]=7;

  // ========================================================
  {
    std::cout<<"Test elements from vgl."<<std::endl;

    vgl_point_2d<double> p(3,7);
    vgl_vector_2d<double> v(1,2);
    vgl_point_2d<double> q=p+v;

    // Use the following function as it is not templated (so must be in the library).
    vgl_point_3d<double> a1(0,0,0), a2(1,0,0), a3(0,1,0);

    double ar=vgl_triangle_3d_aspect_ratio(a1,a2,a3);
  }

  // ========================================================
  std::cout<<"Test elements from vnl, vnl_algo"<<std::endl;

  vnl_vector<double> x(10);
  x.fill(1.0);

  std::cout<<"Sum of elements: "<<x.sum()<<std::endl;

  vnl_matrix<double> A(2,2);
  A(0,0)=1; A(0,1)=2;
  A(1,0)=4; A(1,1)=3;
  std::cout<<"A: \n"<<A<<std::endl;


  std::cout<<"Inverse of A: \n"<<vnl_inverse(A)<<std::endl;

  vnl_svd<double> svd(A);
  std::cout<<"SVD: U\n"<<svd.U()<<std::endl;

  // ========================================================
  std::cout<<"Test elements from vil, vil_algo"<<std::endl;
  vil_image_view<float> image(10,10);
  image.fill(1.0f);
  image(5,5)=0.5f;

  vil_image_view<float> image2=vil_transpose(image);

  vil_image_view<vxl_byte> image3(20,20);
  for (unsigned j=0;j<20;++j)
    for (unsigned i=0;i<20;++i)
      image3(i,j)=i+j;

  std::vector<double> histo;
  vil_histogram(image3,histo,0,255,20);


  return 0;
}
