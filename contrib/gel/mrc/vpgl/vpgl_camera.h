// This is gel/mrc/vpgl/vpgl_camera.h
#ifndef vpgl_camera_h_
#define vpgl_camera_h_

//:
// \file
// \brief A general camera class.
// \author Thomas Pollard
// \date 01/28/05
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
//   A basic abstract camera class on which all specific cameras are subclassed from.
//   As such there is only one operation it performs: projecting a 3d world point into
//   a 2d image point.  The class is templated over T which had better be an algebraic
//   field.

#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

template<class T>
class vpgl_camera
{
public:

  vpgl_camera(){};
  virtual ~vpgl_camera(){};

  //: Project a point in the world onto the image plane.
  virtual vgl_homg_point_2d<T> project( const vgl_homg_point_3d<T>& world_point ) const = 0;

  //: Non-homogeneous version of the above.
  vgl_homg_point_2d<T> project( const vgl_point_3d<T>& world_point ) const {
    return project( vgl_homg_point_3d<T>( world_point ) ); }
  

};


#endif // vpgl_camera_h_
