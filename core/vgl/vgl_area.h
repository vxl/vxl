#ifndef VGL_AREA_H_
#define VGL_AREA_H_


class vgl_polygon;

//: Computes the signed area of a polygon.
// The sign is positive if the polygon is oriented counter-clockwise
// (in RH coordinate system) and negative otherwise. For "polygons"
// consisting of more than one contour, the result is valid only if
// the contours are oriented consistently. That is, an enclosed
// contour must have opposite orientation to the enclosing contour.
float
vgl_area_signed( const vgl_polygon& poly );

//: The area of a polygon.
// \sa vgl_area_signed
float
vgl_area( const vgl_polygon& poly );

#endif
