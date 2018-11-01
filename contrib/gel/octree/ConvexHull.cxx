// This is gel/octree/ConvexHull.cxx
//
// Class: ConvexHull
// Author: Geoffrey Cross, Oxford RRG
// Created: 24 May 99
// Modifications:
//   990524 Geoff Initial version.
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <cstdlib>
#include "ConvexHull.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define CMPM(c,A,B) \
        v = (*(const double*const*)A)[c] - (*(const double*const*)B)[c];\
        if (v>0) return 1;\
        if (v<0) return -1;

//typedef int (ConvexHull::*FPTR)(const void *, const void *);
typedef int (*FFPTR)(const void *, const void *);

// Default ctor
ConvexHull::ConvexHull( int npoints)
  : numberofpoints( npoints),
    hullsize( -1)
{
}

void ConvexHull::set_point( int n, double x, double y)
{
  points[n][0]= x;
  points[n][1]= y;
  P[n]= points[n];
}


int ConvexHull::cmpl(const void *a, const void *b)
{
  double v;
  CMPM(0,a,b);
  CMPM(1,b,a);
  return 0;
}

int ConvexHull::cmph(const void *a, const void *b)
{
  return cmpl(b,a);
}


int ConvexHull::make_chain(double** V, int n, int (*cmp)(const void*, const void*))
{
  int i, j, s = 1;
  double* t;

  std::qsort(V, n, sizeof(double*), cmp);
  for (i=2; i<n; i++) {
    for (j=s; j>=1 && ccw(V, i, j, j-1); j--){}
    s = j+1;
    t = V[s]; V[s] = V[i]; V[i] = t;
  }
  return s;
}

int ConvexHull::ccw(double **P, int i, int j, int k) {
  double   a = P[i][0] - P[j][0],
           b = P[i][1] - P[j][1],
           c = P[k][0] - P[j][0],
           d = P[k][1] - P[j][1];
  return a*d - b*c <= 0;     /* true if points i, j, k counterclockwise */
}


int ConvexHull::ch2d()
{
  int u = make_chain(P, numberofpoints, cmpl);         /* make lower hull */
  if (!numberofpoints) return 0;
  P[numberofpoints] = P[0];
  return u+make_chain(P+u, numberofpoints-u+1, cmph);  /* make upper hull */
}

void ConvexHull::compute()
{
  hullsize= ch2d();
}
