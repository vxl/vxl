// This is brl/bbas/bsta/bsta_mixture.h
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
//   Jan 21 2008  -  Matt Leotta  -  Rename probability to prob_density and
//                                   add probability integration over a box
// \endverbatim

#include "bsta_distribution.h"
#include <vcl_cassert.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

//: A mixture of distributions
template <class dist_>
class bsta_mixture : public bsta_distribution<typename dist_::math_type,
                                                       dist_::dimension>
{
 public:
  typedef dist_ dist_type;
  // unlimited number of component is indicated by 0
  enum { max_components = 0 };

 private:
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vector_;

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
    bool operator() (const component* const c1, const component* const c2) const
    { return comp(c1->distribution, c1->weight, c2->distribution, c2->weight); }
    comp_type_ comp;
  };

  //: The vector of components
  vcl_vector<component*> components_;

 public:
  // Default Constructor
  bsta_mixture<dist_>() {}

  // Copy Constructor
  bsta_mixture<dist_>(const bsta_mixture<dist_>& other)
    : components_(other.components_.size(),NULL)
  {
    // deep copy of the data
    for (unsigned int i=0; i<components_.size(); ++i){
      components_[i] = new component(*other.components_[i]);
    }
  }

  // Destructor
  ~bsta_mixture<dist_>()
  {
    for (unsigned int i=0; i<components_.size(); ++i){
      delete components_[i];
    }
  }

  //: Assignment operator
  bsta_mixture<dist_>& operator= (const bsta_mixture<dist_>& rhs)
  {
    if (this != &rhs){
      for (unsigned int i=0; i<components_.size(); ++i){
        delete components_[i];
      }
      components_.clear();
      for (unsigned int i=0; i<rhs.components_.size(); ++i){
        components_.push_back(new component(*rhs.components_[i]));
      }
    }
    return *this;
  }

  //: Return the number of components in the mixture
  unsigned int num_components() const { return components_.size(); }

  //: Access (const) a component distribution of the mixture
  const dist_& distribution(unsigned int index) const
  { return components_[index]->distribution; }

  //: Access a component distribution of the mixture
  dist_& distribution(unsigned int index)
  { return components_[index]->distribution; }

  //: Return the weight of a component in the mixture
  T weight(unsigned int index) const { return components_[index]->weight; }

  //: Set the weight of a component in the mixture
  void set_weight(unsigned int index, const T& w)
  { components_[index]->weight = w; }

  //: Insert a new component at the end of the vector
  bool insert(const dist_& d, const T& weight = T(0))
  { components_.push_back(new component(d, weight)); return true; }

  //: Remove the last component in the vector
  void remove_last() { delete components_.back(); components_.pop_back(); }

  //: Compute the probability density at this point
  // \note assumes weights have been normalized
  T prob_density(const vector_& pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      prob += (*i)->weight * (*i)->distribution.prob_density(pt);
    return prob;
  }

  //: The probability integrated over a box
  // \note assumes weights have been normalized
  T probability(const vector_& min_pt, const vector_& max_pt) const
  {
    typedef typename vcl_vector<component*>::const_iterator comp_itr;
    T prob = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      prob += (*i)->weight * (*i)->distribution.probability(min_pt,max_pt);
    return prob;
  }

  //: Normalize the weights of the components to add to 1.
  void normalize_weights()
  {
    typedef typename vcl_vector<component*>::iterator comp_itr;
    T sum = 0;
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      sum += (*i)->weight;
    assert(sum > 0);
    for (comp_itr i = components_.begin(); i != components_.end(); ++i)
      (*i)->weight /= sum;
  }

  //: Sort the components in order of decreasing weight
  void sort() { vcl_sort(components_.begin(), components_.end(), sort_weight() ); }

  //: Sort the components using any StrictWeakOrdering function
  // The prototype should be
  // \code
  // template <class T>
  // bool functor(const bsta_distribution<T>& d1, const T& w1,
  //              const bsta_distribution<T>& d2, const T& w2);
  // \endcode
  template <class comp_type_>
  void sort(comp_type_ comp)
  { vcl_sort(components_.begin(), components_.end(), sort_adaptor<comp_type_>(comp)); }

  template <class comp_type_>
  void sort(comp_type_ comp, unsigned int idx)
  { vcl_sort(components_.begin(), components_.begin()+idx+1, sort_adaptor<comp_type_>(comp)); }
};

template <class dist_>
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_mixture<dist_> const& m)
{
  typedef typename dist_::math_type T;
  unsigned n = m.num_components();
  for (unsigned c = 0; c<n; ++c){
    const dist_& mc = m.distribution(c);
    T weight = m.weight(c);
    os << "mixture_comp["<< c << "]wgt(" << weight << ")\n" << mc << '\n';
  }
  return os;
}

#endif // bsta_mixture_h_
