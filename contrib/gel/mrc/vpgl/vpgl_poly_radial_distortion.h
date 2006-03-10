// This is gel/mrc/vpgl/vpgl_poly_radial_distortion.h
#ifndef vpgl_poly_radial_distortion_h_
#define vpgl_poly_radial_distortion_h_

//:
// \file
// \brief A class for polynomial radial lens distortions.
// \author Matt Leotta
// \date 08/19/05
//
//   A radial lens distortion is a 2D warping of the image plane that is radial symmetric
//   about some center of distortion.  It is assumed that the map is
//   bijective, though a closed form solution for the inverse may not exist in general.
//   A default iterative solver is implemented to solve 


#include "vpgl_radial_distortion.h"
#include <vcl_vector.h>


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
                                   const vcl_vector<T>& k)
   : vpgl_radial_distortion<T>(center) 
  {
    set_coefficients(k);
  }

  //: Constructor
  vpgl_poly_radial_distortion<T,n>(const vgl_point_2d<T>& center, 
                                   const vgl_point_2d<T>& distorted_center,
                                   const vcl_vector<T>& k)
   : vpgl_radial_distortion<T>(center, distorted_center) 
  {
    set_coefficients(k);
  }

  void set_coefficients(const vcl_vector<T>& k)
  {
    assert(k.size() == n);
    T* coptr = coefficients_;
    for(unsigned int i=0; i<n; ++i, ++coptr)
      *coptr = k[i]; 
  }

  void set_coefficients(const T* k)
  {
    const T* kptr = k; 
    T* coptr = coefficients_;
    for(unsigned int i=0; i<n; ++i, ++kptr, ++coptr)
      *coptr = *kptr;
  };

  //: Distort a radial length
  virtual T distort_radius( T radius ) const;

  //: Compute the derivative of the distort_radius function
  virtual T distort_radius_deriv( T radius ) const;

protected:
  //: The coefficients of the nth-order polynomial
  T coefficients_[n];
};


#endif // vpgl_poly_radial_distortion_h_
