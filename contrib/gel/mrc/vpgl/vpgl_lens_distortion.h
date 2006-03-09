// This is gel/mrc/vpgl/vpgl_lens_distortion.h
#ifndef vpgl_lens_distortion_h_
#define vpgl_lens_distortion_h_

//:
// \file
// \brief An abstract base class for all lens distortions.
// \author Matt Leotta
// \date 08/19/05
//
//   A lens distortion is a 2D warping of the image plane to account for lens effects
//   not accounted for by the simple camera models.  It is assumed that the map is
//   bijective, though a closed form solution for the inverse may not exist in general.
//   A default iterative solver is implemented to solve 

//: forward delcare
template <class T> class vgl_homg_point_2d;
template <class T> class vgl_vector_2d;


//: A base class for lens distortions
template <class T>
class vpgl_lens_distortion
{
public:

  virtual ~vpgl_lens_distortion(){}

  //: Distort a projected point on the image plane
  virtual vgl_homg_point_2d<T> distort( const vgl_homg_point_2d<T>& point ) const = 0;

  //: Return the original point that was distorted to this location (inverse of distort)
  // \param init is an initial guess at the solution for the iterative solver
  // if \p init is NULL then \p point is used as the initial guess
  virtual vgl_homg_point_2d<T> undistort( const vgl_homg_point_2d<T>& point, 
                                          const vgl_homg_point_2d<T>* init=0) const;

  //: Set a translation to apply before of after distortion
  // This is needed when distorting an image to translate the resulting image
  // such that all points have positive indices          
  virtual void set_translation(const vgl_vector_2d<T>& offset, bool after = true) = 0;

};


#endif // vpgl_lens_distortion_h_
