#ifndef rgrl_convergence_tester_h_
#define rgrl_convergence_tester_h_

#include "rgrl_set_of.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_converge_status_sptr.h"
#include "rgrl_transformation_sptr.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_object.h"

class rgrl_view;
class rgrl_transformation;

class rgrl_convergence_tester
  : public rgrl_object
{
public:
  //:  Compute the converge_status of the current view using multiple match sets
  //   This is the virtual function implemented in the derived classes
  //
  virtual rgrl_converge_status_sptr 
  compute_status( rgrl_converge_status_sptr               prev_status,
		  rgrl_view                        const& prev_view,
		  rgrl_view                        const& current_view,
		  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
		  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false )const  = 0;

  //:  Compute the converge_status of the current view using single match set
  //
  //   Matches are assumed to have from_feature mapped and stored and
  //   associated weights computeded already when passed to this
  //   function.
  virtual rgrl_converge_status_sptr 
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_view                        const& prev_view,
                  rgrl_view                        const& current_view,
                  rgrl_match_set_sptr                     current_match_set,
                  rgrl_scale_sptr                         current_scale,
                  bool                                    penalize_scaling = false)const;

  //:  Compute the converge_status of the current xform estimate using multiple match sets
  virtual rgrl_converge_status_sptr 
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_transformation_sptr                xform_estimate,
                  rgrl_estimator_sptr                     xform_estimator,
                  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false)const;
  //:  Compute the converge_status of the current xform estimate using single match set
  virtual rgrl_converge_status_sptr 
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_transformation_sptr                xform_estimate,
                  rgrl_estimator_sptr                     xform_estimator,
                  rgrl_match_set_sptr                     current_match_set,
                  rgrl_scale_sptr                         current_scale,
                  bool                                    penalize_scaling = false)const; 

  // Defines type-related functions
  rgrl_type_macro( rgrl_convergence_tester, rgrl_object );

};

#endif
