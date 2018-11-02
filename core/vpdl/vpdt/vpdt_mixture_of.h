// This is core/vpdl/vpdt/vpdt_mixture_of.h
#ifndef vpdt_mixture_of_h_
#define vpdt_mixture_of_h_
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

#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vpdl/vpdt/vpdt_probability.h>
#include <cassert>

// forward declarations
template<class dist_t> class vpdt_mixture_of;

template <class dist>
typename vpdt_dist_traits<vpdt_mixture_of<dist> >::scalar_type
vpdt_box_prob(const vpdt_mixture_of<dist>& d,
              const typename vpdt_dist_traits<vpdt_mixture_of<dist> >::field_type& min_pt,
              const typename vpdt_dist_traits<vpdt_mixture_of<dist> >::field_type& max_pt);


template <class dist>
struct vpdt_is_mixture<vpdt_mixture_of<dist> >
{
  static const bool value = true;
};


//: A mixture of a fixed type of distributions
// A mixture is a weighted linear combination of other mixtures.
// This class represents a mixture of a specific type of distribution.
// Each component in the mixture has its own weight and parameters,
// but each must be of the same type.
// \tparam dist_t is the type of a component distribution
// \sa vpdl_mixture
template<class dist_t>
class vpdt_mixture_of
{
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

 private:
  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
  struct component
  {
    //: Constructor
    component() : distribution(), weight(T(0)) {}
    //: Constructor
    component(const component_type& d, const T& w = T(0) )
      : distribution(d), weight(w) {}

    //: Used to sort by decreasing weight
    bool operator< (const component& rhs) const
    { return this->weight > rhs.weight; }

    // ============ Data =============

    //: The distribution
    component_type distribution;
    //: The weight
    T weight;
  };

  //: This functor is used by default for sorting with STL
  // The default sorting is decreasing by weight
  class sort_weight
  {
   public:
    bool operator() (const component* c1, const component* c2) const
      { return c1->weight > c2->weight; }
  };

  //: This adaptor allows users to define ordering functors on the components without accessing the components directly
  template <class comp_type_>
  class sort_adaptor
  {
   public:
    sort_adaptor(comp_type_ c) : comp(c) {}
    bool operator() (const component* const c1, const component* const c2) const
    { return comp(c1->distribution, c1->weight, c2->distribution, c2->weight); }
    comp_type_ comp;
  };

  //: The vector of components
  std::vector<component*> components_;

 public:
  // Default Constructor
  vpdt_mixture_of() {}

  // Copy Constructor
  vpdt_mixture_of(const vpdt_mixture_of<dist_t>& other)
    : components_(other.components_.size(),nullptr)
  {
    // deep copy of the data
    for (unsigned int i=0; i<components_.size(); ++i) {
      components_[i] = new component(*other.components_[i]);
    }
  }

  // Destructor
  ~vpdt_mixture_of()
  {
    for (unsigned int i=0; i<components_.size(); ++i) {
      delete components_[i];
    }
  }

  //: Assignment operator
  vpdt_mixture_of<dist_t>& operator= (const vpdt_mixture_of<dist_t>& rhs)
  {
    if (this != &rhs) {
      for (unsigned int i=0; i<components_.size(); ++i) {
        delete components_[i];
      }
      components_.clear();
      for (unsigned int i=0; i<rhs.components_.size(); ++i) {
        components_.push_back(new component(*rhs.components_[i]));
      }
    }
    return *this;
  }

  //: Return the run time dimension, which does not equal \c n when \c n==0
  unsigned int dimension() const
  {
    if (n > 0 || num_components() == 0)
      return n;
    return components_[0]->distribution.dimension();
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return components_.size(); }

  //: Access (const) a component distribution of the mixture
  const dist_t& distribution(unsigned int index) const
  {
    assert(index < num_components());
    return components_[index]->distribution;
  }

  //: Access a component distribution of the mixture
  dist_t& distribution(unsigned int index)
  {
    assert(index < num_components());
    return components_[index]->distribution;
  }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const
  {
    assert(index < num_components());
    return components_[index]->weight;
  }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w)
  {
    assert(index < num_components());
    assert(w >= T(0));
    components_[index]->weight = w;
  }

  //: Insert a new component at the end of the vector
  bool insert(const dist_t& d, const T& wght = T(0))
  {
    assert(d.dimension() == this->dimension() || num_components() == 0);
    components_.push_back(new component(d, wght));
    return true;
  }

  //: Remove the last component in the vector
  bool remove_last()
  {
    if (components_.empty())
      return false;
    delete components_.back();
    components_.pop_back();
    return true;
  }

  //: Compute the unnormalized density at this point
  T density(const F& pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T prob = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      // must use prob_density here to get meaningful results
      prob += (*i)->weight * (*i)->distribution.prob_density(pt);
    }
    return prob;
  }

  //: Compute the gradient of the unnormalized density at a point
  // \return the density at \a pt since it is usually needed as well, and
  //         is often trivial to compute while computing gradient
  // \retval g the gradient vector
  T gradient_density(const F& pt, vector& g) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(g,d);
    vpdt_fill(g,T(0));
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T dens = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      vector g_i;
      T w_i = (*i)->distribution.norm_const() * (*i)->weight;
      dens +=  w_i * (*i)->distribution.gradient_density(pt,g_i);
      g_i *= w_i;
      g += g_i;
    }
    return dens;
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  T cumulative_prob(const F& pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      prob += (*i)->weight * (*i)->distribution.cumulative_prob(pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }

  //: Compute the mean of the distribution.
  // weighted average of the component means
  void compute_mean(F& mean) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));

    typedef typename std::vector<component*>::const_iterator comp_itr;
    F cmp_mean;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      (*i)->distribution.compute_mean(cmp_mean);
      cmp_mean *= (*i)->weight;
      sum_w += (*i)->weight;
      mean += cmp_mean;
    }
    assert(sum_w > 0);
    mean /= sum_w;
  }

  //: Compute the covariance of the distribution.
  void compute_covar(matrix& covar) const
  {
    const unsigned int d = this->dimension();
    F mean;
    vpdt_set_size(covar,d);
    vpdt_fill(covar,T(0));
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));

    typedef typename std::vector<component*>::const_iterator comp_itr;
    F cmp_mean;
    matrix cmp_covar;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      const T& wgt = (*i)->weight;
      (*i)->distribution.compute_covar(cmp_covar);
      (*i)->distribution.compute_mean(cmp_mean);
      cmp_covar += outer_product(cmp_mean,cmp_mean);
      cmp_covar *= wgt;
      cmp_mean *= wgt;
      sum_w += wgt;
      covar += cmp_covar;
      mean += cmp_mean;
    }
    assert(sum_w > 0);
    covar -= outer_product(mean,mean);
    covar /= sum_w;
  }

  //: The normalization constant for the density
  // When density() is multiplied by this value it becomes prob_density
  // norm_const() is reciprocal of the integral of density over the entire field
  T norm_const() const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T sum = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      sum += (*i)->weight;
    assert(sum > 0);
    return 1/sum;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    typedef typename std::vector<component*>::iterator comp_itr;
    T norm = norm_const();
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      (*i)->weight *= norm;
  }

  //: Sort the components in order of decreasing weight
  void sort() { std::sort(components_.begin(), components_.end(), sort_weight() ); }

  //: Sort the components in the range \a idx1 to \a idx2 in order of decreasing weight
  void sort(unsigned int idx1, unsigned int idx2)
  { std::sort(components_.begin()+idx1, components_.begin()+idx2+1, sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // The prototype should be
  // \code
  // template <class dist_t>
  // bool functor(const dist_t& d1, const vpdt_dist_traits<dist_t>::scalar_type& w1,
  //              const dist_t& d2, const vpdt_dist_traits<dist_t>::scalar_type& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp)
  { std::sort(components_.begin(), components_.end(), sort_adaptor<comp_type_>(comp)); }

  //: Sort the components in the range \a idx1 to \a idx2 using any StrictWeakOrdering function
  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx1, unsigned int idx2)
  { std::sort(components_.begin()+idx1, components_.begin()+idx2+1, sort_adaptor<comp_type_>(comp)); }

  friend T vpdt_box_prob<dist_t>(const vpdt_mixture_of<dist_t>& d, const F& min_pt, const F& max_pt);
};


//: The probability of being in an axis-aligned box.
// The box is defined by two points, the minimum and maximum.
// Implemented in terms of \c vpdt_cumulative_prob() by default.
template <class dist>
typename vpdt_dist_traits<vpdt_mixture_of<dist> >::scalar_type
vpdt_box_prob(const vpdt_mixture_of<dist>& d,
              const typename vpdt_dist_traits<vpdt_mixture_of<dist> >::field_type& min_pt,
              const typename vpdt_dist_traits<vpdt_mixture_of<dist> >::field_type& max_pt)
{
  typedef typename vpdt_dist_traits<vpdt_mixture_of<dist> >::scalar_type T;
  typedef typename std::vector<typename vpdt_mixture_of<dist>::component*>::const_iterator comp_itr;
  T prob = 0;
  T sum_w = 0;
  for (comp_itr i = d.components_.begin(); i != d.components_.end(); ++i) {
    prob += (*i)->weight * vpdt_box_prob((*i)->distribution,min_pt,max_pt);
    sum_w += (*i)->weight;
  }
  assert(sum_w > T(0));
  return prob/sum_w;
}


#endif // vpdt_mixture_of_h_
