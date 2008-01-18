// This is brcv/seg/bsta/bsta_mixture_fixed.h
#ifndef bsta_mixture_fixed_h_
#define bsta_mixture_fixed_h_

//:
// \file
// \brief A mixture of distributions of fixed size
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/17/08
//
// \verbatim
//  Modifications
// \endverbatim

#include "bsta_distribution.h"
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_vector_fixed.h>

//: A mixture of distributions with a fixed size of s components
template <class _dist, unsigned s>
class bsta_mixture_fixed : public bsta_distribution<typename _dist::math_type,
                                                             _dist::dimension>
{
  public:
    typedef _dist dist_type;
    enum { max_components = s };
  
  private:
    typedef typename _dist::math_type T;

  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
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
    bool operator() (const component& c1, const component& c2) const
      { return comp(c1.distribution, c1.weight, c2.distribution, c2.weight); }
    _comp_type comp;
  };

  //: The fixed size array of components
  component components_[s];
  //: The number of active components
  unsigned num_components_;

 public:
  //: Constructor
  bsta_mixture_fixed<_dist,s>() : num_components_(0) {}

  //: Copy Constructor
  bsta_mixture_fixed<_dist,s>(const bsta_mixture_fixed<_dist,s>& other)
  : num_components_(other.num_components_)
  {
    // deep copy of the data
    for(unsigned int i=0; i<s; ++i){
      components_[i] = other.components_[i];
    }
  }

  //: Destructor
  ~bsta_mixture_fixed<_dist,s>()
  {
  }

  //: Assignment operator
  bsta_mixture_fixed<_dist,s>& operator= (const bsta_mixture_fixed<_dist,s>& rhs)
  {
    if(this != &rhs){
       // deep copy of the data
       for(unsigned int i=0; i<s; ++i){
          components_[i] = rhs.components_[i];
       }
       num_components_ = rhs.num_components_;
    }
    return *this;
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return num_components_; }

  //: Access (const) a component distribution of the mixture
  const _dist& distribution(unsigned int index) const
  { return components_[index].distribution; }

  //: Access a component distribution of the mixture
  _dist& distribution(unsigned int index)
  { return components_[index].distribution; }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return components_[index].weight; }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w) { components_[index].weight = w; }
  
  //: Insert a new component in the next location in the array
  bool insert(const _dist& d, const T& weight = T(0))
  { 
    if(num_components_ >= s)
      return false;
    
    components_[num_components_++] = component(d, weight); 
    return true;
  }
  
  //: Remove the last component in the vector
  void remove_last() { components_[--num_components_].weight = T(0); }

  //: Compute the probablity of this point
  // \note assumes weights have been normalized
  T probability(const vnl_vector_fixed<T,_dist::dimension>& pt) const
  {
    T prob = 0;
     
    for(unsigned i=0; i<num_components_; ++i)
      prob += components_[i].weight 
            * components_[i].distribution.probability(pt);
    return prob;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    T sum = 0;
    for(unsigned i=0; i<num_components_; ++i)
      sum += components_[i].weight;
    assert(sum > 0);
    for(unsigned i=0; i<num_components_; ++i)
      components_[i].weight /= sum;
  }

  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_, components_+num_components_, sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // the prototype should be
  // \code
  // template <class T>
  // bool functor(const bsta_distribution<T>& d1, const T& w1,
  //              const bsta_distribution<T>& d2, const T& w2);
  // \endcode
  template <class _comp_type>
  void sort(_comp_type comp) 
  { vcl_sort(components_, components_+num_components_, sort_adaptor<_comp_type>(comp)); }

  //: Sort the first components up to index idx 
  template <class _comp_type>
  void sort(_comp_type comp, unsigned int idx)
  { assert(idx < s);
    vcl_sort(components_, components_+idx+1, sort_adaptor<_comp_type>(comp)); }

};


#endif // bsta_mixture_fixed_h_
