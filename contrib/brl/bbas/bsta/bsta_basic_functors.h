// This is brl/bbas/bsta/bsta_basic_functors.h
#ifndef bsta_basic_functors_h_
#define bsta_basic_functors_h_
//:
// \file
// \brief Basic functors for simple operations on Gaussian mixtures
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 30, 2006
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <vpdl/vpdt/vpdt_enable_if.h>
#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vpdl/vpdt/vpdt_mixture_accessors.h>

//: A functor to return the probability density at a sample
template <class dist_>
class bsta_prob_density_functor
{
 public:
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vector_;
  typedef T return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };

  //: The main function
  bool operator() ( const dist_& d, const vector_& sample, return_T& retval ) const
  {
    retval = d.prob_density(sample);
    return true;
  }
};

//: A functor to return the probability density with additional covariance
template <class mix_dist_>
class bsta_prob_density_addcovar_functor
{
 public:
  typedef typename mix_dist_::dist_type::math_type T;
  typedef typename mix_dist_::dist_type::vector_type vector_;
  typedef typename mix_dist_::dist_type::covar_type covar_t_;
  typedef T return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };
  //: The main function
  bool operator() ( const mix_dist_& d, const vector_& sample,
                    const covar_t_& add_covar, return_T& retval ) const
  {
    unsigned nc = d.num_components();
    std::vector<covar_t_> initial_covars(nc);
    mix_dist_& non_const_d = const_cast<mix_dist_&>(d);
    for (unsigned i = 0; i<nc; ++i) {
      initial_covars[i]=(d.distribution(i)).covar();
      non_const_d.distribution(i).set_covar(initial_covars[i]+add_covar);
    }
    retval = non_const_d.prob_density(sample);
    for (unsigned i = 0; i<nc; ++i)
      non_const_d.distribution(i).set_covar(initial_covars[i]);
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };

  //: The main function
  bool operator() ( const dist_& d, const vector_& min_pt,
                    const vector_& max_pt, return_T& retval ) const
  {
    retval = d.probability(min_pt,max_pt);
    return true;
  }
};

//: A functor to return the probability with added covariance
template <class mix_dist_>
class bsta_probability_addcovar_functor
{
 public:
  typedef typename mix_dist_::dist_type::math_type T;
  typedef typename mix_dist_::dist_type::vector_type vector_;
  typedef typename mix_dist_::dist_type::covar_type covar_t_;
  typedef T return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };
  //: The main function
  bool operator() ( const mix_dist_& d,
                    const vector_& min_pt,
                    const vector_& max_pt,
                    const covar_t_& add_covar,
                    return_T& retval ) const
  {
    unsigned nc = d.num_components();
    mix_dist_& non_const_d = const_cast<mix_dist_&>(d);
    std::vector<covar_t_> initial_covars(nc);
    for (unsigned i = 0; i<nc; ++i){
      initial_covars[i]=(d.distribution(i)).covar();
      non_const_d.distribution(i).set_covar(initial_covars[i]+add_covar);
    }
    retval = d.probability(min_pt, max_pt);
    for (unsigned i = 0; i<nc; ++i)
      non_const_d.distribution(i).set_covar(initial_covars[i]);
    return true;
  }
};

//: A functor to return the mean of the Gaussian
// \note the distribution must be Gaussian
template <class dist_, class Disambiguate = void>
class bsta_mean_functor
{
 public:
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vector_;
  typedef vector_ return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;
  enum { return_dim = dist_::dimension };

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_mean_functor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_& d, return_T& retval ) const
  {
    retval = d.mean();
    return true;
  }
};

//: for compatibility with vpdl/vpdt
template <class dist_>
class bsta_mean_functor<dist_, typename vpdt_enable_if<vpdt_is_mixture<dist_> >::type >
{
 public:
  typedef typename dist_::field_type return_type;
  enum { return_dim = dist_::dimension };
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_mean_functor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_& /*d*/, return_type& /*retval*/ ) const
  {
    return false;
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_var_functor<other_dist> other;
  };

  //: The main function
#if 0
  bool operator() (dist_ const& d, return_T& retval) const
  {
    retval = d.var();
    return true;
  }
#else
  bool operator() (dist_ const&, return_T&) const
  {
    return false;
  }
#endif
};

//: A functor to return the variance of the Gaussian
// \note the distribution must be a spherical Gaussian with one dimensions
//       the default template does nothing.
//       This solution is really just a hack.
//       The correct solution requires is_base_of from Boost or TR1.
//       This class should work for any derived class of bsta_gaussian_indep
template <class T>
class bsta_var_functor<bsta_num_obs<bsta_gaussian_sphere<T,1> > >
{
 public:
  typedef bsta_gaussian_sphere<T,1> dist_;
  typedef typename dist_::vector_type vector_;
  typedef vector_ return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = dist_::dimension };
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_var_functor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_& d, return_T& retval ) const
  {
    retval = d.var();
    return true;
  }
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be an independent Gaussian
//       the default template does nothing
template <class dist_>
class bsta_diag_covar_functor
{
 public:
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vector_;
  typedef vector_ return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = dist_::dimension };
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_diag_covar_functor<other_dist> other;
  };

  //: The main function
#if 0
  bool operator() (dist_ const& d, return_T& retval) const
  {
    retval = d.var();
    return true;
  }
#else
  bool operator() (dist_ const&, return_T&) const
  {
    return false;
  }
#endif
};


//: A functor to return the variance of the Gaussian
// \note the distribution must be an independent Gaussian
//       the default template does nothing.
//       This solution is really just a hack.
//       The correct solution requires is_base_of from Boost or TR1.
//       This class should work for any derived class of bsta_gaussian_indep
template <class T, unsigned n>
class bsta_diag_covar_functor<bsta_num_obs<bsta_gaussian_indep<T,n> > >
{
 public:
  typedef bsta_gaussian_indep<T,n> dist_;
  typedef typename dist_::vector_type vector_;
  typedef vector_ return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = dist_::dimension };
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_diag_covar_functor<other_dist> other;
  };

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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };

  //: The main function
  bool operator() ( const dist_& d, return_T& retval ) const
  {
    retval = d.det_covar();
    return true;
  }
};


//: A functor to return the weight of the component with given index
// "Disambiguate" is for compatibility with vpdl/vpdt.
// \note the distribution must be a mixture
template <class mixture_, class Disambiguate=void>
class bsta_weight_functor
{
 public:
  typedef typename mixture_::math_type T;
  typedef T return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_weight_functor<other_dist> other;
  };

  //: Constructor
  bsta_weight_functor(unsigned int index = 0) {}

  //: The main function
  bool operator() ( const mixture_& mix, return_T& retval ) const
  {
    return false;
  }
};


//: A functor to return the weight of the component with given index
// \note the distribution must be a mixture
template <class mixture_>
class bsta_weight_functor<mixture_,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_> >::type>
{
 public:
  typedef typename mixture_::math_type T;
  typedef T return_T;
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef bsta_weight_functor<other_dist> other;
  };

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


//: A vpdt specialization to make the weight functor work as a mixture accessor
// This is needed because weight is not a property of the mixture component,
// it is a property on the mixture itself that is defined for each component.
template <class mixture_>
class vpdt_mixture_accessor<mixture_,
          bsta_weight_functor<typename mixture_::component_type>,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_> >::type>
{
 public:
  //: the accessor type
  typedef bsta_weight_functor<typename mixture_::component_type> accessor_type;
  //: the functor return type
  typedef typename vpdt_dist_traits<mixture_>::scalar_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_ distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist, class other_accessor = accessor_type>
  struct rebind {
    typedef vpdt_mixture_accessor<other_dist,other_accessor> other;
  };

  //: Constructor
  vpdt_mixture_accessor(unsigned int index = 0)
  : idx(index) {}

  //: Constructor
  vpdt_mixture_accessor(const accessor_type& a, unsigned int index = 0)
  : idx(index) {}

  //: The main function
  bool operator() ( const mixture_& mix, return_type& retval ) const
  {
    if (idx < mix.num_components()){
      retval = mix.weight(idx);
      return true;
    }
    return false;
  }

  //: The component index
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = functor_::return_dim };

  //: Constructor
  bsta_weighted_sum_functor() : functor() {}
  //: Constructor
  bsta_weighted_sum_functor(const functor_& f)
  : functor(f) {}

  //: The main function
  bool operator() ( const mixture_& mix, return_T& retval ) const
  {
    const unsigned int nc = mix.num_components();
    if (nc > 0) {
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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  typedef typename mixture_::vector_type vector_;
  enum { return_dim = functor_::return_dim };

  //: Constructor
  bsta_weighted_sum_data_functor() : functor() {}
  //: Constructor
  bsta_weighted_sum_data_functor(const functor_& f)
  : functor(f) {}

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
  typedef return_T return_type; // for compatibility with vpdl/vdpt
  enum { return_dim = 1 };

  //: The main function
  bool operator() ( const mixture_& mix, return_T& retval ) const
  {
    retval = mix.num_components();
    return true;
  }
};


#endif // bsta_basic_functors_h_
