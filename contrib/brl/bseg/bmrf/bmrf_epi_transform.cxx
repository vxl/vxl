// This is contrib/brl/bseg/bmrf/bmrf_epi_transform.cxx
//:
// \file

#include "bmrf_epi_transform.h"
#include <bmrf/bmrf_epi_point.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_seg.h>
#include <bmrf/bmrf_epi_seg_sptr.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>


struct bmrf_tranformed_epi_seg : public bmrf_epi_seg 
{
  bmrf_tranformed_epi_seg( const bmrf_gamma_func_sptr& gamma,
                           const bmrf_epi_seg_sptr& ep,
                           double t, bool update_all )
    : bmrf_epi_seg(*ep)
  {
    // Do not keep the ref_count of the object we are copying
    // ref_count must be reset to 0 or there will be a memory leak
    this->ref_count = 0;
    for( vcl_vector<bmrf_epi_point_sptr>::iterator s_itr = this->seg_.begin();
         s_itr != seg_.end();  ++s_itr ){
      *s_itr = bmrf_epi_transform(*s_itr, gamma, t, update_all);
    }
    this->limits_valid_ = false;
  }
};


struct bmrf_tranformed_epi_point : public bmrf_epi_point
{
  bmrf_tranformed_epi_point( const bmrf_epi_point_sptr& ep )
    : bmrf_epi_point( *ep )
  {
    this->ref_count = 0;
  }
};


//: Calculates a transformed epi_point
bmrf_epi_point_sptr
bmrf_epi_transform( const bmrf_epi_point_sptr& ep, 
                    const bmrf_gamma_func_sptr& gamma, 
                    double t, 
                    bool update_all )
{
  bmrf_epi_point_sptr new_ep(new bmrf_tranformed_epi_point(ep));
  new_ep->set_s( bmrf_epi_transform(ep->s(), ep->alpha(), gamma, t));
  if( update_all ){
    // update x, y, etc. here
    double ds = new_ep->s() - ep->s();
    new_ep->set( ep->x()+ds*vcl_cos(ep->alpha()), ep->y()+ds*vcl_sin(ep->alpha()) );
  }
  return new_ep;
}


//: Calculates a transformed epi_seg
bmrf_epi_seg_sptr
bmrf_epi_transform( const bmrf_epi_seg_sptr& ep, 
                    const bmrf_gamma_func_sptr& gamma, 
                    double t, 
                    bool update_all )
{
  bmrf_epi_seg_sptr new_seg(new bmrf_tranformed_epi_seg(gamma, ep, t, update_all));
  return new_seg;
}

