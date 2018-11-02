// This is core/vpdl/vpdl_mixture.h
#ifndef vpdl_mixture_h_
#define vpdl_mixture_h_
//:
// \file
// \author Matthew Leotta
// \date February 18, 2009
// \brief A mixture of distributions
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
#include <vpdl/vpdl_multi_cmp_dist.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <cassert>

//: A mixture of distributions
// A mixture is a weighted linear combination of other mixtures.
// This class is the most general, polymorphic version of a mixture.
// Internally it keeps base class pointers to clones of the supplied distributions.
// Each distribution in the mixture could potentially be of a different type.
template<class T, unsigned int n=0>
class vpdl_mixture : public vpdl_multi_cmp_dist<T,n>
{
 public:
  //: the data type used for vectors
  typedef typename vpdt_field_default<T,n>::type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;

 private:
  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
  // Dynamic memory is used to allow for polymorphic distributions.
  // However, this use of memory is self-contained and private so the user
  // should not be able to introduce a memory leak
  struct component
  {
    //: Constructor
    component() : distribution(), weight(T(0)) {}
    //: Constructor
    component(const vpdl_distribution<T,n>& d, const T& w = T(0) )
      : distribution(d.clone()), weight(w) {}
    //: Copy Constructor
    component(const component& other)
      : distribution(other.distribution->clone()), weight(other.weight) {}

    //: Used to sort by decreasing weight
    bool operator< (const component& rhs) const
    { return this->weight > rhs.weight; }

    // ============ Data =============

    //: The distribution
    std::unique_ptr<vpdl_distribution<T,n> > distribution;
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
    { return comp(*c1->distribution, c1->weight, *c2->distribution, c2->weight); }
    comp_type_ comp;
  };

  //: The vector of components
  std::vector<component*> components_;

 public:

  //: Default Constructor
  vpdl_mixture() {}

  // Copy Constructor
  vpdl_mixture(vpdl_mixture<T,n> const& other)
    : vpdl_multi_cmp_dist<T,n>(other), components_(other.components_.size(),nullptr)
  {
    // deep copy of the data
    for (unsigned int i=0; i<components_.size(); ++i) {
      components_[i] = new component(*other.components_[i]);
    }
  }

  // Destructor
  ~vpdl_mixture()
  {
    for (unsigned int i=0; i<components_.size(); ++i) {
      delete components_[i];
    }
  }

  //: Assignment operator
  vpdl_mixture<T,n>& operator= (vpdl_mixture<T,n> const& rhs)
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

  //: Create a copy on the heap and return base class pointer
  virtual vpdl_distribution<T,n>* clone() const
  {
    return new vpdl_mixture<T,n>(*this);
  }

  //: Return the run time dimension, which does not equal \c n when \c n==0
  virtual unsigned int dimension() const
  {
    if (n > 0 || num_components() == 0)
      return n;
    return components_[0]->distribution->dimension();
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return components_.size(); }

  //: Access (const) a component distribution of the mixture
  const vpdl_distribution<T,n>& distribution(unsigned int index) const
  {
    assert(index < num_components());
    return *(components_[index]->distribution);
  }

  //: Access a component distribution of the mixture
  vpdl_distribution<T,n>& distribution(unsigned int index)
  {
    assert(index < num_components());
    return *(components_[index]->distribution);
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
  bool insert(const vpdl_distribution<T,n>& d, const T& wght = T(0))
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

  //: Evaluate the unnormalized density at a point
  virtual T density(const vector& pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T dens = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      // must use prob_density here to get meaningful results
      dens += (*i)->weight * (*i)->distribution->prob_density(pt);
    }
    return dens;
  }

  //: Compute the probability density at this point
  T prob_density(const vector& pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      prob += (*i)->weight * (*i)->distribution->prob_density(pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }

  //: Compute the gradient of the unnormalized density at a point
  // \return the density at \a pt since it is usually needed as well, and
  //         is often trivial to compute while computing gradient
  // \retval g the gradient vector
  virtual T gradient_density(const vector& pt, vector& g) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    const unsigned int d = this->dimension();
    vpdt_set_size(g,d);
    vpdt_fill(g,T(0));
    T dens = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      vector g_i;
      T w_i = (*i)->distribution->norm_const() * (*i)->weight;
      dens +=  w_i * (*i)->distribution->gradient_density(pt,g_i);
      g_i *= w_i;
      g += g_i;
    }
    return dens;
  }

  //: The probability integrated over a box
  T box_prob(const vector& min_pt, const vector& max_pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      prob += (*i)->weight * (*i)->distribution->box_prob(min_pt,max_pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }

  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  virtual T cumulative_prob(const vector& pt) const
  {
    typedef typename std::vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      prob += (*i)->weight * (*i)->distribution->cumulative_prob(pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }

  //: Compute the mean of the distribution.
  // weighted average of the component means
  virtual void compute_mean(vector& mean) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));

    typedef typename std::vector<component*>::const_iterator comp_itr;
    vector cmp_mean;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      (*i)->distribution->compute_mean(cmp_mean);
      cmp_mean *= (*i)->weight;
      sum_w += (*i)->weight;
      mean += cmp_mean;
    }
    assert(sum_w > 0);
    mean /= sum_w;
  }

  //: Compute the covariance of the distribution.
  virtual void compute_covar(matrix& covar) const
  {
    const unsigned int d = this->dimension();
    vector mean;
    vpdt_set_size(covar,d);
    vpdt_fill(covar,T(0));
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));

    typedef typename std::vector<component*>::const_iterator comp_itr;
    vector cmp_mean;
    matrix cmp_covar;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i) {
      const T& wgt = (*i)->weight;
      (*i)->distribution->compute_covar(cmp_covar);
      (*i)->distribution->compute_mean(cmp_mean);
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
  virtual T norm_const() const
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
  // template <class T>
  // bool functor(const vpdl_distribution<T,n>& d1, const T& w1,
  //              const vpdl_distribution<T,n>& d2, const T& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp)
  {
    std::sort(components_.begin(),
             components_.end(),
             sort_adaptor<comp_type_>(comp));
  }

  //: Sort the components in the range \a idx1 to \a idx2 using any StrictWeakOrdering function
  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx1, unsigned int idx2)
  {
    std::sort(components_.begin()+idx1,
             components_.begin()+idx2+1,
             sort_adaptor<comp_type_>(comp));
  }
};


#endif // vpdl_mixture_h_
