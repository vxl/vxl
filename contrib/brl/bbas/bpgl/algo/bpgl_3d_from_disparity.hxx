#ifndef bpgl_3d_from_disparity_hxx_
#define bpgl_3d_from_disparity_hxx_

#include "bpgl_3d_from_disparity.h"

#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>

template<typename T, typename CAM_T>
vil_image_view<T> bpgl_3d_from_disparity(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity)
{
  // create matrix inverse of stacked projection matrices
  vnl_matrix_fixed<double,3,4> P0(cam1.get_matrix());
  vnl_matrix_fixed<double,3,4> P1(cam2.get_matrix());
  size_t ni = disparity.ni(), nj = disparity.nj();
  vil_image_view<T> img3d(ni, nj, 3);
  img3d.fill(NAN);
  if(cam1.type_name()=="vpgl_affine_camera"){
    vnl_matrix_fixed<double,4,3> A;
    for (int r=0; r<2; ++r) {
      for (int c=0; c<3; ++c) {
        A[r][c] = P0[r][c];
        A[r+2][c] = P1[r][c];
      }
    }
    vnl_matrix_inverse<double> invA(A.as_ref());
    for (size_t j=0; j<nj; ++j) {
      for (size_t i=0; i<ni; ++i) {
        //4-30-2019 jlm changed i - disparity to i + disparity to be consistent with
        //the disparity computed by bsgm_disparity_estimator
        double i2 = i + disparity(i,j);
        vnl_vector_fixed<double,3> x(NAN);
        
        // could check against maximum valid value here as well, if we knew the size of the second image.
        if (i2 >= 0) {
          // valid disparity value
          vnl_vector<double> b(4);
          b[0] = i - P0[0][3];
          b[1] = j - P1[1][3];
          b[2] = i2 - P1[0][3];
          b[3] = j - P1[1][3];
          x = invA * b;
        }
        for (int d=0; d<3; ++d) {
          img3d(i,j,d) = T(x[d]);
        }
      }
    }
    return img3d;
  }
  else if (cam1.type_name() == "vpgl_perspective_camera") {
#if 0
    vnl_matrix_fixed<double, 3, 3> M0, M1;
    vnl_matrix_fixed<double, 3, 1> t0, t1;
    for (size_t r = 0; r < 3; ++r) {
      t0[r][0] = P0[r][3];
      t1[r][0] = P1[r][3];
      for (size_t c = 0; c < 3; ++c) {
        M0[r][c] = P0[r][c];
        M1[r][c] = P1[r][c];
      }
    }
    vnl_matrix_inverse<double> Mi0_inv(M0.as_ref());
    vnl_matrix_inverse<double> Mi1_inv(M1.as_ref());
    vnl_matrix_fixed<double, 3, 3> M0_inv(Mi0_inv.as_matrix());
    vnl_matrix_fixed<double, 3, 3> M1_inv(Mi1_inv.as_matrix());
    vnl_matrix_fixed<double, 3, 3> M0mt = M0_inv.transpose(), M1mt = M1_inv.transpose();
    vnl_matrix_fixed<double, 3, 3> M0mtM0i = M0mt * M0_inv, M1mtM1i = M1mt * M1_inv, M0mtM1i = M0mt * M1_inv;
#endif
    vpgl_proj_camera<double> pp0(P0), pp1(P1);
    for (size_t j = 0; j < nj; ++j) {
      for (size_t i = 0; i < ni; ++i) {
        if (i == 41 && j == 41)
          int nonsense = 0;
        //4-30-2019 jlm changed i - disparity to i + disparity to be consistent with
        //the disparity computed by bsgm_disparity_estimator
        // could check against maximum valid value here as well, if we knew the size of the second image.
        double i2 = i + disparity(i, j);
        if (i2 >= 0) {
          vgl_point_2d<double> x0(i, j), x1(i2, j);
          vgl_point_3d<double> p3d = triangulate_3d_point(pp0, x0, pp1, x1);
          img3d(i, j, 0) = p3d.x(); img3d(i, j, 1) = p3d.y();img3d(i, j, 2) = p3d.z();
        }
      }
#if 0
      vnl_matrix_fixed<double, 3, 1> x0, x1;//image points
      x0[0][0] = i;  x0[1][0] = j; x0[2][0] = 1.0;
      x1[0][0] = i2; x1[1][0] = j; x1[2][0] = 1.0;
      vnl_matrix_fixed<double, 2, 2> A;//data matrix for ray intersection
      A[0][0] = (x0.transpose() * M0mtM0i * x0)[0][0];
      A[0][1] = (x0.transpose() * M0mtM1i * x1)[0][0];
      A[1][0] = A[1][0];
      A[1][1] = (x1.transpose() * M1mtM1i * x1)[0][0];
      vnl_vector_fixed<double, 2> b;//constant vector 
      b[0] = (x0.transpose() * M0mtM0i * t0 - x0.transpose() * M0mtM1i * t1)[0][0];
      b[1] = (x1.transpose() * (M0mtM1i.transpose()) * t0 - x1.transpose() * M1mtM1i * t1)[0][0];
      vnl_matrix_inverse<double> Ainv(A.as_ref());
      //ray parameters at intersection
      vnl_vector<double> lambda = Ainv.solve(b.as_ref());
      vnl_matrix_fixed<double, 3, 1> Xint = M0_inv * (lambda[0] * x0 - t0);
      for (int d = 0; d < 3; ++d) {
        img3d(i, j, d) = T(Xint[d][0]);

      }
    }
  }
}
  }
#endif
    }
  }
  
  return img3d;
}
template<typename T, typename CAM_T>
vil_image_view<T> bpgl_3d_from_disparity_with_scalar(
    CAM_T const& cam1,
    CAM_T const& cam2,
    vil_image_view<T> const& disparity,
    vil_image_view<T> const& scalar)
{
  const unsigned int ni = disparity.ni();
  const unsigned int nj = disparity.nj();
  vil_image_view<T> img3d(ni, nj, 4);
  vil_image_view<T> img3d3;
  img3d3 = bpgl_3d_from_disparity(cam1, cam2, disparity);
  for(size_t j = 0; j<nj; ++j)
    for (size_t i = 0; i < ni; ++i) {
      for(size_t p =0; p<3; ++p)
        img3d(i,j,p) = img3d3(i,j,p);
      img3d(i,j, 3) = scalar(i,j);
    }
  return img3d;
}

// explicit template instantiations macro
#define BPGL_3D_FROM_DISPARITY_INSTANIATE(T, CAM_T)   \
template vil_image_view<T> \
bpgl_3d_from_disparity<T, CAM_T>( \
    CAM_T const& cam1, \
    CAM_T const& cam2, \
    vil_image_view<T> const& disparity);\
template vil_image_view<T> \
bpgl_3d_from_disparity_with_scalar<T, CAM_T>( \
    CAM_T const& cam1,\
    CAM_T const& cam2,\
    vil_image_view<T> const& disparity,\
    vil_image_view<T> const& scalar)

#endif
