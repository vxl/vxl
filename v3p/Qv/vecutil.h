// This may look like C code, but it is really -*- C++ -*-

//<copyright>
//
// Copyright (c) 1993,94,95,96
// Institute for Information Processing and Computer Supported New Media (IICM),
// Graz University of Technology, Austria.
//
// This file is part of VRweb.
//
// VRweb is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// VRweb is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with VRweb; see the file LICENCE. If not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
//</copyright>

//<file>
//
// .NAME    vecutil.h - utilities for 3D vectors
// .INCLUDE Qv/vecutil.h
// .FILE    vecutil.cxx
//
// Created:     15 Mar 93   Michael Pichler
// Changed:     19 Mar 96   Michael Pichler
//
//</file>

#ifndef harmony_scene_vecutil_h
#define harmony_scene_vecutil_h

#include "vectors.h"

#include <vcl_cfloat.h> // for MAXFLOAT
// MAXFLOAT is now guaranteed to be defined in <vcl_cfloat.h>
#include <vcl_ostream.h>

#ifdef VCL_VC
# define ARRAYconst  /* gorasche: causes problems with MSC */
#else
# ifdef SUN5
#  define ARRAYconst  /**/
# else
#  define ARRAYconst  const
# endif
#endif


/*** vector utilities ***/

// computing normal vector
// n is computed as the (unnormalized) outward normal vector
// of the triangle (p, q, r), given in counterclockwise order;
// if (p, q, r) are collinear, n will be zero

inline void computeNormal (const point3D& p, const point3D& q, const point3D& r,  // 3 points
                           vector3D& n)  // normal vector
{
  vector3D a, b;
  sub3D (r, q, a);
  sub3D (p, q, b);
  crp3D (a, b, n);  // n = (r - q) x (p - q)
}


// pointinsidecube
// returns 1 if point A is inside the (axis aligned) cube with vertices (min, max)
// (min.x < max.x etc. required); returns 0 otherwise

inline int pointinsidecube (const point3D& A, const point3D& min, const point3D& max)
{
  return (
    min.x <= A.x && A.x <= max.x &&
    min.y <= A.y && A.y <= max.y &&
    min.z <= A.z && A.z <= max.z
  );
}


// rayhitscube
// tests whether ray A + b.t hits the (axis aligned) cube with vertices (min, max)
// (min.x < max.x etc. required) at a value of t, t > tnear and t < tmin, from outside
// if so, t is returned (but tmin is not changed), 0.0 otherwise
// when normal is non NULL, the normal vector in the hit point is returned

float rayhitscube (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& min, const point3D& max, vector3D* normal = 0, int inside = 0
);


// rayhitsline
// tests wheter ray A + b.t comes near the line P + k.v (dist^2 <= eps2)
// if so, t is returned (but tmin not changed), 0.0 otherwise

float rayhitsline (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& P, const vector3D& v, float eps2
);


// rayhitspoint
// tests whether ray A + b.t comes near point P (distance^2 <= eps2)
// if so, t is returned (but tmin not changed), 0.0 otherwise

float rayhitspoint (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& P, float eps2
);


/*** matrix utilities ***/

// extern const float identity_matrix [4][4];
// obsolete. use ge3dPushIdentity

void copymatrix (ARRAYconst matrix4D src, matrix4D dest);  // copy a matrix

void multiplyPoint3DMatrix4D (const point3D pt, ARRAYconst matrix4D mat, point3D& prod);

void multiplyVector3DMatrix4D (const vector3D vec, ARRAYconst matrix4D mat, vector3D& prod);

void transposematrix (matrix4D mat);  // replaces mat with its transposed form

// make 3d transformation matrix from homogenous trf. mat.
// (discard translation and perspective transformations)
void mat4Dto3D (matrix4D mat);

// matrix inversion
void invertmatrix (matrix4D mat);  // replaces mat with its inverse


/*** rotation utilities ***/

// sphere coordinates

struct spherepoint
{ float r,                      // radius
        phi,                    // longitude (x to y)
        theta;                  // latitude (from z)
};


// conversion cartesian - spherepoint

void cartes2sphere (const point3D&, spherepoint&);

void sphere2cartes (const spherepoint&, point3D&);


// rotation about arbitrary axis (through origin)

// void rotatearoundaxis (const vector3D&, float);
// anachronism since ge3dRotate


// conversion quaternion - rotation
// quaternions and rotation axis are given normalized

void rotation2quaternion (const vector3D& axis, float angle, vector3D& v, float& s);
void quaternion2rotation (const vector3D& v, float s, vector3D& axis, float& angle);

// multiply (concatenate) 2 normalized quaternions
void multiplyQuaternions (
  const vector3D& v0, float s0, const vector3D& v1, float s1,
  vector3D& v, float& s);

// multiply (concatenate) 2 rotations (axis must be normalized) via quaternions
void multiplyRotations (
  const vector3D& axis0, float angle0, const vector3D& axis1, float angle1,
  vector3D& axis, float& angle);


/*** bounding boxes ***/

inline void emptyBoundingbox (point3D& min, point3D& max)
{ // set an empty boundingbox (such that any point extends it)
  min.x = min.y = min.z = MAXFLOAT;
  max.x = max.y = max.z = -MAXFLOAT;
}


// object to world bounding box

void computeBoundingbox (const point3D& omin, const point3D& omax, point3D& wmin, point3D& wmax);
// takes an object bounding box, and transforms it to world coordinates
// (by applying the current transformation matrix; all coordinates ordered)


inline void extendBoundingbox (const point3D& pmin, const point3D& pmax, point3D& min, point3D& max)
{ // extend bounding box if pmin/pmax falls outside it
  // purpose: extend bounding box (min, max) by a point pmin==pmax
  // or merge another box (pmin, pmax) into it
  if (pmin.x < min.x)  min.x = pmin.x;
  if (pmin.y < min.y)  min.y = pmin.y;
  if (pmin.z < min.z)  min.z = pmin.z;
  if (pmax.x > max.x)  max.x = pmax.x;
  if (pmax.y > max.y)  max.y = pmax.y;
  if (pmax.z > max.z)  max.z = pmax.z;
}


/*** output (printing) operators ***/

vcl_ostream& operator << (vcl_ostream&, const point3D&);
vcl_ostream& operator << (vcl_ostream&, const point2D&);
vcl_ostream& operator << (vcl_ostream&, const spherepoint&);
vcl_ostream& operator << (vcl_ostream&, ARRAYconst matrix4D);

#endif // harmony_scene_vecutil_h
