// This is core/vgl/vgl_bounding_box.h
#ifndef vgl_bounding_box_h_
#define vgl_bounding_box_h_
//:
// \file
// \brief Set of bounding box functions
// \author J.L. Mundy
//
// Computes the axis aligned bounding box for some selected vgl objects
// \verbatim
//  Modifications
//  Initial version August 30, 2015
// \endverbatim

#include "vgl_fwd.h" // forward declare various vgl classes


//: Bounding box for a sphere
template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_sphere_3d<T> & sphere);

//: Bounding box for a pointset
template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_pointset_3d<T> & ptset);

//: Bounding box for a cubic spline (based on knots, so not exact)
template <class T>
vgl_box_3d<T>
vgl_bounding_box(const vgl_cubic_spline_3d<T> & spline);

//: bounding box for a polygon
template <class T>
vgl_box_2d<T>
vgl_bounding_box(const vgl_polygon<T> & poly);
#endif // vgl_bounding_box_h_
