// This is brl/bbas/bsta/algo/bsta_mixture_functors.h
#ifndef bsta_mixture_functors_h_
#define bsta_mixture_functors_h_
//:
// \file
// \brief Functors that apply to mixtures of distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 29, 2006
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <bsta/bsta_mixture.h>


//: Compute the weighted sum of functor value while the total weight is below a threshold.
template <class mixture_, class functor_>
class bsta_top_weight_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef typename functor_::return_T return_T;
    typedef return_T return_type; // for compatibility with vpdl/vdpt
    typedef vnl_vector_fixed<T,mixture_::dimension> vector_;
    enum { return_dim = functor_::return_dim };

    //: Constructor
    bsta_top_weight_functor(const T& w=T(0.5)) : functor(), weight_thresh(w) {}
    //: Constructor
    bsta_top_weight_functor(const functor_& f, const T& w=T(0.5))
      : functor(f), weight_thresh(w) {}

    //: The main function
    bool operator() (const mixture_& mix, const vector_& sample, return_T& retval) const
    {
      const unsigned int nc = mix.num_components();
      if (nc == 0)
        return false;

      return_T temp;
      if ( !functor(mix.distribution(0),sample,temp) )
        return false;
      T w = mix.weight(0);
      T total_weight = w;
      retval = w * temp;

      for (unsigned int i=1; i<nc; ++i){
        if (total_weight > weight_thresh)
          break;
        if ( !functor(mix.distribution(i),sample,temp) )
          return false;
        w = mix.weight(i);
        total_weight += w;
        retval += w * temp;
      }
      retval /= total_weight;
      return true;
    }

    //: The functor to apply to components
    functor_ functor;
    //: The index to detect
    T weight_thresh;
};


#endif // bsta_mixture_functors_h_
