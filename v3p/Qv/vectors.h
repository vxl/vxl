#ifndef ge3d_vectors_h
#define ge3d_vectors_h

#if 0
//<copyright>
//
// Copyright (c) 1992,93,94,95
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
#endif

/*
 * File:     vectors.h
 *
 * Author:   Michael Hofer (until May 92) and Michael Pichler
 *
 * Created:  12 Mar 92
 *
 * Changed:  24 Jul 95
 *
 *
 */


typedef struct
{ float x, y;
} point2D;


typedef struct
{ float x, y, z;
} vector3D, point3D;


typedef float matrix4D [4][4];

/* this was defined as "matrix" in earlier versions,
 * but has been changed to "matrix4D" to avoid conflicts
 * with other libraries using the type "matrix"
 */


/* macros for 3D-vector operations */
/* comments in pseudo language */


/* initialisation/assignment.  p := (X, Y, Z)  */
#define init3D(p, X, Y, Z) \
  (p).x = (X),  (p).y = (Y),  (p).z = (Z)

/* initialisation/assignment.  p := (X, Y)  */
#define init2D(p, X, Y) \
  (p).x = (X),  (p).y = (Y)

/* addition.  a + b -> c  */
#define add3D(a, b, c)  (c).x = (a).x + (b).x, \
                        (c).y = (a).y + (b).y, \
                        (c).z = (a).z + (b).z

/* subtraction.  a - b -> c  */
#define sub3D(a,b,c)    (c).x = (a).x - (b).x, \
                        (c).y = (a).y - (b).y, \
                        (c).z = (a).z - (b).z

/* negation.  a = - a */
#define neg3D(a)     (a).x = - (a).x, \
                     (a).y = - (a).y, \
                     (a).z = - (a).z

/* increment.  a += b  */
#define inc3D(a, b) \
  (a).x += (b).x,  (a).y += (b).y,  (a).z += (b).z

/* decrement.  a -= b  */
#define dec3D(a, b) \
  (a).x -= (b).x,  (a).y -= (b).y,  (a).z -= (b).z

/* scale.  a *= s  */
#define scl3D(a, s)  (a).x *= (s), \
                     (a).y *= (s), \
                     (a).z *= (s)

/* dotproduct.  return <a.b>  */
#define dot3D(a, b)     ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)
#define dot2D(a, b)     ((a).x * (b).x + (a).y * (b).y)

/* norm.  return |a| */
#define norm3D(a)       sqrt (dot3D (a, a))
#define norm2D(a)       sqrt (dot2D (a, a))

/* crossproduct.  a X b -> c  */
/* important: make sure that c is not identical to a or b! */
#define crp3D(a, b, c)  (c).x = (a).y * (b).z  -  (a).z * (b).y, \
                        (c).y = (a).z * (b).x  -  (a).x * (b).z, \
                        (c).z = (a).x * (b).y  -  (a).y * (b).x

/* parametric line equation.  a + t*b -> c  */
#define pol3D(a, t, b, c)  (c).x = (a).x + (t) * (b).x, \
                           (c).y = (a).y + (t) * (b).y, \
                           (c).z = (a).z + (t) * (b).z

/* linear combination (interpolation).  s*a + t*b -> c  */
#define lcm3D(s, a, t, b, c)  \
  (c).x = (s) * (a).x + (t) * (b).x, \
  (c).y = (s) * (a).y + (t) * (b).y, \
  (c).z = (s) * (a).z + (t) * (b).z


#endif // ge3d_vectors_h
