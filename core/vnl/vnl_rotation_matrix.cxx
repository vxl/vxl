#include "vnl_rotation_matrix.h"
#include "vnl_identity_3x3.h"
#include "vnl_cross_product_matrix.h"

// returns exp([omega]_x), which is rotation by an angle ||omega||
// about the axis given by the 3-vector omega.
vnl_matrix<double> vnl_rotation_matrix::exp_of_skew(const vnl_vector<double> &omega)
{
  double theta=omega.magnitude();

  vnl_matrix<double> R(3,3);
  
  R.set_identity();
  
  if (theta == 0)
    return R;

  R += (cos(theta)-1)*(vnl_identity_3x3() - outer_product(omega,omega)/(theta*theta));
#if defined(VCL_GCC_27)
  R += sin(theta) * static_cast<vnl_matrix<double> >( vnl_cross_product_matrix(omega/theta) );
#else
  R += sin(theta) * vnl_cross_product_matrix(omega/theta);
#endif

  return R;
}
