#ifndef rgrl_convergence_tester_h_
#define rgrl_convergence_tester_h_
//:
// \file
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
 
  //: ctor
  rgrl_convergence_tester() : rel_tol_thres_(1e-3) { }
  
  virtual ~rgrl_convergence_tester(); 
  
  //:  Compute the converge_status of the current view using multiple match sets
  //   This is the pure virtual function implemented in the derived classes.
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
  //   associated weights computed already when passed to this function.
  rgrl_converge_status_sptr
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_view                        const& prev_view,
                  rgrl_view                        const& current_view,
                  rgrl_match_set_sptr                     current_match_set,
                  rgrl_scale_sptr                         current_scale,
                  bool                                    penalize_scaling = false)const;

  //:  Compute the converge_status of the current xform estimate using multiple match sets
  rgrl_converge_status_sptr
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_transformation_sptr                xform_estimate,
                  rgrl_estimator_sptr                     xform_estimator,
                  rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                  rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                  bool                                    penalize_scaling = false)const;
  //:  Compute the converge_status of the current xform estimate using single match set
  rgrl_converge_status_sptr
  compute_status( rgrl_converge_status_sptr               prev_status,
                  rgrl_transformation_sptr                xform_estimate,
                  rgrl_estimator_sptr                     xform_estimator,
                  rgrl_match_set_sptr                     current_match_set,
                  rgrl_scale_sptr                         current_scale,
                  bool                                    penalize_scaling = false)const;

  //: convinient function for initializing status
  rgrl_converge_status_sptr
  initialize_status( rgrl_view       const& init_view,
                     rgrl_scale_sptr const& prior_scale,
                     bool                   penalize_scaling ) const;

  //: convinient function for initializing status
  rgrl_converge_status_sptr
  initialize_status( rgrl_transformation_sptr                xform_estimate,
                     rgrl_estimator_sptr                     xform_estimator,
                     rgrl_scale_sptr                  const& prior_scale,
                     bool                                    penalize_scaling )const;


  //: verify the final alignment
  virtual
  rgrl_converge_status_sptr 
  verify( rgrl_transformation_sptr         const& xform_estimate,
          rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
          rgrl_set_of<rgrl_scale_sptr>     const& current_scales )const;

  rgrl_converge_status_sptr 
  verify( rgrl_transformation_sptr    const& xform_estimate,
          rgrl_match_set_sptr         const& current_match_sets,
          rgrl_scale_sptr             const& current_scales )const;

  //: set relative tolerance on convergence
  void set_rel_tol( double rel_tol ) 
  { rel_tol_thres_ = rel_tol; }
  
  double rel_tol() const 
  { return rel_tol_thres_; }
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_convergence_tester, rgrl_object );

protected:
  
  //: the real init status function
  virtual rgrl_converge_status_sptr
  init_status( rgrl_view       const& init_view,
               rgrl_scale_sptr const& prior_scale,
               bool                   penalize_scaling ) const;
  
  //:  helper function for computing status
  virtual 
  rgrl_converge_status_sptr
  compute_status_helper(double new_error,
                        bool   good_enough,
                        rgrl_converge_status_sptr               prev_status,
                        rgrl_view                        const& prev_view,
                        rgrl_view                        const& current_view ) const;

protected:
  double  rel_tol_thres_;
};

#endif
