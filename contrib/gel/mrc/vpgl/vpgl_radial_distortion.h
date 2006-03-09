// This is gel/mrc/vpgl/vpgl_radial_distortion.h
#ifndef vpgl_radial_distortion_h_
#define vpgl_radial_distortion_h_

//:
// \file
// \brief An abstract base class for radial lens distortions.
// \author Matt Leotta
// \date 08/19/05
//
//   A radial lens distortion is a 2D warping of the image plane that is radial symmetric
//   about some center of distortion.  It is assumed that the map is
//   bijective, though a closed form solution for the inverse may not exist in general.
//   A default iterative solver is implemented to solve 


#include "vpgl_lens_distortion.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>


//: A base class for radial lens distortions
template <class T>
class vpgl_radial_distortion : public vpgl_lens_distortion<T>
{
public:
  //: Constructor
  vpgl_radial_distortion(const vgl_point_2d<T>& center, bool has_deriv=false) 
   : center_(center), distorted_center_(center), has_derivative_(has_deriv) {};
   
  //: Constructor
  vpgl_radial_distortion(const vgl_point_2d<T>& center,
                          const vgl_point_2d<T>& new_center, bool has_deriv=false) 
   : center_(center), distorted_center_(new_center), has_derivative_(has_deriv) {};

  //: Distort a projected point on the image plane
  //  calls the pure virtual radial distortion function
  virtual vgl_homg_point_2d<T> distort( const vgl_homg_point_2d<T>& point ) const;
  
  //: Return the original point that was distorted to this location (inverse of distort)
  // \param init is an initial guess at the solution for the iterative solver
  // if \p init is NULL then \p point is used as the initial guess
  // calls the radial undistortion function
  virtual vgl_homg_point_2d<T> undistort( const vgl_homg_point_2d<T>& point, 
                                          const vgl_homg_point_2d<T>* init=0) const;
                                          
  //: Distort a radial length
  // \retval a scale factor such that 
  // \code
  //   distort_pt = center + distort_radius(radius)*(pt - center)
  // \endcode
  virtual T distort_radius( T radius ) const = 0;
  
  //: Return the inverse of distort function
  // \param init is an initial guess at the solution for the iterative solver
  // if \p init is NULL then \p radius is used as the initial guess
  virtual T undistort_radius( T radius, const T* init=0) const;
  
  //: Compute the derivative of the distort_radius function
  // \note implementing this function is optional but it may improve the convergence
  //       rate of the undistort function if iterative solving is used
  // Set \p has_derivative_ to true if you define this function
  virtual T distort_radius_deriv( T radius ) const
  {
    T eps = T(0.001);
    return (distort_radius(radius) - distort_radius(radius-eps)) / eps;
  }
  
  //: Set a translation to apply before of after distortion
  // This is needed when distorting an image to translate the resulting image
  // such that all points have positive indices          
  virtual void set_translation(const vgl_vector_2d<T>& offset, bool after = true)
  {
    if(after)
      distorted_center_ += offset;
    else
      center_ += offset;
  }
  
  //: Returns the center of distortion
  vgl_point_2d<T> center() const { return center_; }
  //: Returns the center of distortion in the distorted image
  vgl_point_2d<T> distorted_center() const { return distorted_center_; }
  
  //: Set the center of distortion
  void set_center(const vgl_point_2d<T>& c) { center_ = c; }
  //: Set the center of distortion in the distorted image
  void set_distorted_center(const vgl_point_2d<T>& dc) { distorted_center_ = dc; }

protected:
  //: The center of distortion
  vgl_point_2d<T> center_;
  
  //: The center of distortion in the distorted space
  vgl_point_2d<T> distorted_center_;
  
  bool has_derivative_;
};


#endif // vpgl_radial_distortion_h_
