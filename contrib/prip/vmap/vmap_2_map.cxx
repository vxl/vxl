// This is prip/vmap/vmap_2_map.cxx
#include "vmap_2_map.h"

vmap_2_map_dart::~vmap_2_map_dart()
{
}

void vmap_2_map_dart::set_sigma(vmap_2_map_dart *arg)
{
    _sigma=arg ;
    arg->_isigma=this ;
}

void vmap_2_map_dart::set_alpha(vmap_2_map_dart *arg)
{
    _alpha=arg ;
    arg->_alpha=this ;
}

void vmap_2_map_dart::set_phi(vmap_2_map_dart *arg)
{
    alpha()->_sigma=arg ;
    arg->_isigma=alpha();
}

