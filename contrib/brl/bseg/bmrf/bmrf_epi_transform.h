// This is contrib/brl/bseg/bmrf/bmrf_epi_transform.h
#ifndef bmrf_epi_transform_h_
#define bmrf_epi_transform_h_
//:
// \file
// \brief Epipolar transformation functions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 02/24/04
//
// This file contains a set of functions for epipolar transformations
// Transformations are calculated as 
// $ s(\alpha,t) = \frac{s_{0}(\alpha)}{1-\gamma(\alpha,t)*t} $,
// where (s0,alpha) is an initial point in epipolar space, (s,alpha) is the
// transformed point, t is time, and gamma is a function of alpha and time
// that defines the transformation.  See bmrf_gamma_func.h  
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <bmrf/bmrf_gamma_func.h>
#include <bmrf/bmrf_gamma_func_sptr.h>
#include <bmrf/bmrf_epi_point_sptr.h>
#include <bmrf/bmrf_epi_seg_sptr.h>


//: Calculates the transformed value of \i s
inline double 
bmrf_epi_transform( double s0, 
                    double alpha, 
                    const bmrf_gamma_func_sptr& gamma, 
                    double t = 1.0 )
{ 
  return s0/(1.0 - (*gamma)(alpha,t) * t); 
}


//: Calculates a transformed epi_point
// \param ep is the epipoint to trasform
// \param t is the time step
// \param update_all is a flag indicating whether or not to update all
//        members of the bmrf_epi_point.  By default, only the \i s value is updated
bmrf_epi_point_sptr
bmrf_epi_transform( const bmrf_epi_point_sptr& ep, 
                    const bmrf_gamma_func_sptr& gamma, 
                    double t = 1.0, 
                    bool update_all = false );


//: Calculates a transformed epi_seg
// \param ep is the epipoint to trasform
// \param t is the time step
// \param update_all is a flag indicating whether or not to update all
//        members of the bmrf_epi_seg.  By default, only the \i s values
//        of each epi_point are updated
bmrf_epi_seg_sptr
bmrf_epi_transform( const bmrf_epi_seg_sptr& ep, 
                    const bmrf_gamma_func_sptr& gamma, 
                    double t = 1.0, 
                    bool update_all = false );
  


#endif // bmrf_epi_transform_h_
