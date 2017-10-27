#ifndef vgl_area_h_
#define vgl_area_h_
//:
// \file

template <class T> class vgl_polygon;
template <class T> class vgl_point_2d;
template <class T> class vgl_box_2d;
template <class T> class vgl_oriented_box_2d;
//: Computes the signed area of a polygon.
// The sign is positive if the polygon is oriented counter-clockwise
// (in RH coordinate system) and negative otherwise. For "polygons"
// consisting of more than one contour, the result is valid only if
// the contours are oriented consistently. That is, an enclosed
// contour must have opposite orientation to the enclosing contour.
// \sa vgl_area
// \relatesalso vgl_polygon
template <class T> T vgl_area_signed(vgl_polygon<T> const& poly);

//: The area of a polygon.
// \sa vgl_area_signed
// \relatesalso vgl_polygon
template <class T> T vgl_area(vgl_polygon<T> const& poly);

//: The orientation enforced area of a polygon.
// \note This method assumes that the polygon is simple (i.e. no crossings)
//  and the correct orientation is 'enforced' on the polygon (i.e. holes are
//  given negative area) to ensure that the resultant area is correct
// \sa vgl_area
// \relatesalso vgl_polygon
template <class T> T vgl_area_enforce_orientation(vgl_polygon<T> const& poly);

//: The area weighted center of a polygon
//  In general this is different than the mean of the polygon's vertices
// \relatesalso vgl_polygon
template <class T> vgl_point_2d<T> vgl_centroid(vgl_polygon<T> const& poly);

//: The area of an axis-aligned box
template <class T> T vgl_area(vgl_box_2d<T> const& box);

//: area of an oriented box
template <class T> T vgl_area(vgl_oriented_box_2d<T> const& obox);

#define VGL_AREA_INSTANTIATE(T) extern "please include vgl/vgl_area.hxx instead"

#endif // vgl_area_h_
