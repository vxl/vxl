// This is brl/bbas/bsta/algo/bsta_von_mises_updater.h
#ifndef bsta_von_mises_updater_h_
#define bsta_von_mises_updater_h_
//:
// \file
// \brief Von_Mises updaters
// \author Joseph L. Mundy
// \date July 18, 2009
//
// This file contains updaters based on
// Von_Mises distributions
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)
//

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <bsta/bsta_von_mises.h>
#include <bsta/bsta_attributes.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A von_mises window updater based on samples of the space of unit vectors.
// Implemented only for vector dimensions 2 and 3.
template <class von_mises_dist_>
class bsta_von_mises_updater
{
 public:
  typedef typename von_mises_dist_::math_type math_t;
  typedef typename von_mises_dist_::vector_type vect_t;
  typedef bsta_vsum_num_obs<von_mises_dist_> obs_vm_dist_;
  //: for compatibility with vpdl/vpdt
  typedef obs_vm_dist_ distribution_type;
  enum { data_dimension = von_mises_dist_::data_dimension };
  //: for compatibility with vpdl/vpdt
  typedef typename von_mises_dist_::field_type field_type;

  // Constructor
  // initial_kappa_ is assigned when only one sample has been observed
  bsta_von_mises_updater(math_t kappa= math_t(10000)):
    initial_kappa_(kappa){}

  //: The update functor assuming standard learning rate
  void operator ()(obs_vm_dist_& pdist, const vect_t& sample) const{
    pdist.num_observations += math_t(1);
    pdist.vector_sum += sample;
    this->update(pdist, pdist.vector_sum, math_t(1)/pdist.num_observations);
  }
  //: The update functor with a learning rate specified
  void operator ()(obs_vm_dist_& pdist, const vect_t& sample, math_t alpha) const{
    pdist.num_observations += math_t(1);
    pdist.vector_sum += sample;
    this->update(pdist, pdist.vector_sum, alpha);
  }
 private:
  void update(obs_vm_dist_& pdist, const vect_t& vsum, math_t alpha) const;
  math_t initial_kappa_;
};


#endif // bsta_von_mises_updater_h_
