// This is prip/vmap/vmap_2_map.cxx
#include "vmap_2_map.h"

vmap_2_map_dart::~vmap_2_map_dart()
{
}

void vmap_2_map_dart::set_sigma(vmap_2_map_dart *arg)
{
  sigma_=arg ;
  arg->isigma_=this ;
}

void vmap_2_map_dart::set_alpha(vmap_2_map_dart *arg)
{
  alpha_=arg ;
  arg->alpha_=this ;
}

void vmap_2_map_dart::set_phi(vmap_2_map_dart *arg)
{
  alpha()->sigma_=arg ;
  arg->isigma_=alpha();
}
