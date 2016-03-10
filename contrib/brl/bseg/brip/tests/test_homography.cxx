// This is brl/bseg/brip/tests/test_homography.cxx
#define DEBUG
#include <vil1/vil1_memory_image_of.h>
#ifdef DEBUG
#include <vil1/vil1_save.h>
#endif
#include <brip/brip_vil1_float_ops.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vnl/algo/vnl_svd.h>
static void test_homography()
{
  const int w = 100, h = 100;
  vil1_memory_image_of<float> input(w,h), out;
  // diagonal periodic stripes
  int period = 5;
  for (int y = 0; y< h; y++)
    for (int x = 0; x<w; x++)
      input(x,y)=50;

  for (int p = 0; p<h; p+=period)
    for (int y = 0; y< h; y++)
      for (int x = 0; x<w; x++)
        if (x==y+p)
          input(x,y)=150;
  //first try simple rotation
  vnl_matrix_fixed<double,3, 3> M;
  M[0][0]= 0.6; M[0][1]= -0.8; M[0][2]= 70.7;
  M[1][0]= 0.8; M[1][1]=  0.6; M[1][2]= 0;
  M[2][0]= 0;   M[2][1]= 0;    M[2][2]= 1;
  vgl_h_matrix_2d<double> H(M);
  std::cout << "H\n" << H << std::endl;
  if (!brip_vil1_float_ops::homography(input, H, out))
  {
    std::cout << "homography failed\n";
    return;
  }

  vil1_memory_image_of<unsigned char> char_in = brip_vil1_float_ops::convert_to_byte(input);
  vil1_memory_image_of<unsigned char> char_out = brip_vil1_float_ops::convert_to_byte(out, 0, 255);
#ifdef DEBUG
  vil1_save(char_in, "./homg_input.tif");
  vil1_save(char_out, "./homg_out.tif");
#endif
#if 0
  vnl_matrix_fixed<double,4, 3> A;
  A[0][0]= 1.0 ;   A[0][1]= 0.0 ;   A[0][2]= 2.0 ;
  A[1][0]= 1.0;   A[1][1]= 1.0;   A[1][2]= 0.0;
  A[2][0]= 2.0;   A[2][1]= 0; A[2][2]= 4.1;
  A[3][0]= 2;   A[3][1]= 2.1 ; A[3][2]= 0.0;
  vnl_svd<double> SVD(A);
  vnl_matrix_fixed<double, 4,3> U = SVD.U();
  std::cout << "U\n" << U << '\n';

  vnl_matrix_fixed<double, 3,3> W = SVD.W();
  std::cout << "Sigma\n" << W << '\n';

  vnl_matrix_fixed<double, 3,3> V = SVD.V();
  std::cout << "V\n" << V << '\n';
  vnl_vector_fixed<double, 3> nv = SVD.nullvector() ;
  std::cout << "Nullvector " << nv << '\n'
           << "nullresidue " << A*nv << '\n';
#endif
}


TESTMAIN(test_homography);
