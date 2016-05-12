#ifndef bil_perform_tensor_decomposition_hxx_
#define bil_perform_tensor_decomposition_hxx_

#include "bil_perform_tensor_decomposition.h"
#include <vil/vil_math.h>
#include <vnl/vnl_math.h>

class vil_math_square_functor
{
 public:
  vxl_byte operator()(vxl_byte x) const { return vxl_byte(x*x); }
  unsigned operator()(unsigned x) const { return x*x; }
  int operator()(int x)           const { return x*x; }
  short operator()(short x)       const { return short(x*x); }
  float operator()(float x)       const { return x*x; }
  double operator()(double x)     const { return x*x; }
};


template <class T>
void bil_perform_tensor_decomposition( const vil_image_view<T>& k11,
                                       const vil_image_view<T>& k12,
                                       const vil_image_view<T>& k21, // FIXME - unused ?!?
                                       const vil_image_view<T>& k22,
                                       vil_image_view<T>& e1,
                                       vil_image_view<T>& e2,
                                       vil_image_view<T>& l1,
                                       vil_image_view<T>& l2)
{
  unsigned ni = k11.ni();
  unsigned nj = k11.nj();
  e1.set_size(ni,nj,2);
  e2.set_size(ni,nj,2);
  l1.set_size(ni,nj,1);
  l2.set_size(ni,nj,1);

  // t := (k11+k22)/2
  vil_image_view<T> t;
  vil_math_image_sum(k11,k22,t);
  vil_math_scale_values(t,0.5);

  // a := k11 - t
  vil_image_view<T> a;
  vil_math_image_difference(k11,t,a);
  vil_image_view<T> b; b.deep_copy(k12);

  // ab2 := sqrt(a.^2+b.^2);
  vil_image_view<T> a_sq; a_sq.deep_copy(a);
  vil_transform(a_sq,vil_math_square_functor());
  vil_image_view<T> b_sq; b_sq.deep_copy(b);
  vil_transform(b_sq,vil_math_square_functor());

  vil_image_view<T> ab2;
  vil_math_image_sum(a_sq,b_sq,ab2);
  vil_math_sqrt(ab2);

  // l1 := ab2 + t;
  vil_math_image_sum(t,ab2,l1);
  // l2 := t - ab2;
  vil_math_image_difference(t,ab2,l2);

  double theta;
  for (int j = 0; j < static_cast<int>(nj); j++) {
    for (int i = 0; i < static_cast<int>(ni); i++) {
      if (b(i,j) != 0) {
        theta = std::atan2( (double)(ab2(i,j)-a(i,j)), (double)b(i,j) );
      }
      else {
        theta = vnl_math::pi_over_2;
      }
      e1(i,j,0) =  T(std::cos(theta));
      e1(i,j,1) =  T(std::sin(theta));
      e2(i,j,0) = -e1(i,j,1);
      e2(i,j,1) =  e1(i,j,0);
    }
  }
}

#undef  BIL_PERFORM_TENSOR_DECOMPOSITION_INSTANTIATE
#define BIL_PERFORM_TENSOR_DECOMPOSITION_INSTANTIATE(T) \
template void bil_perform_tensor_decomposition(const vil_image_view<T >& k11,\
                                               const vil_image_view<T >& k12,\
                                               const vil_image_view<T >& k21,\
                                               const vil_image_view<T >& k22,\
                                               vil_image_view<T >& e1,\
                                               vil_image_view<T >& e2,\
                                               vil_image_view<T >& l1,\
                                               vil_image_view<T >& l2)

#endif // bil_perform_tensor_decomposition_hxx_
