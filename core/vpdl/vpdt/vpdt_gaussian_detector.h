// This is core/vpdl/vpdt/vpdt_gaussian_detector.h
#ifndef vpdt_gaussian_detector_h_
#define vpdt_gaussian_detector_h_
//:
// \file
// \brief Detectors applying to Gaussians
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 11, 2009
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim

#include <vpdl/vpdt/vpdt_field_traits.h>

//: A simple Mahalanobis distance detector for a Gaussian
//  Detects samples that lie within some Mahalanobis distance
template <class gaussian_type>
class vpdt_gaussian_mthresh_detector
{
 public:
  //: the functor return type
  typedef bool return_type;
  //: the functor return type
  static const unsigned int return_dim = 1;
  //: the distribution operated on by the detector
  typedef gaussian_type distribution_type;

  //: the data type to represent a point in the field
  typedef typename gaussian_type::field_type F;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: Constructor
  vpdt_gaussian_mthresh_detector(const T& thresh=T(2.5))
  : sqr_threshold(thresh*thresh) {}

  //: The main function
  // \retval true if the Mahalanobis distance is less than the threshold
  bool operator() (const gaussian_type& g, const F& sample, bool& result) const
  {
    result = g.sqr_mahal_dist(sample) < sqr_threshold;
    return true;
  }

  //: the threshold on Mahalanobis distance
  T sqr_threshold;
};


#endif // vpdt_gaussian_detector_h_
