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
  if ( u < -B_ || u > B_ )
    return 1.0;
  else
    return 1.0 - cube(1.0 - sqr(u/B_));
}

double
rrel_tukey_obj::wgt( double u ) const
{
  if ( u < -B_ || u > B_ )
    return 0.0;
  else
    return sqr(1.0 - sqr(u/B_));
}
