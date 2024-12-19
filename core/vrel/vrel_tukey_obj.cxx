// This is core/vrel/vrel_tukey_obj.cxx
#include "vrel_tukey_obj.h"


vrel_tukey_obj::vrel_tukey_obj(double B)
  : B_(B)
{}


vrel_tukey_obj::~vrel_tukey_obj() = default;


double
vrel_tukey_obj::rho(double u) const
{
  return rho_fast(u);
}

double
vrel_tukey_obj::wgt(double u) const
{
  return wgt_fast(u);
}
