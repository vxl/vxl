// This is rpl/rrel/rrel_trunc_quad_obj.cxx
#include "rrel_trunc_quad_obj.h"

rrel_trunc_quad_obj::rrel_trunc_quad_obj( double T )
  : T_sqr_( T*T )
{
}


rrel_trunc_quad_obj::~rrel_trunc_quad_obj()
{
}


double
rrel_trunc_quad_obj::rho( double u ) const
{
  double u_sqr = u*u;
  return ( u_sqr < T_sqr_ ) ? u_sqr : T_sqr_;
}

double
rrel_trunc_quad_obj::wgt( double u ) const
{
  return ( u*u < T_sqr_ ) ? 1 : 0.0;
}
