// This is core/vpdl/vpdt/vpdt_mixture_detector.h
#ifndef vpdt_mixture_detector_h_
#define vpdt_mixture_detector_h_
//:
// \file
// \brief Detectors applying to mixtures
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 11, 2009
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <vpdl/vpdt/vpdt_field_traits.h>

//: Apply a detector to each component to see if ANY match
//  Return true if any component matches
template <class mixture_type, class detector_type>
class vpdt_mixture_any_detector
{
 public:
  //: the functor return type
  typedef bool return_type;
  //: the functor return type
  static const unsigned int return_dim = 1;
  //: the distribution operated on by the detector
  typedef mixture_type distribution_type;

  //: the data type to represent a point in the field
  typedef typename mixture_type::field_type F;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mixture_any_detector(const detector_type& d) : detect(d) {}

  //: The main function
  bool operator() (const mixture_type& mix, const F& sample, bool& result) const
  {
    result = false;
    for (unsigned int i=0; i<mix.num_components(); ++i){
      if ( !detect(mix.distribution(i),sample,result) )
        return false;
      if (result)
        return true;
    }
    return true;
  }

  //: The detector to apply to components
  detector_type detect;
};


//: Apply a detector to each component to see if ALL match
//  Return true if all components match
template <class mixture_type, class detector_type>
class vpdt_mixture_all_detector
{
 public:
  //: the functor return type
  typedef bool return_type;
  //: the functor return type
  static const unsigned int return_dim = 1;
  //: the distribution operated on by the detector
  typedef mixture_type distribution_type;

  //: the data type to represent a point in the field
  typedef typename mixture_type::field_type F;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mixture_all_detector(const detector_type& d) : detect(d) {}

  //: The main function
  bool operator() (const mixture_type& mix, const F& sample, bool& result) const
  {
    result = true;
    for (unsigned int i=0; i<mix.num_components(); ++i){
      if ( !detect(mix.distribution(i),sample,result) )
        return false;
      if (!result)
        return true;
    }
    return true;
  }

  //: The detector to apply to components
  detector_type detect;
};


//: Apply a detector to each component in order while the total weight is below a threshold.
//  Return true if any tested component matches
template <class mixture_type, class detector_type>
class vpdt_mixture_top_weight_detector
{
 public:
  //: the functor return type
  typedef bool return_type;
  //: the functor return type
  static const unsigned int return_dim = 1;
  //: the distribution operated on by the detector
  typedef mixture_type distribution_type;

  //: the data type to represent a point in the field
  typedef typename mixture_type::field_type F;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_mixture_top_weight_detector(const detector_type& d, const T& w=T(0.5))
    : detect(d), weight_thresh(w) {}

  //: The main function
  bool operator() (const mixture_type& mix, const F& sample, bool& result) const
  {
    T total_weight = T(0);
    result = false;
    for (unsigned int i=0; i<mix.num_components(); ++i){
      if (total_weight > weight_thresh)
        return true;
      if ( !detect(mix.distribution(i),sample,result) )
        return false;
      if (result)
        return true;
      total_weight += mix.weight(i);
    }
    return true;
  }

  //: The detector to apply to components
  detector_type detect;
  //: The total weight threshold
  T weight_thresh;
};


#endif // vpdt_mixture_detector_h_
