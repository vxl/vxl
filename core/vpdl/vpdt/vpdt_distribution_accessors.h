// This is  core/vpdl/vpdt/vpdt_distribution_accessors.h
#ifndef vpdt_distribution_accessors_h_
#define vpdt_distribution_accessors_h_
//:
// \file
// \brief Accessor functors that apply to all distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 15, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vpdl/vpdt/vpdt_dist_traits.h>


//: An accessor to return the variable dimension of the distribution
template <class dist_type>
class vpdt_dimension_accessor
{
public:
  //: the functor return type
  typedef unsigned int return_type;
  //: the distribution operated on by the functor
  typedef dist_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_dimension_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_type& d, return_type& retval ) const
  {
    retval = d.dimension();
    return true;
  }
};


//: An accessor to return the mean of the distribution
template <class dist_type>
class vpdt_mean_accessor
{
public:
  //: the functor return type
  typedef typename dist_type::field_type return_type;
  //: the distribution operated on by the functor
  typedef dist_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_mean_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_type& d, return_type& retval ) const
  {
    d.compute_mean(retval);
    return true;
  }
};


//: An accessor to return the covariance of the distribution
template <class dist_type>
class vpdt_covar_accessor
{
public:
  //: the functor return type
  typedef typename vpdt_dist_traits<dist_type>::matrix_type return_type;
  //: the distribution operated on by the functor
  typedef dist_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_covar_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_type& d, return_type& retval ) const
  {
    d.compute_covar(retval);
    return true;
  }
};


//: An accessor to return the normalization constant of the distribution
template <class dist_type>
class vpdt_norm_const_accessor
{
public:
  //: the functor return type
  typedef typename vpdt_dist_traits<dist_type>::scalar_type return_type;
  //: the distribution operated on by the functor
  typedef dist_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_norm_const_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const dist_type& d, return_type& retval ) const
  {
    retval = d.norm_const();
    return true;
  }
};


#endif // vpdt_distribution_accessors_h_
