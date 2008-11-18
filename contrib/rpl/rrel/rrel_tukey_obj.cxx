// This is rpl/rrel/rrel_tukey_obj.cxx
#include "rrel_tukey_obj.h"

namespace {
  inline double sqr( double x ) { return x*x; }
  inline double cube( double x ) { return x*x*x; }
}


rrel_tukey_obj::rrel_tukey_obj( double B )
  : B_( B )
{
}


rrel_tukey_obj::~rrel_tukey_obj()
{
}


double
rrel_tukey_obj::rho( double u ) const
{
  return rho_fast(u);
}

double
rrel_tukey_obj::wgt( double u ) const
{
  return wgt_fast(u);
}
