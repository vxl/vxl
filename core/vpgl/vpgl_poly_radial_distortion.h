// This is core/vpgl/vpgl_poly_radial_distortion.h
#ifndef vpgl_poly_radial_distortion_h_
#define vpgl_poly_radial_distortion_h_
//:
// \file
// \brief A class for polynomial radial lens distortions.
// \author Matt Leotta
// \date Aug 19, 2005
//
//   A radial lens distortion is a 2D warping of the image plane that is radial symmetric
//   about some center of distortion.  It is assumed that the map is
//   bijective, though a closed form solution for the inverse may not exist in general.
//   A default iterative solver is implemented to solve

#include <vector>
#include "vpgl_radial_distortion.h"
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: A class for nth order polynomial radial lens distortion
template <class T, int n>
class vpgl_poly_radial_distortion : public vpgl_radial_distortion<T>
{
 public:
  //: Constructor
  vpgl_poly_radial_distortion<T,n>(const vgl_point_2d<T>& center, const T* k)
   : vpgl_radial_distortion<T>(center,true)
  {
    set_coefficients(k);
  }

  //: Constructor
  vpgl_poly_radial_distortion<T,n>(const vgl_point_2d<T>& center,
                                   const vgl_point_2d<T>& distorted_center,
                                   const T* k)
   : vpgl_radial_distortion<T>(center,distorted_center,true)
  {
    set_coefficients(k);
  }

  //: Constructor
  vpgl_poly_radial_distortion<T,n>(const vgl_point_2d<T>& center,
                                   const std::vector<T>& k)
   : vpgl_radial_distortion<T>(center,true)
  {
    set_coefficients(k);
  }

  //: Constructor
  vpgl_poly_radial_distortion<T,n>(const vgl_point_2d<T>& center,
                                   const vgl_point_2d<T>& distorted_center,
                                   const std::vector<T>& k)
   : vpgl_radial_distortion<T>(center, distorted_center,true)
  {
    set_coefficients(k);
  }

  void set_coefficients(const std::vector<T>& k)
  {
    assert(k.size() == n);
    T* coptr = coefficients_;
    for (unsigned int i=0; i<n; ++i, ++coptr)
      *coptr = k[i];
  }

  void set_coefficients(const T* k)
  {
    if ( k == nullptr ) return;
    const T* kptr = k;
    T* coptr = coefficients_;
    for (unsigned int i=0; i<n; ++i, ++kptr, ++coptr)
      *coptr = *kptr;
  };

  //: Read-only coefficient accessor
  T coefficient( unsigned int i ) const
  {
    assert( i < n );
    return this->coefficients_[i];
  }

  //: Read-write coefficient accessor
  T& coefficient( unsigned int i )
  {
    assert( i < n );
    return this->coefficients_[i];
  }

  //: Distort a radial length
  T distort_radius( T radius ) const override;

  //: Compute the derivative of the distort_radius function
  T distort_radius_deriv( T radius ) const override;

 protected:
  //: The coefficients of the nth-order polynomial
  T coefficients_[n];
};


#endif // vpgl_poly_radial_distortion_h_
