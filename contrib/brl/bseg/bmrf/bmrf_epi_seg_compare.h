// This is brl/bseg/bmrf/bmrf_epi_seg_compare.h
#ifndef bmrf_epi_seg_compare_h_
#define bmrf_epi_seg_compare_h_
//---------------------------------------------------------------------
//:
// \file
// \brief Functions for comparing epi-segments
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 7/28/04
//
//
// \verbatim
//  Modifications
// \endverbatim
//---------------------------------------------------------------------


#include <vcl_algorithm.h>
#include "bmrf_epi_seg_sptr.h"
#include "bmrf_epi_seg.h"

//: Return the minimum alpha value in both epi-segments 
inline double bmrf_min_alpha( const bmrf_epi_seg_sptr& ep1, 
                              const bmrf_epi_seg_sptr& ep2 )
{
  return vcl_max(ep1->min_alpha(), ep2->min_alpha());
}
  
//: Return the maximum alpha value in both epi-segments
inline double bmrf_max_alpha( const bmrf_epi_seg_sptr& ep1, 
                              const bmrf_epi_seg_sptr& ep2 )
{
  return vcl_min(ep1->max_alpha(), ep2->max_alpha());
}
  
//: Return the minimum alpha step size needed for both epi-segments
inline double bmrf_d_alpha( const bmrf_epi_seg_sptr& ep1, 
                            const bmrf_epi_seg_sptr& ep2 )
{
  return vcl_min((ep1->max_alpha() - ep1->min_alpha())/ep1->n_pts() ,
                 (ep2->max_alpha() - ep2->min_alpha())/ep2->n_pts() );
}

//: Compute the average distance ratio in the overlapping alpha
double bmrf_avg_distance_ratio( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2);

//: Compute the average match error between to segments
double bmrf_match_error( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2 );

//: Compute the average intensity error between to segments
double bmrf_intensity_error( const bmrf_epi_seg_sptr& ep1, const bmrf_epi_seg_sptr& ep2 );

#endif // bmrf_epi_seg_compare_h_
