// This is rpl/rrel/rrel_trunc_quad_obj.cxx
#include "rrel_trunc_quad_obj.h"

rrel_trunc_quad_obj::rrel_trunc_quad_obj( double T )
  : T_sqr_( T*T )
{
}


rrel_trunc_quad_obj::~rrel_trunc_quad_obj() = default;


double
rrel_trunc_quad_obj::rho( double u ) const
{
  return rho_fast(u);
}

double
rrel_trunc_quad_obj::wgt( double u ) const
{
  return wgt_fast(u);
}
