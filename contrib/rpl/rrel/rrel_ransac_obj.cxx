// This is rpl/rrel/rrel_ransac_obj.cxx
#include "rrel_ransac_obj.h"

rrel_ransac_obj::rrel_ransac_obj( double scale_mult )
  : scale_mult_( scale_mult )
{
}

rrel_ransac_obj::~rrel_ransac_obj( ) = default;


double
rrel_ransac_obj::fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0;

  for ( ; res_begin != res_end; ++res_begin, ++scale_begin ) {
    double thres = *scale_begin * scale_mult_;
    if ( *res_begin < -thres || *res_begin > thres )
      sum += 1.0;
  }

  return sum;
}

double
rrel_ransac_obj::fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0;
  double thres = scale * scale_mult_;

  for ( ; begin != end; ++begin ) {
    if ( *begin < -thres || *begin > thres )
      sum += 1.0;
  }

  return sum;
}
