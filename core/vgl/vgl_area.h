#ifndef vgl_area_h_
#define vgl_area_h_
//:
// \file

template <class T> class vgl_polygon;

//: Computes the signed area of a polygon.
// The sign is positive if the polygon is oriented counter-clockwise
// (in RH coordinate system) and negative otherwise. For "polygons"
// consisting of more than one contour, the result is valid only if
// the contours are oriented consistently. That is, an enclosed
// contour must have opposite orientation to the enclosing contour.
// \sa vgl_area
// \relates vgl_polygon
template <class T> T vgl_area_signed(vgl_polygon<T> const& poly);

//: The area of a polygon.
// \sa vgl_area_signed
// \relates vgl_polygon
template <class T> T vgl_area(vgl_polygon<T> const& poly);

#define VGL_AREA_INSTANTIATE(T) extern "please include vgl/vgl_area.txx instead"

#endif // vgl_area_h_
