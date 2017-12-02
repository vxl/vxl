#include "rgtl_sqt_base.h"
//:
// \file
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <vnl/vnl_math.h>
// Define a constant for pi.
#define RGTL_SQT_PI vnl_math::pi

// Conversion factor from parameter value to angle.
#define RGTL_SQT_PI_OVER_FOUR vnl_math::pi_over_4

// Conversion factor from angle to parameter value.
#define RGTL_SQT_FOUR_OVER_PI (2.0 * vnl_math::two_over_pi)

//:
// The following rules help write the functions based on the ascii-art
// diagrams of the parameterization of each face.  These rules hold no
// matter how the parameters are oriented.  Changing the orientation
// of a face requires first updating the diagram and then applying
// these rules to re-write the functions.
//
// For direction/parameter conversion:
//  - axis into screen has +1 and is on bottom  inside atan with + sign
//  - axis from screen has -1 and is on bottom  inside atan with - sign
//  - axis to right    has u inside +tan and is inside atan with + sign
//  - axis to left     has u inside -tan and is inside atan with - sign
//  - axis up          has v inside +tan and is inside atan with + sign
//  - axis down        has v inside -tan and is inside atan with - sign
//
// For plane normal vectors:
//  - axis normal to screen has 0
//  - axis normal to face has +sin for away from face
//                        and -sin for toward face
//  - axis parallel to face has +cos for axis in parameter direction
//                          and -cos for axis in negative parameter direction
//
// Note that these functions produce directions that are not unit
// length.  The plane normal vectors however are unit length.

//----------------------------------------------------------------------------
// Negative X Axis Face
//        \      z      /                                    z
//  (-1,+1)\_____|_____/(+1,+1)   ___________                |    v=+1
//         |     |     |        u=-1       u=+1              |     |
//         |     |x    |                z                    |y    |
//         |     O------- y            O------- y   x -------O     |
//         |           |               |                           |
//         |___________|               |                           |
//  (-1,-1)/           \(+1,-1)        |                          v=-1
//        /             \              x
static void ParametersToDirectionNegativeX(double u, double v, double d[3])
{
  d[0] = -1;
  d[1] = +std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[2] = +std::tan(v * RGTL_SQT_PI_OVER_FOUR);
}

static void DirectionToParametersNegativeX(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(+d[1]/-d[0]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(+d[2]/-d[0]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUNegativeX(double u, double n[3])
{
  n[0] = +std::sin(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = +std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[2] = 0;
}

static void PlaneNormalVNegativeX(double v, double n[3])
{
  n[0] = +std::sin(v * RGTL_SQT_PI_OVER_FOUR);
  n[1] = 0;
  n[2] = +std::cos(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
//:
// Positive X Axis Face
//        \      z      /              x                     z
//  (-1,+1)\_____|_____/(+1,+1)   _____|_____                |    v=+1
//         |     |     |        u=-1   |   u=+1              |     |
//         |     |x    |               |z                    |y    |
//      y -------*     |      y -------O                     *------- x
//         |           |                                           |
//         |___________|                                           |
//  (-1,-1)/           \(+1,-1)                                   v=-1
//        /             \             -x
static void ParametersToDirectionPositiveX(double u, double v, double d[3])
{
  d[0] = +1;
  d[1] = -std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[2] = std::tan(v * RGTL_SQT_PI_OVER_FOUR);
}

static void DirectionToParametersPositiveX(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(-d[1]/+d[0]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(+d[2]/+d[0]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUPositiveX(double u, double n[3])
{
  n[0] = -std::sin(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = -std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[2] = 0;
}

static void PlaneNormalVPositiveX(double v, double n[3])
{
  n[0] = -std::sin(v * RGTL_SQT_PI_OVER_FOUR);
  n[1] = 0;
  n[2] = +std::cos(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
//:
// Negative Y Axis Face
//        \      z      /                                    z
//  (-1,+1)\_____|_____/(+1,+1)   ___________                |    v=+1
//         |     |     |        u=-1       u=+1              |     |
//         |     |y    |                z                    |x    |
//      x -------O     |      x -------O            y -------*     |
//         |           |               |                           |
//         |___________|               |                           |
//  (-1,-1)/           \(+1,-1)        |                          v=-1
//        /             \              y
static void ParametersToDirectionNegativeY(double u, double v, double d[3])
{
  d[0] = -std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[1] = -1;
  d[2] = std::tan(v * RGTL_SQT_PI_OVER_FOUR);
}

static void DirectionToParametersNegativeY(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(-d[0]/-d[1]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(+d[2]/-d[1]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUNegativeY(double u, double n[3])
{
  n[0] = -std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = +std::sin(u * RGTL_SQT_PI_OVER_FOUR);
  n[2] = 0;
}

static void PlaneNormalVNegativeY(double v, double n[3])
{
  n[0] = 0;
  n[1] = +std::sin(v * RGTL_SQT_PI_OVER_FOUR);
  n[2] = +std::cos(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
//:
// Positive Y Axis Face
//        \      z      /              y                     z
//  (-1,+1)\_____|_____/(+1,+1)   _____|_____                |    v=+1
//         |     |     |        u=-1   |   u=+1              |     |
//         |     |y    |               |z                    |x    |
//         |     *------- x            O------- x            O------- y
//         |           |                                           |
//         |___________|                                           |
//  (-1,-1)/           \(+1,-1)                                   v=-1
//        /             \             -y
static void ParametersToDirectionPositiveY(double u, double v, double d[3])
{
  d[0] = std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[1] = +1;
  d[2] = std::tan(v * RGTL_SQT_PI_OVER_FOUR);
}

static void DirectionToParametersPositiveY(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(+d[0]/+d[1]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(+d[2]/+d[1]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUPositiveY(double u, double n[3])
{
  n[0] = +std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = -std::sin(u * RGTL_SQT_PI_OVER_FOUR);
  n[2] = 0;
}

static void PlaneNormalVPositiveY(double v, double n[3])
{
  n[0] = 0;
  n[1] = -std::sin(v * RGTL_SQT_PI_OVER_FOUR);
  n[2] = +std::cos(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
//:
// Negative Z Axis Face
//        \      y      /                                    y
//  (-1,+1)\_____|_____/(+1,+1)   ___________                |    v=+1
//         |     |     |        u=-1       u=+1              |     |
//         |     |z    |                y                    |x    |
//         |     O------- x            O------- x   z -------O     |
//         |           |               |                           |
//         |___________|               |                           |
//  (-1,-1)/           \(+1,-1)        |                          v=-1
//        /             \              z
static void ParametersToDirectionNegativeZ(double u, double v, double d[3])
{
  d[0] = std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[1] = std::tan(v * RGTL_SQT_PI_OVER_FOUR);
  d[2] = -1;
}

static void DirectionToParametersNegativeZ(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(+d[0]/-d[2]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(+d[1]/-d[2]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUNegativeZ(double u, double n[3])
{
  n[0] = +std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = 0;
  n[2] = +std::sin(u * RGTL_SQT_PI_OVER_FOUR);
}

static void PlaneNormalVNegativeZ(double v, double n[3])
{
  n[0] = 0;
  n[1] = +std::cos(v * RGTL_SQT_PI_OVER_FOUR);
  n[2] = +std::sin(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
//:
// Positive Z Axis Face
//        \             /              z
//  (-1,+1)\___________/(+1,+1)   _____|_____                     v=+1
//         |           |        u=-1   |   u=+1                    |
//         |      z    |               |y                     x    |
//         |     *------- x            *------- x            O------- z
//         |     |     |                                     |     |
//         |_____|_____|                                     |     |
//  (-1,-1)/     |     \(+1,-1)                              |    v=-1
//        /      y      \                                    y
static void ParametersToDirectionPositiveZ(double u, double v, double d[3])
{
  d[0] = std::tan(u * RGTL_SQT_PI_OVER_FOUR);
  d[1] = -std::tan(v * RGTL_SQT_PI_OVER_FOUR);
  d[2] = +1;
}

static void DirectionToParametersPositiveZ(double const d[3],
                                           double& u, double& v)
{
  u = std::atan(+d[0]/+d[2]) * RGTL_SQT_FOUR_OVER_PI;
  v = std::atan(-d[1]/+d[2]) * RGTL_SQT_FOUR_OVER_PI;
}

static void PlaneNormalUPositiveZ(double u, double n[3])
{
  n[0] = +std::cos(u * RGTL_SQT_PI_OVER_FOUR);
  n[1] = 0;
  n[2] = -std::sin(u * RGTL_SQT_PI_OVER_FOUR);
}

static void PlaneNormalVPositiveZ(double v, double n[3])
{
  n[0] = 0;
  n[1] = -std::cos(v * RGTL_SQT_PI_OVER_FOUR);
  n[2] = -std::sin(v * RGTL_SQT_PI_OVER_FOUR);
}

//----------------------------------------------------------------------------
rgtl_sqt_base::face_index_type
rgtl_sqt_base::direction_to_face(double const d[3])
{
  unsigned int max_a = 0;
  double mag_d[3] = {std::fabs(d[0]), std::fabs(d[1]), std::fabs(d[2])};
  if (mag_d[1] > mag_d[max_a]) { max_a = 1; }
  if (mag_d[2] > mag_d[max_a]) { max_a = 2; }
  return face_index_type((max_a<<1) | (d[max_a] >= 0? 1:0));
}

//----------------------------------------------------------------------------
void
rgtl_sqt_base::direction_to_parameters(double const d[3],
                                       face_index_type& face,
                                       double& u, double& v)
{
  face = direction_to_face(d);
  switch (face)
  {
    case face_negative_x: DirectionToParametersNegativeX(d, u, v); return;
    case face_positive_x: DirectionToParametersPositiveX(d, u, v); return;
    case face_negative_y: DirectionToParametersNegativeY(d, u, v); return;
    case face_positive_y: DirectionToParametersPositiveY(d, u, v); return;
    case face_negative_z: DirectionToParametersNegativeZ(d, u, v); return;
    case face_positive_z: DirectionToParametersPositiveZ(d, u, v); return;
    default: return;
  }
}

//----------------------------------------------------------------------------
void
rgtl_sqt_base::parameters_to_direction(face_index_type face,
                                       double u, double v, double d[3])
{
  switch (face)
  {
    case face_negative_x: ParametersToDirectionNegativeX(u, v, d); return;
    case face_positive_x: ParametersToDirectionPositiveX(u, v, d); return;
    case face_negative_y: ParametersToDirectionNegativeY(u, v, d); return;
    case face_positive_y: ParametersToDirectionPositiveY(u, v, d); return;
    case face_negative_z: ParametersToDirectionNegativeZ(u, v, d); return;
    case face_positive_z: ParametersToDirectionPositiveZ(u, v, d); return;
    default: return;
  }
}

//----------------------------------------------------------------------------
void rgtl_sqt_base::plane_normal_u(face_index_type face, double u,
                                   double n[3])
{
  switch (face)
  {
    case face_negative_x: PlaneNormalUNegativeX(u, n); return;
    case face_positive_x: PlaneNormalUPositiveX(u, n); return;
    case face_negative_y: PlaneNormalUNegativeY(u, n); return;
    case face_positive_y: PlaneNormalUPositiveY(u, n); return;
    case face_negative_z: PlaneNormalUNegativeZ(u, n); return;
    case face_positive_z: PlaneNormalUPositiveZ(u, n); return;
    default: return;
  }
}

//----------------------------------------------------------------------------
void rgtl_sqt_base::plane_normal_v(face_index_type face, double v,
                                   double n[3])
{
  switch (face)
  {
    case face_negative_x: PlaneNormalVNegativeX(v, n); return;
    case face_positive_x: PlaneNormalVPositiveX(v, n); return;
    case face_negative_y: PlaneNormalVNegativeY(v, n); return;
    case face_positive_y: PlaneNormalVPositiveY(v, n); return;
    case face_negative_z: PlaneNormalVNegativeZ(v, n); return;
    case face_positive_z: PlaneNormalVPositiveZ(v, n); return;
    default: return;
  }
}

//----------------------------------------------------------------------------
void rgtl_sqt_base::plane_normal_u(face_index_type face, double u,
                                   float n[3])
{
  double nd[3];
  rgtl_sqt_base::plane_normal_u(face, u, nd);
  n[0] = static_cast<float>(nd[0]);
  n[1] = static_cast<float>(nd[1]);
  n[2] = static_cast<float>(nd[2]);
}

//----------------------------------------------------------------------------
void rgtl_sqt_base::plane_normal_v(face_index_type face, double v,
                                   float n[3])
{
  double nd[3];
  rgtl_sqt_base::plane_normal_v(face, v, nd);
  n[0] = static_cast<float>(nd[0]);
  n[1] = static_cast<float>(nd[1]);
  n[2] = static_cast<float>(nd[2]);
}
