// This is core/vpdl/vpdl_mixture_of.h
#ifndef vpdl_mixture_of_h_
#define vpdl_mixture_of_h_
//:
// \file
// \author Matthew Leotta
// \date February 24, 2009
// \brief A mixture of a fixed type of distributions
//
// \verbatim
// Modifications
//   None
// \endverbatim

#include <vpdl/vpdl_multi_cmp_dist.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vpdl/vpdt/vpdt_mixture_of.h>
#include <vpdl/vpdt/vpdt_probability.h>

//: A mixture of a fixed type of distributions
// A mixture is a weighted linear combination of other mixtures.
// This class represents a mixture of a specific type of distribution.
// Each component in the mixture has its own weight and parameters,
// but each must be of the same type.
// \tparam dist_t is the type of a component distribution
// \sa vpdl_mixture
template<class dist_t>
class vpdl_mixture_of
 : public vpdl_multi_cmp_dist<typename vpdt_dist_traits<dist_t>::scalar_type,
          vpdt_dist_traits<dist_t>::dimension>
{
  vpdt_mixture_of<dist_t> impl_;

 public:
  //: the data type to represent a point in the field
  typedef typename dist_t::field_type field_type;
  //: define the component type
  typedef dist_t component_type;

  //: define the fixed dimension (normally specified by template parameter n)
  static const unsigned int n = vpdt_field_traits<field_type>::dimension;
  //: the data type to represent a point in the field
  typedef typename dist_t::field_type F;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<field_type>::scalar_type T;
  //: define the vector type
  typedef typename vpdt_field_traits<field_type>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<field_type>::matrix_type matrix;

  // Default Constructor
  vpdl_mixture_of() {}

  // Destructor
  virtual ~vpdl_mixture_of() {}

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_mixture_of<dist_t>(*this);
  }

  //: Return the run time dimension
  virtual unsigned int dimension() const { return impl_.dimension(); }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return impl_.num_components(); }

  //: Access (const) a component distribution of the mixture
  const dist_t& distribution(unsigned int index) const
  { return impl_.distribution(index); }

  //: Access a component distribution of the mixture
  dist_t& distribution(unsigned int index) { return impl_.distribution(index); }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return impl_.weight(index); }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w) { impl_.set_weight(index,w); }

  //: Insert a new component at the end of the vector
  bool insert(const dist_t& d, const T& wght = T(0))
  { return impl_.insert(d,wght); }

  //: Remove the last component in the vector
  bool remove_last() { return impl_.remove_last(); }

  //: Compute the unnormalized density at this point
  T density(const vector& pt) const { return impl_.density(pt); }

  //: Compute the probability density at this point
  T prob_density(const vector& pt) const { return vpdt_prob_density(impl_,pt); }

  //: Compute the gradient of the unnormalized density at a point
  // \return the density at \a pt since it is usually needed as well, and
  //         is often trivial to compute while computing gradient
  // \retval g the gradient vector
  virtual T gradient_density(const vector& pt, vector& g) const
  {
    return impl_.gradient_density(pt,g);
  }

  //: The probability integrated over a box
  T box_prob(const vector& min_pt, const vector& max_pt) const
  { return vpdt_box_prob(impl_,min_pt,max_pt); }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  { return impl_.cumulative_prob(pt); }

  //: Compute the mean of the distribution.
  // weighted average of the component means
  virtual void compute_mean(vector& mean) const { impl_.compute_mean(mean); }

  //: Compute the covariance of the distribution.
  virtual void compute_covar(matrix& covar) const { impl_.compute_covar(covar); }

  //: The normalization constant for the density
  // When density() is multiplied by this value it becomes prob_density
  // norm_const() is reciprocal of the integral of density over the entire field
  virtual T norm_const() const { return impl_.norm_const(); }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights() { impl_.normalize_weights(); }

  //: Sort the components in order of decreasing weight
  void sort() { impl_.sort(); }

  //: Sort the components in the range \a idx1 to \a idx2 in order of decreasing weight
  void sort(unsigned int idx1, unsigned int idx2) { impl_.sort(idx1, idx2); }

  //: Sort the components using any StrictWeakOrdering function
  // The prototype should be
  // \code
  // template <class dist_t>
  // bool functor(const dist_t& d1, const vpdt_dist_traits<dist_t>::scalar_type& w1,
  //              const dist_t& d2, const vpdt_dist_traits<dist_t>::scalar_type& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp) { impl_.sort(comp); }

  //: Sort the components in the range \a idx1 to \a idx2 using any StrictWeakOrdering function
  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx1, unsigned int idx2) { impl_.sort(comp,idx1,idx2); }
};


#endif // vpdl_mixture_of_h_
