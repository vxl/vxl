// This is rpl/rrel/rrel_cauchy_obj.cxx
#include "rrel_cauchy_obj.h"

#include <vcl_cmath.h>

namespace {
  inline double sqr( double x ) { return x*x; }
}


rrel_cauchy_obj::rrel_cauchy_obj( double C )
  : C_( C )
{
}


rrel_cauchy_obj::~rrel_cauchy_obj()
{
}


double
rrel_cauchy_obj::rho( double u ) const
{
  return 0.5 * vcl_log( 1 + sqr( u/C_ ) );
}

double
rrel_cauchy_obj::wgt( double u ) const
{
  return 1.0 / ( 1 + sqr(u/C_) );
}
