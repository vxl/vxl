#ifndef rgrl_initializer_inv_indexing_h_
#define rgrl_initializer_inv_indexing_h_
//:
// \file
// \brief Generate a set of initial estimates using invariant indexing
// \author Charlene Tsai
// \date March 2004

#include "rgrl_initializer.h"

#include <vcl_vector.h>

#include "rgrl_view_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_invariant_match_sptr.h"
#include "rgrl_invariant_sptr.h"

//: Generate the set of initial estimates using invariant indexing.
//
//  The underlying data structure is rrel_kd_tree for
//  k-dimensional invariant fields. The transformation model is
//  determined by the invariant class. Therefore, no estimator is
//  associated with the initializer.
//
class rgrl_initializer_inv_indexing
  : public rgrl_initializer
{
 public:
  //: Constructor
  rgrl_initializer_inv_indexing( rgrl_view_sptr prior_view,
                                 bool should_estimate_global_region = true,
                                 int max_num_matches_tried = -1);

  rgrl_initializer_inv_indexing( rgrl_mask_box  const&     from_image_roi,
                                 rgrl_mask_box  const&     to_image_roi,
                                 rgrl_estimator_sptr xform_estimator = 0,
                                 unsigned             initial_resolution = 0,
                                 bool should_estimate_global_region = true,
                                 int max_num_matches_tried = -1);

  //: Destructor
  ~rgrl_initializer_inv_indexing(){}

  //: Add the set of invariants from the fixed image and the set from one moving image
  //
  //  Matches are sorted in order of descreasing distance. \a
  //  nn_radius has precedence over \a k_nn. If no neighbor falles in
  //  \a nn_radius, \a k_nn are taken.
  //
  //  \param nn_radius  nearest-neighbors in nn_radius.
  //  \param k   k-nearest neighbor(s)
  void add_data( vcl_vector<rgrl_invariant_sptr> fixed_set,
                 vcl_vector<rgrl_invariant_sptr> moving_set,
                 double nn_radius,
                 unsigned int k_nn = 1 );

  //: Add a set of invariants from the fixed image and multiple sets from multiple moving images
  //
  //  Matches are sorted in order of descreasing distance. \a
  //  nn_radius has precedence over \a k_nn. If no neighbor falles in
  //  \a nn_radius, \a k_nn are taken.
  //
  //  \param nn_radius nearest-neighbors in nn_radius.
  //  \param k_nn k-nearest-neighbor(s)
  void add_data( vcl_vector<rgrl_invariant_sptr> fixed_set,
                 vcl_vector<vcl_vector<rgrl_invariant_sptr> > moving_sets,
                 double nn_radius,
                 unsigned int k_nn  = 1);

  //: Set the index of the current moving image. Default is the the 0th moving-image
  void set_current_moving_image( unsigned int moving_image_index);

  //: Return the matches for the given \a moving_image_index.
  const vcl_vector<rgrl_invariant_match_sptr>& matches_for_moving_image( unsigned int moving_image_index);

  //: Get and remove the next initial estimate from the end of the list for the current moving_set
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale);

  //: Get and remove the next initial estimate from the end of the list for the current moving_set
  //  Return this match in best_match
  bool next_initial( rgrl_invariant_match_sptr& best_match );

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_inv_indexing, rgrl_initializer );

 private:
  vcl_vector<vcl_vector<rgrl_invariant_match_sptr> > matches_;
  rgrl_view_sptr view_;
  bool should_estimate_global_region_;
  unsigned int current_moving_image_ind_;
  int num_matches_tried_;
  int max_num_matches_tried_;
};

#endif
