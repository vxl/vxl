// This is  core/vpdl/vpdt/vpdt_mixture_accessors.h
#ifndef vpdt_mixture_accessors_h_
#define vpdt_mixture_accessors_h_
//:
// \file
// \brief Accessor functors that apply to all mixture distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 15, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vpdl/vpdt/vpdt_enable_if.h>


//: A functor to return the number of components in a mixture
// This is a dummy class for non-mixture types; it always fails
template <class mixture_type, class Disambiguate=void>
class vpdt_num_components_accessor
{
 public:
  //: the functor return type
  typedef unsigned int return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_num_components_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
  {
    return false;
  }
};

//
//: A functor to return the number of components in a mixture
template <class mixture_type>
class vpdt_num_components_accessor<mixture_type,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_type> >::type>
{
 public:
  //: the functor return type
  typedef unsigned int return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_num_components_accessor<other_dist> other;
  };

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
  {
    retval = mix.num_components();
    return true;
  }
};


//: A functor to return the weight of the component with given index
template <class mixture_type, class Disambiguate=void>
class vpdt_weight_accessor
{
 public:
  //: the functor return type
  typedef typename vpdt_dist_traits<mixture_type>::scalar_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_weight_accessor<other_dist> other;
  };

  //: Constructor
  vpdt_weight_accessor(unsigned int index = 0) {}

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
  {
    return false;
  }
};


//: A functor to return the weight of the component with given index
template <class mixture_type>
class vpdt_weight_accessor<mixture_type,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_type> >::type>
{
 public:
  //: the functor return type
  typedef typename vpdt_dist_traits<mixture_type>::scalar_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist>
  struct rebind {
    typedef vpdt_weight_accessor<other_dist> other;
  };

  //: Constructor
  vpdt_weight_accessor(unsigned int index = 0) : idx(index) {}

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
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


//: A functor to apply another functor to the component with given index
template <class mixture_type, class accessor_type, class Disambiguate=void>
class vpdt_mixture_accessor
{
 public:
  //: the functor return type
  typedef typename accessor_type::return_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = false;

  //: rebind this functor to another distribution type
  template <class other_dist, class other_accessor = accessor_type>
  struct rebind {
    typedef vpdt_mixture_accessor<other_dist,other_accessor> other;
  };

  //: Constructor
  vpdt_mixture_accessor(unsigned int index = 0) {}

  //: Constructor
  vpdt_mixture_accessor(const accessor_type& a, unsigned int index = 0) {}

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
  {
    return false;
  }
};


//: A functor to apply another functor to the component with given index
template <class mixture_type, class accessor_type>
class vpdt_mixture_accessor<mixture_type,accessor_type,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_type> >::type>
{
 public:
  //: the functor return type
  typedef typename accessor_type::return_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
  //: is this functor valid for its distribution type
  static const bool valid_functor = true;

  //: rebind this functor to another distribution type
  template <class other_dist, class other_accessor = accessor_type>
  struct rebind {
    typedef vpdt_mixture_accessor<other_dist,other_accessor> other;
  };

  //: Constructor
  vpdt_mixture_accessor(unsigned int index = 0)
  : accessor(), idx(index) {}

  //: Constructor
  vpdt_mixture_accessor(const accessor_type& a, unsigned int index = 0)
  : accessor(a), idx(index) {}

  //: The main function
  bool operator() ( const mixture_type& mix, return_type& retval ) const
  {
    if (idx < mix.num_components()){
      return accessor(mix.distribution(idx),retval);
    }
    return false;
  }

  //: The accessor to apply
  accessor_type accessor;
  //: The component index
  unsigned int idx;
};


//: A specialization to make the weight accessor work as a mixture accessor
template <class mixture_type>
class vpdt_mixture_accessor<mixture_type,
          vpdt_weight_accessor<typename mixture_type::component_type>,
          typename vpdt_enable_if<vpdt_is_mixture<mixture_type> >::type>
{
 public:
  //: the accessor type
  typedef vpdt_weight_accessor<typename mixture_type::component_type> accessor_type;
  //: the functor return type
  typedef typename vpdt_dist_traits<mixture_type>::scalar_type return_type;
  //: the distribution operated on by the functor
  typedef mixture_type distribution_type;
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
  bool operator() ( const mixture_type& mix, return_type& retval ) const
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


#endif // vpdt_mixture_accessors_h_
