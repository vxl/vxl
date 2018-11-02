// This is core/vpdl/vpdt/vpdt_probability.h
#ifndef vpdt_probability_h_
#define vpdt_probability_h_
//:
// \file
// \author Matthew Leotta
// \brief The basic functions for probability calculations
// \date March 13, 2009
//
// These functions provide default implementations for various probability
// calculation functions.  They are written in terms of distribution
// member functions
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim


#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute the probability from the density and normalization constant.
template <class dist>
inline typename vpdt_dist_traits<dist>::scalar_type
vpdt_prob_density(const dist& d,
                  const typename vpdt_dist_traits<dist>::field_type& pt)
{
  typedef typename vpdt_dist_traits<dist>::scalar_type T;
  T norm = d.norm_const();
  if (vnl_math::isinf(norm))
    return T(0);
  return norm * d.density(pt);
}


//: The probability of being in an axis-aligned box.
// The box is defined by two points, the minimum and maximum.
// Implemented in terms of \c vpdt_cumulative_prob() by default.
template <class dist>
typename vpdt_dist_traits<dist>::scalar_type
vpdt_box_prob(const dist& d,
              const typename vpdt_dist_traits<dist>::field_type& min_pt,
              const typename vpdt_dist_traits<dist>::field_type& max_pt)
{
  typedef typename vpdt_dist_traits<dist>::scalar_type T;
  typedef typename vpdt_dist_traits<dist>::field_type F;
  const unsigned int dim = d.dimension();

  // return zero for ill-defined box
  for (unsigned int j=0; j<dim; ++j) {
    if (vpdt_index(max_pt,j)<=vpdt_index(min_pt,j))
      return T(0);
  }

  // this method is not tractable for large dimensions
  assert(sizeof(unsigned long)*8 > dim);
  // compute the number of corners of the box (2^dim)
  const unsigned long num_corners = 1 << dim;

  T prob = T(0);
  F corner(min_pt);
  for (unsigned long i=0; i<num_corners; ++i)
  {
    // In odd dimensions, corners with an odd number of maximal axis
    // are added to the sum.  In even dimensions, corners with an even
    // number of maximal axis are added. The other corners are subtracted.
    bool plus = (dim%2 != 1);
    // create the corner position by selecting elements from max_pt and min_pt
    for (unsigned int j=0; j<dim; ++j) {
      bool is_max = (i>>j) & 1;
      plus ^= is_max; // toggle plus if is_max
      vpdt_index(corner,j) = is_max?vpdt_index(max_pt,j):vpdt_index(min_pt,j);
    }
    if (plus)
      prob += d.cumulative_prob(corner);
    else
      prob -= d.cumulative_prob(corner);
  }

  return prob;
}

#endif // vpdt_probability_h_
