#ifndef vgl_clip_h_
#define vgl_clip_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vgl/vgl_clip.h

//:
// \file
// \author fsm@robots.ox.ac.uk
// \verbatim
// 29 Apr 2002: Amitha Perera: added a polygon clipper (a wrap around
//                Alan Murt's Generic Polygon Clipper)
// \endverbatim

#include "vgl_polygon.h"

//: Type of "clip" operations.
enum vgl_clip_type
{
  vgl_clip_type_intersect,
  vgl_clip_type_union,
  vgl_clip_type_difference,
  vgl_clip_type_xor
};


//: clips away the portion where ax+by+c<0. return false if nothing left.

bool vgl_clip_lineseg_to_line(double &x1, double &y1, // line segment start
                              double &x2, double &y2, // and end.
                              double a, double b, double c);

//: clip line ax+by+c=0 to given box. return false if no intersection.

bool vgl_clip_line_to_box(double a, double b, double c, // line equation ax+by+c=0.
                          double x1,double y1, // coordinates of
                          double x2,double y2, // box corners.
                          double &bx, double &by,  // clipped line
                          double &ex, double &ey); // segment.


//: Clip a polygon against another polygon.
// The operation (intersection, union, etc) is given by \param op.
vgl_polygon
vgl_clip( const vgl_polygon& poly1, const vgl_polygon& poly2,
          vgl_clip_type op = vgl_clip_type_intersect );

#endif // vgl_clip_h_
