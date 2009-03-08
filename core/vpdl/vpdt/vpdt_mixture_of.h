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

#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_memory.h>

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
  //: the data type used for vectors
  typedef typename dist_t::vector vector;
  //: define the fixed dimension (normally specified by template parameter n)
  static const unsigned int n = vpdt_field_traits<vector>::dimension;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<vector>::scalar_type T;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<vector>::matrix_type matrix;
  //: define the component type
  typedef dist_t component_type;

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
  vcl_vector<component*> components_;

public:
  // Default Constructor
  vpdt_mixture_of() {}

  // Copy Constructor
  vpdt_mixture_of(const vpdt_mixture_of<dist_t>& other)
    : components_(other.components_.size(),NULL)
  {
    // deep copy of the data
    for (unsigned int i=0; i<components_.size(); ++i){
      components_[i] = new component(*other.components_[i]);
    }
  }

  // Destructor
  ~vpdt_mixture_of()
  {
    for (unsigned int i=0; i<components_.size(); ++i){
      delete components_[i];
    }
  }

  //: Assignment operator
  vpdt_mixture_of<dist_t>& operator= (const vpdt_mixture_of<dist_t>& rhs)
  {
    if (this != &rhs){
      for (unsigned int i=0; i<components_.size(); ++i){
        delete components_[i];
      }
      components_.clear();
      for (unsigned int i=0; i<rhs.components_.size(); ++i){
        components_.push_back(new component(*rhs.components_[i]));
      }
      this->set_dimension(rhs.dimension());
    }
    return *this;
  }
  
  //: Return the run time dimension, which does not equal \c n when \c n==0
  unsigned int dimension() const 
  { 
    if(n > 0 || num_components() == 0)
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
  bool insert(const dist_t& d, const T& weight = T(0))
  { 
    assert(d.dimension() == this->dimension() || num_components() == 0);
    components_.push_back(new component(d, weight)); 
    return true;
  }

  //: Remove the last component in the vector
  bool remove_last() 
  { 
    if(components_.empty())
      return false;
    delete components_.back(); 
    components_.pop_back();
    return true;
  }
  
  //: Compute the unnormalized density at this point
  T density(const vector& pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
      // must use prob_density here to get meaningful results
      prob += (*i)->weight * (*i)->distribution.prob_density(pt);
    }
    return prob;
  }

  //: Compute the probability density at this point
  T prob_density(const vector& pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
      prob += (*i)->weight * (*i)->distribution.prob_density(pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }

  //: The probability integrated over a box
  T box_prob(const vector& min_pt, const vector& max_pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
      prob += (*i)->weight * (*i)->distribution.box_prob(min_pt,max_pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }
  
  //: Evaluate the cumulative distribution function at a point
  // This is the integral of the density function from negative infinity
  // (in all dimensions) to the point in question
  T cumulative_prob(const vector& pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    T sum_w = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
      prob += (*i)->weight * (*i)->distribution.cumulative_prob(pt);
      sum_w += (*i)->weight;
    }
    assert(sum_w > T(0));
    return prob/sum_w;
  }
  
  //: Compute the mean of the distribution.
  // weighted average of the component means
  void compute_mean(vector& mean) const
  {
    const unsigned int d = this->dimension();
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));
    
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    vector cmp_mean;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
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
    vector mean;
    vpdt_set_size(covar,d);
    vpdt_fill(covar,T(0));
    vpdt_set_size(mean,d);
    vpdt_fill(mean,T(0));
    
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    vector cmp_mean;
    matrix cmp_covar;
    T sum_w = T(0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i){
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
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T sum = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      sum += (*i)->weight;
    assert(sum > 0);
    return 1/sum;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    typedef typename vcl_vector<component*>::iterator comp_itr;
    T norm = norm_const();
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      (*i)->weight *= norm;
  }

  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_.begin(), components_.end(), sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // The prototype should be
  // \code
  // template <class dist_t>
  // bool functor(const dist_t& d1, const vpdt_dist_traits<dist_t>::scalar_type& w1,
  //              const dist_t& d2, const vpdt_dist_traits<dist_t>::scalar_type& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp)
  { vcl_sort(components_.begin(), components_.end(), sort_adaptor<comp_type_>(comp)); }

  //: Sort the top \c idx components using any StrictWeakOrdering function
  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx)
  { vcl_sort(components_.begin(), components_.begin()+idx+1, sort_adaptor<comp_type_>(comp)); }

};


#endif // vpdt_mixture_of_h_
