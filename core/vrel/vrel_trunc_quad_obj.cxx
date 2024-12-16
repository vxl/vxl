// This is core/vrel/vrel_trunc_quad_obj.cxx
#include "vrel_trunc_quad_obj.h"

vrel_trunc_quad_obj::vrel_trunc_quad_obj(double T)
  : T_sqr_(T * T)
{}


vrel_trunc_quad_obj::~vrel_trunc_quad_obj() = default;


double
vrel_trunc_quad_obj::rho(double u) const
{
  return rho_fast(u);
}

double
vrel_trunc_quad_obj::wgt(double u) const
{
  return wgt_fast(u);
}
