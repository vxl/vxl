#ifndef bpgl_3d_from_disparity_hxx_
#define bpgl_3d_from_disparity_hxx_

#include "bpgl_3d_from_disparity.h"

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>

template<typename DISP_T>
vil_image_view<float> bpgl_3d_from_disparity(vpgl_affine_camera<double> const& cam1,
                                             vpgl_affine_camera<double> const& cam2,
                                             vil_image_view<DISP_T> const& disparity)
{
  // create matrix inverse of stacked affine projection matrices
  vnl_matrix_fixed<double,3,4> P1(cam1.get_matrix());
  vnl_matrix_fixed<double,3,4> P2(cam2.get_matrix());
  vnl_matrix_fixed<double,4,3> A;
  for (int r=0; r<2; ++r) {
    for (int c=0; c<3; ++c) {
      A[r][c] = P1[r][c];
      A[r+2][c] = P2[r][c];
    }
  }
  vnl_matrix_inverse<double> invA(A);

  const unsigned int ni = disparity.ni();
  const unsigned int nj = disparity.nj();
  vil_image_view<float> img3d(ni, nj, 3);
  for (int j=0; j<nj; ++j) {
    for (int i=0; i<ni; ++i) {
      double i2 = i - disparity(i,j);
      vnl_vector_fixed<double,3> x(NAN);
      if (i2 >= 0) {
        // valid disparity value
        vnl_vector<double> b(4);
        b[0] = i - P1[0][3];
        b[1] = j - P1[1][3];
        b[2] = i2 - P2[0][3];
        b[3] = j - P2[1][3];
        x = invA * b;
      }
      for (int d=0; d<3; ++d) {
        img3d(i,j,d) = x[d];
      }
    }
  }
  return img3d;
}

// explicit template instantiations macro
#define BPGL_3D_FROM_DISPARITY_INSTANIATE(DISP_T) \
template vil_image_view<float> \
bpgl_3d_from_disparity<DISP_T>(vpgl_affine_camera<double> const& cam1, \
                               vpgl_affine_camera<double> const& cam2, \
                               vil_image_view<DISP_T> const& disparity)

#endif
