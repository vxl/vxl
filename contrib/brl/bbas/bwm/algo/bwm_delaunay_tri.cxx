#include <complex>
#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "bwm_delaunay_tri.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

double bwm_delaunay_tri::EPSILON = 0.000001;

//:  Return true if a point p is inside the circumcircle made up of the points p1(x1,y1), p2(x2,y2), p3(x3,y3)
//   The circumcircle centre is returned in c(xc,yc) and the radius r
//   Note : A point on the edge is inside the circumcircle
bool bwm_delaunay_tri::CircumCircle(vgl_point_2d<double> p,
                                    vgl_point_2d<double> p1,
                                    vgl_point_2d<double> p2,
                                    vgl_point_2d<double> p3,
                                    vgl_point_2d<double>& c, double &r)
{
  double m1, m2, mx1, mx2, my1, my2;
  double dx, dy, rsqr, drsqr;
  double xc, yc;

  // Check for coincident points
  if (std::abs(p1.y()-p2.y()) < EPSILON && std::abs(p2.y()-p3.y()) < EPSILON)
    return false;

  if (std::abs(p2.y()-p1.y()) < EPSILON) {
    m2 = - (p3.x() - p2.x()) / (p3.y() - p2.y());
    mx2 = (p2.x() + p3.x()) / 2.0;
    my2 = (p2.y() + p3.y()) / 2.0;
    xc = (p2.x() + p1.x()) / 2.0;
    yc = m2 * (xc - mx2) + my2;
    c = vgl_point_2d<double> (xc,yc);
  }
  else if (std::abs(p3.y() - p2.y()) < EPSILON) {
    m1 = - (p2.x() - p1.x()) / (p2.y() - p1.y());
    mx1 = (p1.x() + p2.x()) / 2.0;
    my1 = (p1.y() + p2.y()) / 2.0;
    xc = (p3.x() + p2.x()) / 2.0;
    yc = m1 * (xc - mx1) + my1;
    c = vgl_point_2d<double> (xc,yc);
  }
  else {
    m1 = - (p2.x() - p1.x()) / (p2.y() - p1.y());
    m2 = - (p3.x() - p2.x()) / (p3.y() - p2.y());
    mx1 = (p1.x() + p2.x()) / 2.0;
    mx2 = (p2.x() + p3.x()) / 2.0;
    my1 = (p1.y() + p2.y()) / 2.0;
    my2 = (p2.y() + p3.y()) / 2.0;
    xc = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
    yc = m1 * (xc - mx1) + my1;
    c = vgl_point_2d<double> (xc,yc);
  }

  dx = p2.x() - xc;
  dy = p2.y() - yc;
  rsqr = dx * dx + dy * dy;
  r = std::sqrt(rsqr);
  dx = p.x() - xc;
  dy = p.y() - yc;
  drsqr = dx * dx + dy * dy;
  return drsqr <= rsqr;
}

//:  Triangulation subroutine
//   Takes as input NV vertices in array pxyz
//   Returned is a list of ntri triangular faces in the array v
//   These triangles are arranged in a consistent clockwise order.
//   The triangle array 'v' should be malloced to 3 * nv
//   The vertex array pxyz must be big enough to hold 3 more points
//   The vertex array must be sorted in increasing x values say
//
//   qsort(p,nv,sizeof(XYZ),XYZCompare);
///////////////////////////////////////////////////////////////////////////////

int bwm_delaunay_tri::triangulate(std::vector<vgl_point_3d<double> >& pxyz,
                                  std::vector<vgl_point_3d<int> >& v,
                                  int &ntri)
{
  std::vector<bool> complete;
  std::vector<vgl_point_2d<int> > edges;
  std::vector<vgl_point_2d<int> > p_EdgeTemp;
  unsigned int nedge = 0;
  unsigned int trimax, emax = 200;
  bool inside;
  vgl_point_2d<double> p, p1, p2, p3, c;
  double r;
  double xmin, xmax, ymin, ymax, xmid, ymid;
  double dx, dy, dmax;


  // sort the vertices on X
  std::qsort(&pxyz[0], pxyz.size(), sizeof pxyz[0], &bwm_delaunay_tri::XYZCompare);

  //Allocate memory for the completeness list, flag for each triangle */
  trimax = 4 * pxyz.size();
  emax=trimax;
  complete.resize(trimax);

  //Allocate memory for the edge list */
  edges.resize(emax);

  //  Find the maximum and minimum vertex bounds.
  //  This is to allow calculation of the bounding triangle
  xmin = pxyz[0].x();
  ymin = pxyz[0].y();
  xmax = xmin;
  ymax = ymin;
  for (unsigned i = 1; i < pxyz.size(); i++) {
    if (pxyz[i].x() < xmin) xmin = pxyz[i].x();
    if (pxyz[i].x() > xmax) xmax = pxyz[i].x();
    if (pxyz[i].y() < ymin) ymin = pxyz[i].y();
    if (pxyz[i].y() > ymax) ymax = pxyz[i].y();
  }
  dx = xmax - xmin;
  dy = ymax - ymin;
  dmax = (dx > dy) ? dx : dy;
  xmid = (xmax + xmin) / 2.0;
  ymid = (ymax + ymin) / 2.0;

  // Set up the supertriangle
  // This is a triangle which encompasses all the sample points.
  // The supertriangle coordinates are added to the end of the
  // vertex list. The supertriangle is the first triangle in
  // the triangle list.
  unsigned nv = pxyz.size();
  v.resize(3*nv);
  pxyz.push_back(vgl_point_3d<double>(xmid - 20 * dmax, ymid - dmax,0));
  pxyz.push_back(vgl_point_3d<double>(xmid, ymid + 20 * dmax,0));
  pxyz.push_back(vgl_point_3d<double>(xmid + 20 * dmax, ymid - dmax,0));
  v[0].set(nv, nv+1, nv+2);
  complete[0] = false;
  ntri = 1;

  // Include each point one at a time into the existing mesh
  for (unsigned i = 0; i < nv; i++)
  {
    p.set(pxyz[i].x(), pxyz[i].y());
    nedge = 0;
    // Set up the edge buffer.
    // If the point (xp,yp) lies inside the circumcircle then the
    // three edges of that triangle are added to the edge buffer
    // and that triangle is removed.
    for (int j = 0; j < ntri; ++j)
    {
      if (complete[j])
        continue;
      p1.set(pxyz[v[j].x()].x(), pxyz[v[j].x()].y());
      p2.set(pxyz[v[j].y()].x(), pxyz[v[j].y()].y());
      p3.set(pxyz[v[j].z()].x(), pxyz[v[j].z()].y());
      inside = CircumCircle(p, p1, p2, p3, c, r);
      if (c.x() + r < p.x())
        complete[j] = true;
        if (inside) {
          // Check that we haven't exceeded the edge list size
          if (nedge + 3 >= emax) {
            emax += 100;
            p_EdgeTemp.resize(emax);
            for (unsigned int i = 0; i < nv; ++i) {
              p_EdgeTemp[i] = edges[i];
            }
            edges.clear();
            edges = p_EdgeTemp;
          }
          edges[nedge+0].set(v[j].x(), v[j].y());
          edges[nedge+1].set(v[j].y(), v[j].z());
          edges[nedge+2].set(v[j].z(), v[j].x());
          nedge += 3;
          v[j] = v[ntri-1];
          complete[j] = complete[ntri-1];
          --ntri;
          --j;
        }
    }
    // Tag multiple edges
    // Note: if all triangles are specified anticlockwise then all
    // interior edges are opposite pointing in direction.
    for (unsigned j = 0; j+1 < nedge; ++j) {
      for (unsigned k = j + 1; k < nedge; ++k) {
       if ((edges[j].x() == edges[k].y()) && (edges[j].y() == edges[k].x())) {
          edges[j].set(-1, -1);
          edges[k].set(-1, -1);
        }
         // Shouldn't need the following, see note above
        if ((edges[j].x() == edges[k].x()) && (edges[j].y() == edges[k].y())) {
         edges[j].set(-1, -1);
         edges[k].set(-1, -1);
        }
      }
    }
    // Form new triangles for the current point
    // Skipping over any tagged edges.
    // All edges are arranged in clockwise order.
    for (unsigned j = 0; j < nedge; j++) {
     if (edges[j].x() < 0 || edges[j].y() < 0)
       continue;
     v[ntri].set(edges[j].x(), edges[j].y(), i);
      complete[ntri] = false;
     ntri++;
    }
  }
  // Remove triangles with supertriangle vertices
  // These are triangles which have a vertex number greater than nv
  for (int i = 0; i < ntri; ++i) {
    if (v[i].x() >= (int)nv || v[i].y() >= (int)nv || v[i].z() >= (int)nv) {
      v[i] = v[ntri-1];
      --ntri;
      --i;
    }
  }
  v.erase(v.begin()+ntri, v.end());
  return 0;
}

//: compares the X value of a 3D point
int bwm_delaunay_tri::XYZCompare(const void *v1, const void *v2)
{
  vgl_point_3d<double> *p1, *p2;

  p1 = (vgl_point_3d<double>*)v1;
  p2 = (vgl_point_3d<double>*)v2;
  if (p1->x() < p2->x())
    return -1;
  else if (p1->x() > p2->x())
    return 1;
  else
    return 0;
}
