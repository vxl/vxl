// This is brl/bbas/bsta/bsta_mixture_fixed.h
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
//   Jan 21 2008  -  Matt Leotta  -  Rename probability to prob_density and
//                                   add probability integration over a box
// \endverbatim

#include "bsta_distribution.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h>


//: A mixture of distributions with a fixed size of s components
template <class dist_, unsigned s>
class bsta_mixture_fixed : public bsta_distribution<typename dist_::math_type,
                                                             dist_::dimension>
{
 public:
  typedef dist_ dist_type;
  enum { max_components = s };

 private:
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vector_;

  //: A struct to hold the component distributions and weights
  // This class is private and should not be used outside of the mixture.
  struct component
  {
    //: Constructor
    component(): distribution(), weight(T(0)) {}
    //: Constructor
    component(const dist_& d, const T& w = T(0) )
      : distribution(d), weight(w) {}

    //: Used to sort by decreasing weight
    bool operator< (const component& rhs) const
    { return this->weight > rhs.weight; }

    // ============ Data =============

    //: The distribution
    dist_ distribution;
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
    bool operator() (const component& c1, const component& c2) const
      { return comp(c1.distribution, c1.weight, c2.distribution, c2.weight); }
    comp_type_ comp;
  };

  //: The fixed size array of components
  component components_[s];
  //: The number of active components
  unsigned num_components_;

 public:
  // Default Constructor
  bsta_mixture_fixed<dist_,s>() : num_components_(0) {}

  // Copy Constructor
  bsta_mixture_fixed<dist_,s>(const bsta_mixture_fixed<dist_,s>& other)
  : num_components_(other.num_components_)
  {
    // deep copy of the data
    for (unsigned int i=0; i<s; ++i){
      components_[i] = other.components_[i];
    }
  }

  // Destructor
  ~bsta_mixture_fixed<dist_,s>()
  {
  }

  //: Assignment operator
  bsta_mixture_fixed<dist_,s>& operator= (const bsta_mixture_fixed<dist_,s>& rhs)
  {
    if (this != &rhs) {
       // deep copy of the data
       for (unsigned int i=0; i<s; ++i) {
          components_[i] = rhs.components_[i];
       }
       num_components_ = rhs.num_components_;
    }
    return *this;
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return num_components_; }

  //: Access (const) a component distribution of the mixture
  const dist_& distribution(unsigned int index) const
  { return components_[index].distribution; }

  //: Access a component distribution of the mixture
  dist_& distribution(unsigned int index)
  { return components_[index].distribution; }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return components_[index].weight; }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w) { components_[index].weight = w; }

  //: Insert a new component in the next location in the array
  bool insert(const dist_& d, const T& weight = T(0))
  {
    if (num_components_ >= s)
      return false;

    components_[num_components_++] = component(d, weight);
    return true;
  }

  //: Remove the last component in the vector
  void remove_last() { components_[--num_components_].weight = T(0); }

  //: Compute the probability density at this point
  // \note assumes weights have been normalized
  T prob_density(const vector_& pt) const
  {
    T prob = 0;

    for (unsigned i=0; i<num_components_; ++i)
      prob += components_[i].weight
            * components_[i].distribution.prob_density(pt);
    return prob;
  }

  //: The probability integrated over a box
  // \note assumes weights have been normalized
  T probability(const vector_& min_pt, const vector_& max_pt) const
  {
    T prob = 0;

    for (unsigned i=0; i<num_components_; ++i)
      prob += components_[i].weight
      * components_[i].distribution.probability(min_pt,max_pt);
    return prob;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    T sum = 0;
    for (unsigned i=0; i<num_components_; ++i)
      sum += components_[i].weight;
    assert(sum > 0);
    for (unsigned i=0; i<num_components_; ++i)
      components_[i].weight /= sum;
  }

  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_, components_+num_components_, sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // The prototype should be
  // \code
  // template <class T>
  // bool functor(const bsta_distribution<T>& d1, const T& w1,
  //              const bsta_distribution<T>& d2, const T& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp)
  { vcl_sort(components_, components_+num_components_, sort_adaptor<comp_type_>(comp)); }

  //: Sort the first components up to index idx
  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx)
  { assert(idx < s);
    vcl_sort(components_, components_+idx+1, sort_adaptor<comp_type_>(comp)); }
};


#endif // bsta_mixture_fixed_h_
