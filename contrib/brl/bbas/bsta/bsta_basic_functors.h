// This is brl/bbas/bsta/bsta_basic_functors.h
#ifndef bsta_basic_functors_h_
#define bsta_basic_functors_h_
//:
// \file
// \brief Basic functors for simple operations on Gaussian mixtures
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/30/06
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim


//: A functor to return the probability density at a sample
template <class dist_>
class bsta_prob_density_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef typename dist_::vector_type vector_;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const dist_& d, const vector_& sample, return_T& retval ) const
    {
      retval = d.prob_density(sample);
      return true;
    }
};

//: A functor to return the probability integrated over a box
template <class dist_>
class bsta_probability_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef typename dist_::vector_type vector_;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const dist_& d, const vector_& min_pt,
                      const vector_& max_pt, return_T& retval ) const
    {
      retval = d.probability(min_pt,max_pt);
      return true;
    }
};

//: A functor to return the mean of the Gaussian
// \note the distribution must be Gaussian
template <class dist_>
class bsta_mean_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef typename dist_::vector_type vector_;
    typedef vector_ return_T;
    enum { return_dim = dist_::dimension };

    //: The main function
    bool operator() ( const dist_& d, return_T& retval ) const
    {
      retval = d.mean();
      return true;
    }
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be spherical Gaussian
template <class dist_>
class bsta_var_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const dist_& d, return_T& retval ) const
    {
      retval = d.var();
      return true;
    }
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be spherical Gaussian
template <class dist_>
class bsta_diag_covar_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef typename dist_::vector_type vector_;
    typedef vector_ return_T;
    enum { return_dim = dist_::dimension };

    //: The main function
    bool operator() ( const dist_& d, return_T& retval ) const
    {
      retval = d.diag_covar();
      return true;
    }
};


//: A functor to return the determinant of the covariance of the Gaussian
// \note the distribution must be Gaussian
template <class dist_>
class bsta_det_covar_functor
{
  public:
    typedef typename dist_::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const dist_& d, return_T& retval ) const
    {
      retval = d.det_covar();
      return true;
    }
};


//: A functor to return the weight of the component with given index
// \note the distribution must be a mixture
template <class mixture_>
class bsta_weight_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef T return_T;
    enum { return_dim = 1 };

    //: Constructor
    bsta_weight_functor(unsigned int index = 0) : idx(index) {}

    //: The main function
    bool operator() ( const mixture_& mix, return_T& retval ) const
    {
      if (idx < mix.num_components()){
        retval = mix.weight(idx);
        return true;
      }
      return false;
    }

    unsigned int idx;
};


//: A functor to apply another functor to one distribution in the mixture
// \note the distribution must be a mixture
template <class mixture_, class functor_>
class bsta_mixture_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef typename functor_::return_T return_T;
    enum { return_dim = functor_::return_dim };

    //: Constructor
    bsta_mixture_functor(const functor_& f, unsigned int index = 0 )
    : functor(f), idx(index) {}

    //: The main function
    bool operator() ( const mixture_& mix, return_T& retval ) const
    {
      if (idx < mix.num_components() && mix.weight(idx) > T(0)){
        return functor(mix.distribution(idx),retval);
      }
      return false;
    }

    //: The functor to apply
    functor_ functor;
    //: The index to apply to
    unsigned int idx;
};


//: A functor to apply another functor with data to one distribution in the mixture
// \note the distribution must be a mixture
template <class mixture_, class functor_>
class bsta_mixture_data_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef typename functor_::return_T return_T;
    typedef typename mixture_::vector_type vector_;
    enum { return_dim = functor_::return_dim };

    //: Constructor
    bsta_mixture_data_functor(const functor_& f, unsigned int index = 0 )
    : functor(f), idx(index) {}

    //: The main function
    bool operator() ( const mixture_& mix, const vector_& sample, return_T& retval ) const
    {
      if (idx < mix.num_components() && mix.weight(idx) > T(0)){
        return functor(mix.distribution(idx),sample,retval);
      }
      return false;
    }

    //: The functor to apply
    functor_ functor;
    //: The index to apply to
    unsigned int idx;
};


//: A functor to apply another functor to each distribution and produce a weighted sum
// \note the distribution must be a mixture
template <class mixture_, class functor_>
class bsta_weighted_sum_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef typename functor_::return_T return_T;
    enum { return_dim = functor_::return_dim };

    //: Constructor
    bsta_weighted_sum_functor() : functor() {}
    //: Constructor
    bsta_weighted_sum_functor(const functor_& f):
        functor(f) {}

    //: The main function
    bool operator() ( const mixture_& mix, return_T& retval ) const
    {
      const unsigned int nc = mix.num_components();
      if (nc > 0){
        return_T temp;
        if ( !functor(mix.distribution(0),temp) )
          return false;
        retval = mix.weight(0) * temp;
        for (unsigned int idx=1; idx<nc; ++idx){
          if ( !functor(mix.distribution(idx),temp) )
            return false;
          retval += mix.weight(idx) * temp;
        }
        return true;
      }
      return false;
    }

    //: The functor to apply
    functor_ functor;
};


//: A functor to apply another functor with data to each distribution and produce a weighted sum
// \note the distribution must be a mixture
template <class mixture_, class functor_>
class bsta_weighted_sum_data_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef typename functor_::return_T return_T;
    typedef typename mixture_::vector_type vector_;
    enum { return_dim = functor_::return_dim };

    //: Constructor
    bsta_weighted_sum_data_functor() : functor() {}
    //: Constructor
    bsta_weighted_sum_data_functor(const functor_& f):
        functor(f) {}

    //: The main function
    bool operator() ( const mixture_& mix, const vector_& sample, return_T& retval ) const
    {
      const unsigned int nc = mix.num_components();
      if (nc > 0)
      {
        return_T temp;
        if ( !functor(mix.distribution(0),sample,temp) )
          return false;
        retval = mix.weight(0) * temp;
        for (unsigned int idx=1; idx<nc; ++idx){
          if ( !functor(mix.distribution(idx),sample,temp) )
            return false;
          retval += mix.weight(idx) * temp;
        }
        return true;
      }
      return false;
    }

    //: The functor to apply
    functor_ functor;
};


//: A functor to count the number of components in the mixture
// \note the distribution must be a mixture
template <class mixture_>
class bsta_mixture_size_functor
{
  public:
    typedef typename mixture_::math_type T;
    typedef unsigned int return_T;
    enum { return_dim = 1 };

    //: The main function
    bool operator() ( const mixture_& mix, return_T& retval ) const
    {
      retval = mix.num_components();
      return true;
    }
};


#endif // bsta_basic_functors_h_
