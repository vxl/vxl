#ifndef boxm2_surface_distance_refine_h_
#define boxm2_surface_distance_refine_h_
// :
// \file
// \brief  Refine a scene based on a surface distance functor
//
// \author J.L. Mundy
// \date   18 November 2015
//
#include <boxm2/boxm2_scene.h>
#include <vcl_cmath.h>
#include <vcl_string.h>

// template over a functor that defines surface distance
// the vector types contains the prefix strings for each database in databases
template <class D>
void boxm2_surface_distance_refine(D const& dist_functor, boxm2_scene_sptr scene,
                                   vcl_vector<vcl_string> const& prefixes, double distance_thresh = vcl_sqrt(3.0),
                                   bool zero_model = true, float occupied_prob = 0.99f);

#endif// boxm2_surface_distance_refine
