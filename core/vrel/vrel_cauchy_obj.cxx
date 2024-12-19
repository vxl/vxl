// This is core/vrel/vrel_cauchy_obj.cxx
#include "vrel_cauchy_obj.h"


vrel_cauchy_obj::vrel_cauchy_obj(double C)
  : C_(C)
{}


vrel_cauchy_obj::~vrel_cauchy_obj() = default;


double
vrel_cauchy_obj::rho(double u) const
{
  return rho_fast(u);
}

double
vrel_cauchy_obj::wgt(double u) const
{
  return wgt_fast(u);
}
