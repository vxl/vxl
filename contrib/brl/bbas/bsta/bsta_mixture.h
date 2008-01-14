// This is brcv/seg/bsta/bsta_mixture.h
#ifndef bsta_mixture_h_
#define bsta_mixture_h_

//:
// \file
// \brief A mixture of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/26/06
//
// \verbatim
//  Modifications
// \endverbatim

#include "bsta_distribution.h"
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_vector_fixed.h>

//: A mixture of distributions
template <class _dist>
class bsta_mixture : public bsta_distribution<typename _dist::math_type,
                                                  _dist::dimension>
{
  private:
    typedef typename _dist::math_type T;

  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
  // Dynamic memory is used to allow for polymorphic distributions.
  // However, this use of memory is self-contained and private so the user
  // should not be able to introduce a memory leak
  struct component
  {
    //: Constructor
    component(): distribution(), weight(T(0)) {}
    //: Constructor
    component(const _dist& d, const T& w = T(0) )
      : distribution(d), weight(w) {}

    //: Used to sort by decreasing weight
    bool operator< (const component& rhs) const 
    { return this->weight > rhs.weight; }

    // ============ Data =============
    //: The distribution
    _dist distribution;
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

  //: This adaptor allows users to define ordering functors on the components
  //  without accessing the components directly
  template <class _comp_type>
  class sort_adaptor
  {
   public:
    sort_adaptor(_comp_type c) : comp(c) {}
    bool operator() (const component* const c1, const component* const c2) const
      { return comp(c1->distribution, c1->weight, c2->distribution, c2->weight); }
    _comp_type comp;
  };

  //: The vector of components
  vcl_vector<component*> components_;

 public:
  //: Constructor
  bsta_mixture<_dist>(){}

  //: Copy Constructor
  bsta_mixture<_dist>(const bsta_mixture<_dist>& other)
    : components_(other.components_.size(),NULL)
  {
    // deep copy of the data
    for(unsigned int i=0; i<components_.size(); ++i){
      components_[i] = new component(*other.components_[i]);
    }
  }

  //: Destructor
  ~bsta_mixture<_dist>()
  {
    for(unsigned int i=0; i<components_.size(); ++i){
      delete components_[i];
    }
  }

  //: Assignment operator
  bsta_mixture<_dist>& operator= (const bsta_mixture<_dist>& rhs)
  {
    if(this != &rhs){
      for(unsigned int i=0; i<components_.size(); ++i){
        delete components_[i];
      }
      components_.clear();
      for(unsigned int i=0; i<rhs.components_.size(); ++i){
        components_.push_back(new component(*rhs.components_[i]));
      }
    }
    return *this;
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return components_.size(); }

  //: Access (const) a component distribution of the mixture
  const _dist& distribution(unsigned int index) const
  { return components_[index]->distribution; }

  //: Access a component distribution of the mixture
  _dist& distribution(unsigned int index)
  { return components_[index]->distribution; }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return components_[index]->weight; }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w) { components_[index]->weight = w; }

  //: Insert a new component at the end of the vector
  void insert(const _dist& d, const T& weight = T(0))
  { components_.push_back(new component(d, weight)); }

  //: Remove the last component in the vector
  void remove_last() { delete components_.back(); components_.pop_back(); }

  //: Compute the probablity of this point
  // \note assumes weights have been normalized
  T probability(const vnl_vector_fixed<T,_dist::dimension>& pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    for(comp_itr i = components_.begin(); i != components_.end(); ++i)
      prob += (*i)->weight * (*i)->distribution.probability(pt);
    return prob;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    typedef typename vcl_vector<component*>::iterator comp_itr;
    T sum = 0;
    for(comp_itr i = components_.begin(); i != components_.end(); ++i)
      sum += (*i)->weight;
    assert(sum > 0);
    for(comp_itr i = components_.begin(); i != components_.end(); ++i)
      (*i)->weight /= sum;
  }

  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_.begin(), components_.end(), sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // the prototype should be
  // \code
  // template <class T>
  // bool functor(const bsta_distribution<T>& d1, const T& w1,
  //              const bsta_distribution<T>& d2, const T& w2);
  // \endcode
  template <class _comp_type>
  void sort(_comp_type comp) 
  { vcl_sort(components_.begin(), components_.end(), sort_adaptor<_comp_type>(comp)); }

  template <class _comp_type>
  void sort(_comp_type comp, unsigned int idx)
  { vcl_sort(components_.begin(), components_.begin()+idx+1, sort_adaptor<_comp_type>(comp)); }

};


#endif // bsta_mixture_h_
