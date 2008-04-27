#include "bil_detect_ridges.h"
#include "bil_perform_tensor_decomposition.h"
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil/vil_bilin_interp.h>

void bil_detect_ridges(const vil_image_view<float>& Ix,
                       const vil_image_view<float>& Iy,
                       const vil_image_view<float>& Ixx,
                       const vil_image_view<float>& Iyy,
                       const vil_image_view<float>& Ixy,
                       float sigma,
                       float epsilon,
                       vil_image_view<int>& rho_int,
                       vil_image_view<float>& ex,
                       vil_image_view<float>& ey,
                       vil_image_view<float>& lambda)
{
  unsigned ni = Ix.ni();
  unsigned nj = Ix.nj();

  //compute the eigenvectors and eigenvalues of the Hessian
  //[e1,e2,l1,l2] = perform_tensor_decomp(H);
  vil_image_view<float> e1;
  vil_image_view<float> e2;
  vil_image_view<float> l1;
  vil_image_view<float> l2;

  bil_perform_tensor_decomposition(Ixx,
                                   Ixy,
                                   Ixy,
                                   Iyy,
                                   e1,
                                   e2,
                                   l1,
                                   l2);

  vil_image_view<float> max_eigen_x(ni,nj,1);
  vil_image_view<float> max_eigen_y(ni,nj,1);
  vil_image_view<float> max_lambda(ni,nj,1);

  for (unsigned j =0; j < nj; j++) {
    for (unsigned i =0; i < ni; i++) {
      if (vcl_fabs(l1(i,j)) < vcl_fabs(l2(i,j))) {
        max_eigen_x(i,j) = e2(i,j,0);
        max_eigen_y(i,j) = e2(i,j,1);
        max_lambda(i,j) = l2(i,j);
      }
      else {
        max_eigen_x(i,j) = e1(i,j,0);
        max_eigen_y(i,j) = e1(i,j,1);
        max_lambda(i,j) = l1(i,j);
      }
    }
  }

  vil_image_view<float> Ix_plus_e(ni,nj);
  vil_image_view<float> Iy_plus_e(ni,nj);
  vil_image_view<float> Ix_minus_e(ni,nj);
  vil_image_view<float> Iy_minus_e(ni,nj);

  const float* xdata = Ix.top_left_ptr();
  const float* ydata = Iy.top_left_ptr();
  vcl_ptrdiff_t x_istep = Ix.istep();
  vcl_ptrdiff_t y_istep = Iy.istep();
  vcl_ptrdiff_t x_jstep = Ix.jstep();
  vcl_ptrdiff_t y_jstep = Iy.jstep();

  for (unsigned j =0; j < nj; j++) {
    for (unsigned i =0; i < ni; i++) {
      double y_ind_p = j + epsilon*max_eigen_y(i,j);
      double y_ind_m = j - epsilon*max_eigen_y(i,j);
      double x_ind_p = i + epsilon*max_eigen_x(i,j);
      double x_ind_m = i - epsilon*max_eigen_x(i,j);
      Ix_plus_e(i,j)  = (float)vil_bilin_interp_safe(x_ind_p , y_ind_p , xdata , ni , nj , x_istep , x_jstep);
      Iy_plus_e(i,j)  = (float)vil_bilin_interp_safe(x_ind_p , y_ind_p , ydata , ni , nj , y_istep , y_jstep);
      Ix_minus_e(i,j) = (float)vil_bilin_interp_safe(x_ind_m , y_ind_m , xdata , ni , nj , x_istep , x_jstep);
      Iy_minus_e(i,j) = (float)vil_bilin_interp_safe(x_ind_m , y_ind_m , ydata , ni , nj , y_istep , y_jstep);
    }
  }

  // grad(I).nu (at +epsilon)
  vil_image_view<float> gI_dot_nu_plus_e;
  vil_image_view<float> Ix_p_times_ex;
  vil_image_view<float> Iy_p_times_ey;
  vil_math_image_product(Ix_plus_e,max_eigen_x,Ix_p_times_ex);
  vil_math_image_product(Iy_plus_e,max_eigen_y,Iy_p_times_ey);
  vil_math_image_sum(Ix_p_times_ex,Iy_p_times_ey,gI_dot_nu_plus_e);

  // grad(I).nu (at -epsilon)
  vil_image_view<float> gI_dot_nu_minus_e;
  vil_image_view<float> Ix_m_times_ex;
  vil_image_view<float> Iy_m_times_ey;
  vil_math_image_product(Ix_minus_e,max_eigen_x,Ix_m_times_ex);
  vil_math_image_product(Iy_minus_e,max_eigen_y,Iy_m_times_ey);
  vil_math_image_sum(Ix_m_times_ex,Iy_m_times_ey,gI_dot_nu_minus_e);

  // compute rho
  vil_image_view<float> rho(ni,nj);

#define SIGN(X) (X < 0? -1 : (X==0 ? 0 : 1))
  for (unsigned j =0; j < nj; j++) {
    for (unsigned i =0; i < ni; i++) {
      rho(i,j) = -0.5f*SIGN(max_lambda(i,j))*vcl_abs(SIGN(gI_dot_nu_plus_e(i,j)) - SIGN(gI_dot_nu_minus_e(i,j)));
    }
  }

#undef SIGN

  ex = max_eigen_x;
  ey = max_eigen_y;
  lambda = max_lambda;
  vil_convert_cast(rho,rho_int);
}

