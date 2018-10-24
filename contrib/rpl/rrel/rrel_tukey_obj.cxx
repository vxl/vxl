// This is rpl/rrel/rrel_tukey_obj.cxx
#include "rrel_tukey_obj.h"


rrel_tukey_obj::rrel_tukey_obj( double B )
  : B_( B )
{
}


rrel_tukey_obj::~rrel_tukey_obj() = default;


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
