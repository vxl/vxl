// This is rpl/rrel/rrel_m_est_obj.cxx
#include "rrel_m_est_obj.h"

double
rrel_m_est_obj::fcn( vect_const_iter res_begin, vect_const_iter res_end,
                     vect_const_iter scale_begin,
                     vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0;

  for ( ; res_begin != res_end; ++res_begin, ++scale_begin ) {
    sum += rho( *res_begin / *scale_begin );
  }

  return sum;
}

double
rrel_m_est_obj::fcn( vect_const_iter res_begin, vect_const_iter res_end,
                     double scale,
                     vnl_vector<double>* /*param_vector*/ ) const
{
  double sum = 0;

  for ( ; res_begin != res_end; ++res_begin ) {
    sum += rho( *res_begin, scale );
  }

  return sum;
}

void
rrel_m_est_obj::wgt( vect_const_iter res_begin, vect_const_iter res_end,
                     vect_const_iter scale_begin,
                     vect_iter wgt_begin ) const
{
  for ( ; res_begin != res_end; ++res_begin, ++scale_begin, ++wgt_begin ) {
    *wgt_begin = wgt( *res_begin, *scale_begin );
  }
}

void
rrel_m_est_obj::wgt( vect_const_iter res_begin, vect_const_iter res_end,
                     double scale,
                     vect_iter wgt_begin ) const
{
  for ( ; res_begin != res_end; ++res_begin, ++wgt_begin ) {
    *wgt_begin = wgt( *res_begin, scale );
  }
}
