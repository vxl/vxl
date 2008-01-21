// This is brcv/seg/bsta/bsta_basic_functors.h
#ifndef bsta_basic_functors_h_
#define bsta_basic_functors_h_

//:
// \file
// \brief Basic functors for simple operations on Gaussian mixtures
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/30/06
//
// 
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_vector_fixed.h>


//: A functor to return the probability density at a sample
template <class _dist>
class bsta_prob_density_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef typename _dist::vector_type _vector;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const _dist& d, const _vector& sample, return_T& retval ) const
    {
      retval = d.prob_density(sample);
      return true;
    }
};

//: A functor to return the probability integrated over a box
template <class _dist>
class bsta_probability_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef typename _dist::vector_type _vector;
    typedef T return_T;
    enum { return_dim = 1 };
    
    //: The main function
    bool operator() ( const _dist& d, const _vector& min_pt, 
                      const _vector& max_pt, return_T& retval ) const
    {
      retval = d.probability(min_pt,max_pt);
      return true;
    }
};

//: A functor to return the mean of the Gaussian
// \note the distribution must be Gaussian
template <class _dist>
class bsta_mean_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef typename _dist::vector_type _vector;
    typedef _vector return_T;
    enum { return_dim = _dist::dimension };

    //: The main function
    bool operator() ( const _dist& d, return_T& retval ) const
    {
      retval = d.mean();
      return true;
    }

};


//: A functor to return the variance of the Gaussian
// \note the distribution must be spherical Gaussian
template <class _dist>
class bsta_var_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const _dist& d, return_T& retval ) const
    {
      retval = d.var();
      return true;
    }
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be spherical Gaussian
template <class _dist>
class bsta_diag_covar_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef typename _dist::vector_type _vector;
    typedef _vector return_T;
    enum { return_dim = _dist::dimension };

    //: The main function
    bool operator() ( const _dist& d, return_T& retval ) const
    {
      retval = d.diag_covar();
      return true;
    }
};


//: A functor to return the determinant of the covariance of the Gaussian
// \note the distribution must be Gaussian
template <class _dist>
class bsta_det_covar_functor
{
  public:
    typedef typename _dist::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const _dist& d, return_T& retval ) const
    {
      retval = d.det_covar();
      return true;
    }
};


//: A functor to return the weight of the component with given index
// \note the distribution must be a mixture
template <class _mixture>
class bsta_weight_functor
{
  public:
    typedef typename _mixture::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: Constructor
    bsta_weight_functor(unsigned int index = 0) : idx(index) {}

    //: The main function
    bool operator() ( const _mixture& mix, return_T& retval ) const
    {
      if(idx < mix.num_components()){
        retval = mix.weight(idx);
        return true;
      }
      return false;
    }

    unsigned int idx;
};


//: A functor to apply another functor to one distribution in the mixture
// \note the distribution must be a mixture
template <class _mixture, class _functor>
class bsta_mixture_functor
{
  public:
    typedef typename _mixture::math_type T;
    typedef typename _functor::return_T return_T;
    enum { return_dim = _functor::return_dim };

    //: Constructor
    bsta_mixture_functor(const _functor& f, unsigned int index = 0 )
    : functor(f), idx(index) {}

    //: The main function
    bool operator() ( const _mixture& mix, return_T& retval ) const
    {
      if(idx < mix.num_components() && mix.weight(idx) > T(0)){
        return functor(mix.distribution(idx),retval);
      }
      return false;
    }

    //: The functor to apply
    _functor functor;
    //: The index to apply to
    unsigned int idx;
};


//: A functor to apply another functor with data to one distribution in the mixture
// \note the distribution must be a mixture
template <class _mixture, class _functor>
class bsta_mixture_data_functor
{
  public:
    typedef typename _mixture::math_type T;
    typedef typename _functor::return_T return_T;
    typedef typename _mixture::vector_type _vector;
    enum { return_dim = _functor::return_dim };

    //: Constructor
    bsta_mixture_data_functor(const _functor& f, unsigned int index = 0 )
    : functor(f), idx(index) {}

    //: The main function
    bool operator() ( const _mixture& mix, const _vector& sample, return_T& retval ) const
    {
      if(idx < mix.num_components() && mix.weight(idx) > T(0)){
        return functor(mix.distribution(idx),sample,retval);
      }
      return false;
    }

    //: The functor to apply
    _functor functor;
    //: The index to apply to
    unsigned int idx;
};


//: A functor to apply another functor to each distribution and produce a weighted sum
// \note the distribution must be a mixture
template <class _mixture, class _functor>
class bsta_weighted_sum_functor 
{
  public:
    typedef typename _mixture::math_type T;
    typedef typename _functor::return_T return_T;
    enum { return_dim = _functor::return_dim };

    //: Constructor
    bsta_weighted_sum_functor() : functor() {}
    //: Constructor
    bsta_weighted_sum_functor(const _functor& f):
        functor(f) {}

    //: The main function
    bool operator() ( const _mixture& mix, return_T& retval ) const
    {
      const unsigned int nc = mix.num_components();
      if(nc > 0){
        return_T temp;
        if( !functor(mix.distribution(0),temp) )
          return false;
        retval = mix.weight(0) * temp;
        for(unsigned int idx=1; idx<nc; ++idx){
          if( !functor(mix.distribution(idx),temp) )
            return false;
          retval += mix.weight(idx) * temp;
        }
        return true;
      }
      return false;
    }

    //: The functor to apply
    _functor functor;
};


//: A functor to apply another functor with data to each distribution and produce a weighted sum
// \note the distribution must be a mixture
template <class _mixture, class _functor>
class bsta_weighted_sum_data_functor 
{
  public:
    typedef typename _mixture::math_type T;
    typedef typename _functor::return_T return_T;
    typedef typename _mixture::vector_type _vector;
    enum { return_dim = _functor::return_dim };

    //: Constructor
    bsta_weighted_sum_data_functor() : functor() {}
    //: Constructor
    bsta_weighted_sum_data_functor(const _functor& f):
        functor(f) {}

    //: The main function
    bool operator() ( const _mixture& mix, const _vector& sample, return_T& retval ) const
    {
      const unsigned int nc = mix.num_components();
      if(nc > 0){
        return_T temp;
        if( !functor(mix.distribution(0),sample,temp) )
          return false;
        retval = mix.weight(0) * temp;
        for(unsigned int idx=1; idx<nc; ++idx){
          if( !functor(mix.distribution(idx),sample,temp) )
            return false;
          retval += mix.weight(idx) * temp;
        }
        return true;
      }
      return false;
    }

    //: The functor to apply
    _functor functor;
};


//: A functor to count the number of components in the mixture
// \note the distribution must be a mixture
template <class _mixture>
class bsta_mixture_size_functor
{
  public:
    typedef typename _mixture::math_type T;
    typedef unsigned int return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const _mixture& mix, return_T& retval ) const
    {
      retval = mix.num_components();
      return true;
    }
};




#endif // bsta_basic_functors_h_
