//:
// \file
// \brief     utilities for 3D vectors
//
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
//
// \author
// Created:     15 Mar 93   Michael Pichler
// Changed:     30 Jan 96   Michael Pichler
//

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include "vecutil.h"

// became obsolete by ge3dPushIdentity
// const float identity_matrix [4][4] =
// { { 1.0, 0.0, 0.0, 0.0 },
//   { 0.0, 1.0, 0.0, 0.0 },
//   { 0.0, 0.0, 1.0, 0.0 },
//   { 0.0, 0.0, 0.0, 1.0 }
// };


// rayhitscube
// tests wheter ray A + b.t hits the (axis aligned) cube with vertices (min, max)
// (min.x < max.x etc. required) at a value of t, t > tnear and t < tmin, from outside;
// if so, t is returned (but tmin is not changed), 0.0 otherwise
// when normal is non NULL, the normal vector in the hit point is returned
// flag inside tells wheter to pick the outer (default) or inner side of the cube
// (this test requires only 3 divisions, up to 6 multiplications and some additions/comparisons)

float rayhitscube (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& min, const point3D& max, vector3D* normal, int inside
)
{
  float thit, hpx, hpy, hpz;  // hit time and hit point

  // inside == 0: pick at min for "increasing" direction (normal -1)
  // inside != 0: pick at max for "increasing" direction (normal -1)

  if (b.x)
  { thit = (((b.x > 0) ^ inside ? min.x : max.x) - A.x) / b.x;
    if (tnear < thit && thit < tmin)
    { // check if hit point hp = A + thit * b  lies within face
      hpy = A.y + thit * b.y;
      hpz = A.z + thit * b.z;
      if (min.y <= hpy && hpy <= max.y && min.z <= hpz && hpz <= max.z)
      { if (normal)
          init3D (*normal, b.x > 0 ? -1 : 1, 0, 0);
        return thit;
      }
    }
  }

  if (b.y)
  { thit = (((b.y > 0) ^ inside ? min.y : max.y) - A.y) / b.y;
    if (tnear < thit && thit < tmin)
    { // check if hit point hp = A + thit * b  lies within face
      hpx = A.x + thit * b.x;
      hpz = A.z + thit * b.z;
      if (min.x <= hpx && hpx <= max.x && min.z <= hpz && hpz <= max.z)
      { if (normal)
          init3D (*normal, 0, b.y > 0 ? -1 : 1, 0);
        return thit;
      }
    }
  }

  if (b.z)
  { thit = (((b.z > 0) ^ inside ? min.z : max.z) - A.z) / b.z;
    if (tnear < thit && thit < tmin)
    { // check if hit point hp = A + thit * b  lies within face
      hpx = A.x + thit * b.x;
      hpy = A.y + thit * b.y;
      if (min.x <= hpx && hpx <= max.x && min.y <= hpy && hpy <= max.y)
      { if (normal)
          init3D (*normal, 0, 0, b.z > 0 ? -1 : 1);
        return thit;
      }
    }
  }

  return 0.0;

} // rayhitscube


// rayhitsline
// tests wheter ray A + b.t comes near the line P + k.v (dist^2 <= eps2)
// if so, t is returned (but tmin not changed), 0.0 otherwise

float rayhitsline (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& P, const vector3D& v, float eps2
)
{
  vector3D bv, pa, h;  // line direction, b X v, P - A, helper

  crp3D (b, v, bv);
  sub3D (P, A, pa);
  float denom = dot3D (bv, bv);

  if (denom)  // nonparallel lines
  {
    float d2 = dot3D (bv, pa);  // square distance
    d2 = d2 * d2 / denom;       // <bXv . (P-A)>^2 / <bXv . bXv>

    if (d2 <= eps2)
    {
      crp3D (pa, v, h);
      float hit = dot3D (bv, h) / denom;  // <bXv . (P-A)Xv> / <bXv . bXv>
      if (tnear < hit && hit < tmin)  // this line hit first
      {
        crp3D (pa, b, h);
        float k = dot3D (bv, h) / denom;  // <bXv . (P-A)Xv> / <bXv . bXv>
        if (0 <= k && k <= 1)  // on line segment from P to Q
          return hit;
      }
    }
  }
  else  // parallel lines
  {
    float hit = rayhitspoint (A, b, tnear, tmin, P, eps2);

    if (hit)
    {
      denom = dot3D (v, v);
      float k = denom ? (dot3D (pa, v) / denom) : 0;  // <(P-A) . v> / <v . v>
      // in case v is 0, we did the hit test with the single point

      if (0 <= k && k <= 1)  // on line segment from P to Q
        return hit;
    }
  }

  return 0.0;

} // rayhitsline


// rayhitspoint
// tests whether ray A + b.t comes near point P (distance^2 <= eps2)
// if so, t is returned (but tmin not changed), 0.0 otherwise

float rayhitspoint (
  const point3D& A, const vector3D& b, float tnear, float tmin,
  const point3D& P, float eps2
)
{
  float denom = dot3D (b, b);

  if (denom)  // otherwise degenerated ray
  {
    vector3D pa, h;
    sub3D (P, A, pa);
    float thit = dot3D (pa, b) / denom;  // <(P-A) . b> / <b . b>

    if (tnear < thit && thit < tmin)
    {
      pol3D (pa, -thit, b, h);   // h = P-A - thit*b
      if (dot3D (h, h) <= eps2)  // square norm of distance vector
        return thit;
    }
  }

  return 0.0;

} // rayhitspoint


void copymatrix (ARRAYconst matrix4D srcmat, matrix4D destmat)  // copy a matrix
{
  const float* src = (const float*) srcmat;
  float* dest = (float*) destmat;

  int j;
  for (j = 0; j < 16; j++)
    *dest++ = *src++;
}


void multiplyPoint3DMatrix4D (const point3D pt, ARRAYconst matrix4D mat, point3D& prod)
{
  // pt passed by value, so pt and prod may be the same
  prod.x = pt.x * mat [0][0] + pt.y * mat [1][0] + pt.z * mat [2][0] + mat [3][0];
  prod.y = pt.x * mat [0][1] + pt.y * mat [1][1] + pt.z * mat [2][1] + mat [3][1];
  prod.z = pt.x * mat [0][2] + pt.y * mat [1][2] + pt.z * mat [2][2] + mat [3][2];
}


void multiplyVector3DMatrix4D (const vector3D vec, ARRAYconst matrix4D mat, vector3D& prod)
{
  // as for points; vectors are not translated
  prod.x = vec.x * mat [0][0] + vec.y * mat [1][0] + vec.z * mat [2][0];
  prod.y = vec.x * mat [0][1] + vec.y * mat [1][1] + vec.z * mat [2][1];
  prod.z = vec.x * mat [0][2] + vec.y * mat [1][2] + vec.z * mat [2][2];
}


void transposematrix (matrix4D mat)
{
  unsigned i, j;
  float temp;

  for (i = 0;  i < 3;  i++)  // row 0 to 2
    for (j = i + 1;  j < 4;  j++)  // col i+1 to 3
    {
      temp = mat [i][j];
      mat [i][j] = mat [j][i];
      mat [j][i] = temp;
    }
}


void mat4Dto3D (matrix4D mat)  // set last row and column to (0, 0, 0, 1)
{
  mat [0][3] = mat [1][3] = mat [2][3] = 0.0;
  mat [3][0] = mat [3][1] = mat [3][2] = 0.0;
  mat [3][3] = 1.0;
}


// invertmatrix
// overwrites 4 by 4 matrix a with its inverse
// (algorithm from: Schwarz, "Numerische Mathematik")
// (row permutations for less numerical errors)

void invertmatrix (matrix4D a)
{
  int n = 4;

  float max, s, h, q, pivot;
  int p [4];
  int i, j, k;

  for (k = 0; k < n; k++)
  { max = 0.0;
    p [k] = 0;

    for (i = k; i < n; i++)
    { s = 0.0;
      for (j = k; j < n; j++)
        s += (float)vcl_fabs (a [i][j]);
      q = (float)vcl_fabs (a [i][k]) / s;
      if (q > max)
      { max = q;
        p [k] = i;
      }
    } // for i

    if (!max)
      return;
    if (p [k] != k)
      for (j = 0; j < n; j++)
      { h = a [k][j];  a [k][j] = a [p[k]][j];  a [p[k]][j] = h;
      }

    pivot = a [k][k];
    for (j = 0; j < n; j++)
      if (j != k)
      { a [k][j] = - a [k][j] / pivot;
        for (i = 0; i < n; i++)
          if (i != k)
            a [i][j] += a [i][k] * a [k][j];
      } // for j, if

    for (i = 0; i < n; i++)
      a [i][k] /= pivot;
    a [k][k] = 1 / pivot;

  } // for k

  for (k = n-2; k >= 0; k--)
    if (p [k] != k)
      for (i = 0; i < n; i++)
      { h = a [i][k];  a [i][k] = a [i][p[k]];  a [i][p[k]] = h;
      } // for k, if, for i

} // invertmatrix


// conversion cartesian - spherepoint

// cartes2sphere
// convert cartesian to sphere coordinates

void cartes2sphere (const point3D& p, spherepoint& s)
{
  s.r = vcl_sqrt (dot3D (p, p));  // length
  s.phi = (float)vcl_atan2 (p.y, p.x);  // angle from x to y axis
  s.theta = (s.r > 0.0) ? (float)vcl_acos (p.z / s.r) : 0.0f;  // angle from z axis to x-y-plane
}


// sphere2cartes
// convert sphere to cartesian coordinates
// p = (0, 0, r) . ROTy (theta) . ROTz (phi)

void sphere2cartes (const spherepoint& s, point3D& p)
{
  float sinth = (float)vcl_sin (s.theta);
  p.x = s.r * (float)vcl_cos (s.phi) * sinth;
  p.y = s.r * (float)vcl_sin (s.phi) * sinth;
  p.z = s.r * (float)vcl_cos (s.theta);
}


#if 0 // rotatearoundaxis() is not used

#include "Qv_pi.h" // for QV_ONE_OVER_PI

// convert radians to degrees
#define DEGREES(R)  ( (R) * 180 * QV_ONE_OVER_PI )

//:
// preconcatenate the current transformation matrix (of ge3d) with one that does a
// rotation about an arbitrary axis u through origin about an angle omega -
// in radians; positive direction = CCW when looking along u towards origin

void rotatearoundaxis (const vector3D& u, float omega)
{
  spherepoint s;
  cartes2sphere (u, s);

  // TEST !!!
  static int firstcall = 1;

  // sorry, ge3d (as gl) expects angles in degrees
  // note: the matrices are PREconcatenated, therefore to be read from bottom up!
  if (firstcall)  vcl_cerr << "rotz" << vcl_endl;
    ge3d_rotate_axis ('z', DEGREES (s.phi));
  if (firstcall)  ge3d_print_cur_matrix (), vcl_cerr << "roty" << vcl_endl;
    ge3d_rotate_axis ('y', DEGREES (s.theta));
  if (firstcall)  ge3d_print_cur_matrix (), vcl_cerr << "rotz" << vcl_endl;
    ge3d_rotate_axis ('z', DEGREES (omega));  // u lies in positive z axis
  if (firstcall)  ge3d_print_cur_matrix (), vcl_cerr << "roty" << vcl_endl;
    ge3d_rotate_axis ('y', DEGREES (-s.theta));
  if (firstcall)  ge3d_print_cur_matrix (), vcl_cerr << "rotz" << vcl_endl;
    ge3d_rotate_axis ('z', DEGREES (-s.phi));
  if (firstcall)  ge3d_print_cur_matrix ();
  firstcall = 0;
}
#endif

// quaternion utilities

// convert normalized rotation to normalized quaternion

void rotation2quaternion (const vector3D& axis, float angle, vector3D& v, float& s)
{
  angle /= 2;
  s = (float)vcl_sin (angle);
  v = axis;
  scl3D (v, s);         // v = vcl_sin (phi/2) * axis
  s = (float)vcl_cos (angle);  // s = vcl_cos (phi/2)
}


// convert normalized quaternion to normalized rotation

void quaternion2rotation (const vector3D& v, float s, vector3D& axis, float& angle)
{
  angle = (float)vcl_acos (s);  // half angle
  s = (float)vcl_sin (angle);
  if (s)
  { s = 1.0f / s;
    axis = v;
    scl3D (axis, s);  // axis = v / vcl_sin (vcl_acos (s))
  }
  else
    init3D (axis, 0, 0, 0);

  angle *= 2.0;  // angle = 2 * vcl_acos (s)
}


// multiply (concatenate) two normalized quaternions

static void multiply_quaternions (
  const vector3D& v0, float s0, const vector3D& v1, float s1,
  vector3D& v, float& s)
{
  // assert: v and s different from v[01] and s[01]
  s = s0 * s1 - dot3D (v0, v1);
  // v = s0*v1 + s1*v0 + v0Xv1
  vector3D temp = v1;  scl3D (temp, s0);
  v = temp;
  temp = v0;  scl3D (temp, s1);
  inc3D (v, temp);
  crp3D (v0, v1, temp);
  inc3D (v, temp);
}


void multiplyQuaternions (
  const vector3D& v0, float s0, const vector3D& v1, float s1,
  vector3D& v, float& s)
{
  // be aware of v or s refering to v[01] or s[01]
  vector3D vtmp;
  float stmp;
  multiply_quaternions (v0, s0, v1, s1, vtmp, stmp);
  v = vtmp;
  s = stmp;
}


// multiply (concatenate) two normalized rotations (via quaternions)

void multiplyRotations (
  const vector3D& axis0, float angle0, const vector3D& axis1, float angle1,
  vector3D& axis, float& angle)
{
  vector3D v0, v1, v;
  float s0, s1, s;
  rotation2quaternion (axis0, angle0, v0, s0);
  rotation2quaternion (axis1, angle1, v1, s1);
  multiply_quaternions (v0, s0, v1, s1, v, s);  // no pointer aliasing
  quaternion2rotation (v, s, axis, angle);
}


// object to world bounding box

void computeBoundingbox (const point3D& /*omin*/, const point3D& /*omax*/,
                         point3D& wmin, point3D& wmax)
{
  // transform the bounding box of the object into world coordinates
  // and then put another (aligned) bounding box around - it is exact for
  // cubes but somewhat larger for "round" objects
  // computing the 'best' world bounding box is not worthwile

  // transform all eight vertices of the object bounding and compare it to min/max

  emptyBoundingbox (wmin, wmax);

#if 0 // commented out
  point3D p; p.x = 0; p.y = 0; p.z = 0;

  for (int i = 0;  i < 8;  i++)
  {
    ge3d_transform_mc_wc (
      (i & 4) ? omin.x : omax.x,  (i & 2) ? omin.y : omax.y,  (i & 1) ? omin.z : omax.z,
      &p.x, &p.y, &p.z
    );
    extendBoundingbox (p, p, wmin, wmax);
  }
#endif

} // computeBoundingbox


// vcl_ostream << point3D
// also handles vcl_ostream << vector3D

vcl_ostream& operator << (vcl_ostream& os, const point3D& pt)
{
  os << '(' << pt.x << ", " << pt.y << ", " << pt.z << ')';
  return os;
}


// vcl_ostream << point2D

vcl_ostream& operator << (vcl_ostream& os, const point2D& pt)
{
  os << '(' << pt.x << ", " << pt.y << ')';
  return os;
}


// vcl_ostream << spherepoint

vcl_ostream& operator << (vcl_ostream& os, const spherepoint& sp)
{
  os << '[' << sp.r << ", " << sp.phi << ", " << sp.theta << ']';
  return os;
}


// vcl_ostream << matrix4D

vcl_ostream& operator << (vcl_ostream& os, ARRAYconst matrix4D mat)
{
  os << '\n';

  for (int i = 0;  i < 4;  i++)
  {
    const float* row = &(mat [i][0]);
    os << row [0] << ", " << row [1] << ", " << row [2] << ", " << row [3] << vcl_endl;
  }

  return os << vcl_endl;
}
