// This is core/vgl/vgl_clip.hxx
#ifndef vgl_clip_hxx_
#define vgl_clip_hxx_
//:
// \file
// \author fsm

#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <limits>
#include <cmath>
#include "vgl_clip.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  if (x1>x2) std::swap(x1,x2);
  if (y1>y2) std::swap(y1,y2);
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
  if (b_set) { std::swap(bx,ex); std::swap(by,ey); std::swap(b_set,e_set); }
  // now b_set is false

  if (b != 0) // line is not vertical
  {
    // Intersection point with the line x=x1:
    bx = x1; by = -(a*x1+c)/b;
    b_set =  by >= y1 && by <= y2;
    if (b_set && e_set) return true;
    if (b_set) { std::swap(bx,ex); std::swap(by,ey); e_set=true; }

    // Intersection point with the line x=x2:
    bx = x2; by = -(a*x2+c)/b;
    b_set =  by >= y1 && ey <= y2;
  }

  return b_set && e_set;
}


// This license is very restrictive, prefer Angus Johnson's more liberal Clipper library.
#ifdef BUILD_NONCOMMERCIAL

extern "C" {
#include "internals/gpc.h"
}

#define MALLOC(p, T, c, s) { if ((c) > 0) { \
                            p= (T*)std::malloc(c * sizeof(T)); if (!(p)) { \
                            std::fprintf(stderr, "vgl: gpc malloc failure: %s\n", s); \
                            std::exit(0);}} else p=NULL; }

#define FREE(p)            { if (p) { std::free(p); (p)= NULL; } }

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

#elif HAS_CLIPPER

#include <clipper.hxx>

namespace {
  //: Creates a Clipper polygon from a vgl_polygon.
  template <class T>
  ClipperLib::Paths
  vgl_to_clipper( const vgl_polygon<T>& vgl_poly, double scale )
  {
    ClipperLib::Paths clipper_poly;
    for ( size_t s = 0; s < vgl_poly.num_sheets(); ++s ) {
      ClipperLib::Path path;
      for ( size_t p = 0; p < vgl_poly[s].size(); ++p ) {
        ClipperLib::IntPoint pt((ClipperLib::cInt)((double)vgl_poly[s][p].x()*scale),
                                (ClipperLib::cInt)((double)vgl_poly[s][p].y()*scale));
        path.push_back(pt);
      }
      clipper_poly.push_back(path);
    }

    return clipper_poly;
  }

  //: Adds a Clipper polygon to a given vgl_polygon.
  template <class T>
  void
  add_clipper_to_vgl( vgl_polygon<T>& vgl_poly, const ClipperLib::Paths& clipper_poly, double scale )
  {
    for (const auto & c : clipper_poly) {
      vgl_poly.new_sheet();
      for ( size_t p=0; p < c.size(); ++p ) {
        vgl_poly.push_back( T((double)c[p].X/scale),
                            T((double)c[p].Y/scale) );
      }
    }
  }
}

template <class T>
void
bounds(vgl_polygon<T> vgl_poly, T& min_x, T& max_x, T& min_y, T& max_y)
{
  for (size_t s=0; s < vgl_poly.num_sheets(); ++s) {
    for (size_t p=0; p < vgl_poly[s].size(); ++p) {
      if(s==0 && p==0) { // not the most ideal way to initilize this...
        min_x = max_x = vgl_poly[0][0].x();
        min_y = max_y = vgl_poly[0][0].y();
      }

      min_x = std::min(vgl_poly[s][p].x(), min_x);
      min_y = std::min(vgl_poly[s][p].y(), min_y);
      max_x = std::max(vgl_poly[s][p].x(), max_x);
      max_y = std::max(vgl_poly[s][p].y(), max_y);
    }
  }
}

#endif

template <class T>
vgl_polygon<T>
vgl_clip(vgl_polygon<T> const& poly1, vgl_polygon<T> const& poly2, vgl_clip_type op, int *p_retval)
{
  // Check for the null case
  if ( poly1.num_sheets() == 0 ) {
    *p_retval = 1;
    switch ( op )
    {
      case vgl_clip_type_intersect:    return poly1;
      case vgl_clip_type_difference:   return poly1;
      case vgl_clip_type_union:        return poly2;
      case vgl_clip_type_xor:          return poly2;
      default:                         *p_retval = -1; return vgl_polygon<T>(); // this should not happen...
    }
  }
  if ( poly2.num_sheets() == 0 ) {
    *p_retval = 1;
    switch ( op )
    {
      case vgl_clip_type_intersect:    return poly2;
      case vgl_clip_type_difference:   return poly1;
      case vgl_clip_type_union:        return poly1;
      case vgl_clip_type_xor:          return poly1;
      default:                         *p_retval = -1; return vgl_polygon<T>(); // this should not happen...
    }
  }

  vgl_polygon<T> result;

#ifdef BUILD_NONCOMMERCIAL
  gpc_polygon p1 = vgl_to_gpc( poly1 );
  gpc_polygon p2 = vgl_to_gpc( poly2 );
  gpc_polygon p3;

  gpc_op g_op = GPC_INT;
  switch ( op )
  {
    case vgl_clip_type_intersect:    g_op = GPC_INT;   break;
    case vgl_clip_type_difference:   g_op = GPC_DIFF;  break;
    case vgl_clip_type_union:        g_op = GPC_UNION; break;
    case vgl_clip_type_xor:          g_op = GPC_XOR;   break;
    default:                         break;
  }

  int retval = gpc_polygon_clip( g_op, &p1, &p2, &p3 );
  *p_retval = retval;

  if (retval == 0) {
    gpc_free_polygon( &p1 );
    gpc_free_polygon( &p2 );
    return result;
  }
  add_gpc_to_vgl( result, p3 );

  gpc_free_polygon( &p1 );
  gpc_free_polygon( &p2 );
  gpc_free_polygon( &p3 );

#elif HAS_CLIPPER
  ClipperLib::Clipper clpr;
  // Clipper operates in fixed point space (because it is more robust), so we need
  // to compute a scale factor to preserve precision.
  // per Angus Johnson, "if any coordinate value exceeds +/-3.0e+9, large integer
  // math slows clipping by about 10%"
  int halfSignificantDigits = std::numeric_limits<ClipperLib::cInt>::digits10/2;

  T min_x, max_x, min_y, max_y;
  bounds( poly1, min_x, max_x, min_y, max_y);
  max_x = std::max(max_x, std::abs(min_x));
  max_y = std::max(max_y, std::abs(min_y));
  T max1 = std::max(max_x, max_y);

  bounds( poly2, min_x, max_x, min_y, max_y);
  max_x = std::max(max_x, std::abs(min_x));
  max_y = std::max(max_y, std::abs(min_y));
  T max2 = std::max(max_x, max_y);

  T max = std::max(max1, max2);
  double scale = std::pow(10.0, halfSignificantDigits) / max;


  ClipperLib::Paths p1 = vgl_to_clipper( poly1, scale );
  ClipperLib::Paths p2 = vgl_to_clipper( poly2, scale );
  ClipperLib::Paths p3;

  ClipperLib::ClipType g_op = ClipperLib::ctIntersection;
  switch ( op )
  {
    case vgl_clip_type_intersect:    g_op = ClipperLib::ctIntersection; break;
    case vgl_clip_type_difference:   g_op = ClipperLib::ctDifference;   break;
    case vgl_clip_type_union:        g_op = ClipperLib::ctUnion; break;
    case vgl_clip_type_xor:          g_op = ClipperLib::ctXor;   break;
    default:                         break;
  }


  clpr.AddPaths(p1, ClipperLib::ptSubject, true);
  clpr.AddPaths(p2, ClipperLib::ptClip, true);
  int retval = clpr.Execute(g_op, p3, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);
  *p_retval = retval;

  add_clipper_to_vgl( result, p3, scale );

#else
  *p_retval = -1;
  std::fprintf(stdout,"WARNING: GPC is only free for non-commercial use -- assuming disjoint polygons.\n");
  std::fprintf(stderr,"WARNING: GPC is only free for non-commercial use -- assuming disjoint polygons.\n");
  switch ( op )
  {
    default:
    case vgl_clip_type_intersect:    result = vgl_polygon<T>(); break; // empty
    case vgl_clip_type_difference:   result = poly1; break;
    case vgl_clip_type_union:
    case vgl_clip_type_xor:
      result = poly1;
      for (unsigned int i=0; i<poly2.num_sheets(); ++i)
        result.push_back(poly2[i]);
      break;
  }

#endif

  return result;
}

template <class T>
vgl_polygon<T>
vgl_clip(vgl_polygon<T> const& poly1, vgl_polygon<T> const& poly2, vgl_clip_type op )
{
  int retval;
  return vgl_clip(poly1, poly2, op, &retval);
}

#undef VGL_CLIP_INSTANTIATE
#define VGL_CLIP_INSTANTIATE(T) \
template vgl_polygon<T > vgl_clip(vgl_polygon<T >const&,vgl_polygon<T >const&,vgl_clip_type); \
template vgl_polygon<T > vgl_clip(vgl_polygon<T >const&,vgl_polygon<T >const&,vgl_clip_type,int *); \
template bool vgl_clip_lineseg_to_line(T&,T&,T&,T&,T,T,T); \
template bool vgl_clip_line_to_box(T,T,T,T,T,T,T,T&,T&,T&,T&); \
template vgl_line_segment_2d<T > vgl_clip_line_to_box(vgl_line_2d<T >const&,vgl_box_2d<T >const&)

#endif // vgl_clip_hxx_
