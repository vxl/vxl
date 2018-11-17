#ifndef rgrl_initializer_inv_indexing_h_
#define rgrl_initializer_inv_indexing_h_
//:
// \file
// \brief Generate a set of initial estimates using invariant indexing
// \author Charlene Tsai
// \date March 2004

#include <iostream>
#include <vector>
#include "rgrl_initializer.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  rgrl_initializer_inv_indexing( const rgrl_view_sptr& prior_view,
                                 bool should_estimate_global_region = true,
                                 int max_num_matches_tried = -1);

  rgrl_initializer_inv_indexing( rgrl_mask_sptr const&     from_image_roi,
                                 rgrl_mask_sptr const&     to_image_roi,
                                 rgrl_estimator_sptr xform_estimator = nullptr,
                                 unsigned             initial_resolution = 0,
                                 bool should_estimate_global_region = true,
                                 int max_num_matches_tried = -1);

  //: Destructor
  ~rgrl_initializer_inv_indexing() override = default;

  //: Add the set of invariants from the fixed image and the set from one moving image
  //
  //  Matches are sorted in order of decreasing distance. \a
  //  nn_radius has precedence over \a k_nn. If no neighbor falls in
  //  \a nn_radius, \a k_nn are taken.
  //
  //  \param nn_radius  nearest-neighbors in nn_radius.
  //  \param k   k-nearest neighbor(s)
  void add_data( std::vector<rgrl_invariant_sptr> const& fixed_set,
                 std::vector<rgrl_invariant_sptr> const& moving_set,
                 double nn_radius = 0,
                 unsigned int k_nn = 1 );

  //: Add a set of invariants from the fixed image and multiple sets from multiple moving images
  //
  //  Matches are sorted in order of decreasing distance. \a
  //  nn_radius has precedence over \a k_nn. If no neighbor falls in
  //  \a nn_radius, \a k_nn are taken.
  //
  //  \param nn_radius nearest-neighbors in nn_radius.
  //  \param k_nn k-nearest-neighbor(s)
  virtual void add_multiple_data( std::vector<rgrl_invariant_sptr> const& fixed_set,
                                  std::vector<std::vector<rgrl_invariant_sptr> > const& moving_sets,
                                  double nn_radius = 0,
                                  unsigned int k_nn = 1);

  //: Set the index of the current moving image. Default is the 0th moving-image
  void set_current_moving_image( unsigned int moving_image_index);

  //: Return the matches for the given \a moving_image_index.
  const std::vector<rgrl_invariant_match_sptr>& matches_for_moving_image( unsigned int moving_image_index);

  //: Get and remove the next initial estimate from the end of the list for the current moving_set
  bool next_initial( rgrl_view_sptr  & view,
                     rgrl_scale_sptr & prior_scale) override;

  //: Get and remove the next initial estimate from the end of the list for the current moving_set
  //  Return this match in best_match
  bool next_initial( rgrl_invariant_match_sptr& best_match );

  //: return number of initializations
  //  -1 stands for unknown
  int size() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_inv_indexing, rgrl_initializer );

 protected:
  std::vector<std::vector<rgrl_invariant_match_sptr> > matches_;
  rgrl_view_sptr view_;
  bool should_estimate_global_region_;
  unsigned int current_moving_image_ind_;
  int num_matches_tried_;
  int max_num_matches_tried_;
};

#endif
