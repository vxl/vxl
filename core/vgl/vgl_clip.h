// This is core/vgl/vgl_clip.h
#ifndef vgl_clip_h_
#define vgl_clip_h_
//:
// \file
// \author fsm
// \verbatim
// Modifications
// 29 Apr 2002: Amitha Perera: added a polygon clipper (a wrap around for
//                             Alan Murt's Generic Polygon Clipper)
// 12 Oct 2002: Peter Vanroose: vgl_clip_line_to_box completely re-implemented
// 14 Nov 2003: Peter Vanroose: made all functions templated
// \endverbatim

#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_polygon.h>

//: Type of polygon "clip" operations.
enum vgl_clip_type
{
  vgl_clip_type_intersect,
  vgl_clip_type_union,
  vgl_clip_type_difference,
  vgl_clip_type_xor
};


//: clips away the portion where ax+by+c<0. return false if nothing left.

template <class T>
bool vgl_clip_lineseg_to_line(T &x1, T &y1, // line segment start
                              T &x2, T &y2, // and end.
                              T a, T b, T c);

//: clip line ax+by+c=0 to given box. return false if no intersection.

template <class T>
bool vgl_clip_line_to_box(T a, T b, T c, // line equation ax+by+c=0.
                          T x1,T y1,     // coordinates of
                          T x2,T y2,     // box corners.
                          T &bx, T &by,  // clipped line
                          T &ex, T &ey); // segment.


//: clip given line to given box, and return resulting line segment
// \relates vgl_box_2d

template <class T>
inline
vgl_line_segment_2d<T> vgl_clip_line_to_box(vgl_line_2d<T> const& l,
                                            vgl_box_2d<T> const& b)
{
  T sx, sy, ex, ey;
  bool r = vgl_clip_line_to_box(l.a(), l.b(), l.c(),
                                b.min_x(), b.min_y(), b.max_x(), b.max_y(),
                                sx, sy, ex, ey);
  return r ? vgl_line_segment_2d<T>(vgl_point_2d<T>(sx, sy),
                                    vgl_point_2d<T>(ex, ey))
           : vgl_line_segment_2d<T>(); // uninitialised when no intersection
}

//: Clip a polygon against another polygon.
// The two polygons poly1 and poly2 are combined with each other.
// The operation (intersection, union, etc) is given by parameter op.
//
// \note The implementation of this code is based on Alan Murta's GPC
// library (http://www.cs.man.ac.uk/aig/staff/alan/software/gpc.html)
// which is free for non-commercial use.
//
// \relates vgl_polygon
template <class T>
vgl_polygon<T>
vgl_clip( const vgl_polygon<T>& poly1, const vgl_polygon<T>& poly2,
          vgl_clip_type op = vgl_clip_type_intersect );

#define VGL_CLIP_INSTANTIATE(T) extern "please include vgl/vgl_clip.txx instead"

#endif // vgl_clip_h_
