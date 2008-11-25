// This is rpl/rrel/rrel_cauchy_obj.cxx
#include "rrel_cauchy_obj.h"

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
  return rho_fast(u);
}

double
rrel_cauchy_obj::wgt( double u ) const
{
  return wgt_fast(u);
}
