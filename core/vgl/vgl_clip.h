// This is vxl/vgl/vgl_clip.h
#ifndef vgl_clip_h_
#define vgl_clip_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \verbatim
// Modifications
// 29 Apr 2002: Amitha Perera: added a polygon clipper (a wrap around for
//                Alan Murt's Generic Polygon Clipper)
// \endverbatim

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_polygon.h>

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


//: clip given line to given box, and return resulting line segment

inline
vgl_line_segment_2d<double> vgl_clip_line_to_box(vgl_line_2d<double> const& l,
                                                 vgl_box_2d<double> const& b)
{
  double sx, sy, ex, ey;
  bool r = vgl_clip_line_to_box(l.a(), l.b(), l.c(),
                                b.min_x(), b.min_y(), b.max_x(), b.max_y(),
                                sx, sy, ex, ey);
  return r ? vgl_line_segment_2d<double>(vgl_point_2d<double>(sx, sy),
                                         vgl_point_2d<double>(ex, ey))
           : vgl_line_segment_2d<double>(); // uninitialised when no intersection
}

//: Clip a polygon against another polygon.
// The two polygons poly1 and poly2 are combined with each other.
// The operation (intersection, union, etc) is given by parameter op.
vgl_polygon
vgl_clip( const vgl_polygon& poly1, const vgl_polygon& poly2,
          vgl_clip_type op = vgl_clip_type_intersect );

#endif // vgl_clip_h_
