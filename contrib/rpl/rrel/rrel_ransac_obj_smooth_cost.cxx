// This is rpl/rrel/rrel_ransac_obj_smooth_cost.cxx
#include "rrel_ransac_obj_smooth_cost.h"
#include <vnl/vnl_math.h>

double
rrel_ransac_obj_smooth_cost::fcn( vect_const_iter res_begin,
                                  vect_const_iter res_end,
                                  vect_const_iter scale_begin,
                                  vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0.0;

  for ( ; res_begin != res_end; ++res_begin, ++scale_begin ) {
    double thres = *scale_begin * scale_mult_;
    if ( *res_begin < -thres || *res_begin > thres )
      sum += 1.0;
    else sum+= vnl_math_sqr(*res_begin/thres);
  }
  return sum;
}

double
rrel_ransac_obj_smooth_cost::fcn( vect_const_iter res_begin,
                                  vect_const_iter res_end,
                                  double scale,
                                  vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0.0;
  double thres = scale * scale_mult_;

  for ( ; res_begin != res_end; ++res_begin ) {
    if ( *res_begin < -thres || *res_begin > thres )
      sum += 1.0;
    else sum+= vnl_math_sqr(*res_begin/thres);
  }
  return sum;
}
