// This is vxl/vgl/vgl_clip.cxx

//:
// \file
// \author fsm@robots.ox.ac.uk

#ifdef __GNUC__
#pragma implementation
#endif
#include "vgl_clip.h"
#include <vcl_cmath.h> // for vcl_abs(double) and vcl_sqrt()
#include <vcl_algorithm.h> // for swap

bool vgl_clip_lineseg_to_line(double &x1, double &y1,
                              double &x2, double &y2,
                              double a,double b,double c)
{
  double f1 = a*x1+b*y1+c;
  double f2 = a*x2+b*y2+c;
  if (f1<0) {
    if (f2<0)
      return false; // both out
    // 1 out, 2 in
    x1 = (f2*x1 - f1*x2)/(f2-f1);
    y1 = (f2*y1 - f1*y2)/(f2-f1);
    return true;
  }
  else {
    if (f2>0)
      return true;  // both in
    // 1 in, 2 out
    x2 = (f2*x1 - f1*x2)/(f2-f1);
    y2 = (f2*y1 - f1*y2)/(f2-f1);
    return true;
  }
}

bool vgl_clip_line_to_box(double a, double b, double c, // coefficients.
                          double x1,double y1,  // bounding
                          double x2,double y2,  // box.
                          double &bx, double &by,  // start and
                          double &ex, double &ey)  // end points.
{
  if (x1>x2)
    vcl_swap(x1,x2);
  if (y1>y2)
    vcl_swap(y1,y2);

  // I guess this is not really necessary.
  double r = vcl_sqrt(a*a + b*b);
  if (r == 0)
    return false;
  a /= r;
  b /= r;
  c /= r;

  if (vcl_abs(a) > vcl_abs(b)) {
    // more vertical than horizontal
    bx = -(b*y1+c)/a;
    by = y1;
    ex = -(b*y2+c)/a;
    ey = y2;

    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(+1),double(0),-x1))
      return false;
    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(-1),double(0), x2))
      return false;
  }
  else {
    // more horizontal than vertical
    bx = x1;
    by = -(a*x1+c)/b;
    ex = x2;
    ey = -(a*x2+c)/b;

    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(0),double(+1),-y1))
      return false;
    if (!vgl_clip_lineseg_to_line(bx,by, ex,ey, double(0),double(-1), y2))
      return false;
  }

  return true;
}


extern "C" {
#include "internals/gpc.h"
}

#define MALLOC(p, T, c, s) {if ((c) > 0) { \
                            p= (T*)vcl_malloc(c * sizeof(T)); if (!(p)) { \
                            vcl_fprintf(stderr, "vgl: gpc malloc failure: %s\n", s); \
                            vcl_exit(0);}} else p= NULL;}

#define FREE(p)            {if (p) {vcl_free(p); (p)= NULL;}}

namespace {
  //: Creates a gpc polygon from a vgl_polygon.
  // The caller is responsible for freeing the gpc_polygon.
  gpc_polygon
  vgl_to_gpc( const vgl_polygon& vgl_poly )
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
  void
  add_gpc_to_vgl( vgl_polygon& vgl_poly, const gpc_polygon& gpc_poly )
  {
    for ( int c=0; c < gpc_poly.num_contours; ++c ) {
      vgl_poly.new_sheet();
      for ( int p=0; p < gpc_poly.contour[c].num_vertices; ++p ) {
        vgl_poly.push_back( gpc_poly.contour[c].vertex[p].x, gpc_poly.contour[c].vertex[p].y );
      }
    }
  }
}

vgl_polygon
vgl_clip( const vgl_polygon& poly1, const vgl_polygon& poly2, vgl_clip_type op )
{
  // Check for the null case
  if ( poly1.num_sheets() == 0 )
    return poly2;
  if ( poly2.num_sheets() == 0 )
    return poly1;

  gpc_polygon p1 = vgl_to_gpc( poly1 );
  gpc_polygon p2 = vgl_to_gpc( poly2 );
  gpc_polygon p3;
  vgl_polygon result;

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
