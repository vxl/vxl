// This is core/vgl/vgl_clip.txx
#ifndef vgl_clip_txx_
#define vgl_clip_txx_
//:
// \file
// \author fsm

#include "vgl_clip.h"
#include <vcl_cstdlib.h> // for vcl_malloc() and vcl_free()
#include <vcl_cstdio.h> // for vcl_fprintf()
#include <vcl_algorithm.h> // for swap

template <class T>
bool vgl_clip_lineseg_to_line(T &x1, T &y1,
                              T &x2, T &y2,
                              T a,T b,T c)
{
  T f1 = a*x1+b*y1+c;
  T f2 = a*x2+b*y2+c;
  if (f1<0) {
    if (f2<0) // both out
      return false;
    // 1 out, 2 in
    x1 = (f2*x1 - f1*x2)/(f2-f1);
    y1 = (f2*y1 - f1*y2)/(f2-f1);
    return true;
  }
  else {
    if (f2<0)  // 1 in, 2 out
    {
      x2 = (f2*x1 - f1*x2)/(f2-f1);
      y2 = (f2*y1 - f1*y2)/(f2-f1);
    }
    // both in
    return true;
  }
}

template <class T>
bool vgl_clip_line_to_box(T a, T b, T c, // line coefficients.
                          T x1, T y1,    // bounding
                          T x2, T y2,    // box.
                          T &bx, T &by,  // start and
                          T &ex, T &ey)  // end points.
{
  if (x1>x2) vcl_swap(x1,x2);
  if (y1>y2) vcl_swap(y1,y2);
  // now x1 <= x2 and y1 <= y2

  if (a == 0 && b == 0) return false; // then ax+by+c=0 is the line at infinity

  bool b_set = false, // has the point (bx,by) been set to a valid point?
       e_set = false; // has the point (ex,ey) been set to a valid point?

  if (a != 0) // line is not horizontal
  {
    // Intersection point with the line y=y1:
    by = y1; bx = -(b*y1+c)/a;
    // Intersection point with the line y=y2:
    ey = y2; ex = -(b*y2+c)/a;

    b_set =  bx >= x1 && bx <= x2; // does this intersection point
    e_set =  ex >= x1 && ex <= x2; // lie on the bounding box?
  }

  if (b_set && e_set) return true;
  if (b_set) { vcl_swap(bx,ex); vcl_swap(by,ey); vcl_swap(b_set,e_set); }
  // now b_set is false

  if (b != 0) // line is not vertical
  {
    // Intersection point with the line x=x1:
    bx = x1; by = -(a*x1+c)/b;
    b_set =  by >= y1 && by <= y2;
    if (b_set && e_set) return true;
    if (b_set) { vcl_swap(bx,ex); vcl_swap(by,ey); e_set=true; }

    // Intersection point with the line x=x2:
    bx = x2; by = -(a*x2+c)/b;
    b_set =  by >= y1 && ey <= y2;
  }

  return b_set && e_set;
}


extern "C" {
#include "internals/gpc.h"
}

#define MALLOC(p, T, c, s) { if ((c) > 0) { \
                            p= (T*)vcl_malloc(c * sizeof(T)); if (!(p)) { \
                            vcl_fprintf(stderr, "vgl: gpc malloc failure: %s\n", s); \
                            vcl_exit(0);}} else p=NULL; }

#define FREE(p)            { if (p) { vcl_free(p); (p)= NULL; } }

namespace {
  //: Creates a gpc polygon from a vgl_polygon.
  // The caller is responsible for freeing the gpc_polygon.
  template <class T>
  gpc_polygon
  vgl_to_gpc( const vgl_polygon<T>& vgl_poly )
  {
    gpc_polygon gpc_poly;
    gpc_poly.num_contours = vgl_poly.num_sheets();
    MALLOC( gpc_poly.hole, int, gpc_poly.num_contours, "allocating hole array" );
    MALLOC( gpc_poly.contour, gpc_vertex_list, gpc_poly.num_contours, "allocating contour array" );
    for ( int s = 0; s < gpc_poly.num_contours; ++s ) {
      gpc_poly.hole[s] = 0;
      gpc_poly.contour[s].num_vertices = vgl_poly[s].size();
      MALLOC( gpc_poly.contour[s].vertex, gpc_vertex, vgl_poly[s].size(), "allocating vertex list" );
      for ( unsigned int p = 0; p < vgl_poly[s].size(); ++p ) {
        gpc_poly.contour[s].vertex[p].x = vgl_poly[s][p].x();
        gpc_poly.contour[s].vertex[p].y = vgl_poly[s][p].y();
      }
    }

    return gpc_poly;
  }

  //: Adds a gpc_polygon to a given vgl_polygon.
  template <class T>
  void
  add_gpc_to_vgl( vgl_polygon<T>& vgl_poly, const gpc_polygon& gpc_poly )
  {
    for ( int c=0; c < gpc_poly.num_contours; ++c ) {
      vgl_poly.new_sheet();
      for ( int p=0; p < gpc_poly.contour[c].num_vertices; ++p ) {
        vgl_poly.push_back( T(gpc_poly.contour[c].vertex[p].x),
                            T(gpc_poly.contour[c].vertex[p].y) );
      }
    }
  }
}

template <class T>
vgl_polygon<T>
vgl_clip(vgl_polygon<T> const& poly1, vgl_polygon<T> const& poly2, vgl_clip_type op )
{
  // Check for the null case
  if ( poly1.num_sheets() == 0 )
    return poly2;
  if ( poly2.num_sheets() == 0 )
    return poly1;

  gpc_polygon p1 = vgl_to_gpc( poly1 );
  gpc_polygon p2 = vgl_to_gpc( poly2 );
  gpc_polygon p3;
  vgl_polygon<T> result;

  gpc_op g_op = GPC_INT;
  switch( op ) {
    case vgl_clip_type_intersect:    g_op = GPC_INT;   break;
    case vgl_clip_type_difference:   g_op = GPC_DIFF;  break;
    case vgl_clip_type_union:        g_op = GPC_UNION; break;
    case vgl_clip_type_xor:          g_op = GPC_XOR;   break;
  }

  gpc_polygon_clip( g_op, &p1, &p2, &p3 );
  add_gpc_to_vgl( result, p3 );

  gpc_free_polygon( &p1 );
  gpc_free_polygon( &p2 );
  gpc_free_polygon( &p3 );

  return result;
}

#undef VGL_CLIP_INSTANTIATE
#define VGL_CLIP_INSTANTIATE(T) \
template vgl_polygon<T > vgl_clip(vgl_polygon<T >const&,vgl_polygon<T >const&,vgl_clip_type); \
template bool vgl_clip_lineseg_to_line(T&,T&,T&,T&,T,T,T); \
template bool vgl_clip_line_to_box(T,T,T,T,T,T,T,T&,T&,T&,T&); \
template vgl_line_segment_2d<T > vgl_clip_line_to_box(vgl_line_2d<T >const&,vgl_box_2d<T >const&)

#endif // vgl_clip_txx_
