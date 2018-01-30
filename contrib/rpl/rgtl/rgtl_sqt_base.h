/* Copyright 2006-2009 Brad King, Chuck Stewart
   Distributed under the Boost Software License, Version 1.0.
   (See accompanying file rgtl_license_1_0.txt or copy at
   http://www.boost.org/LICENSE_1_0.txt) */
#ifndef rgtl_sqt_base_h
#define rgtl_sqt_base_h

//:
// \file
// \brief Define a parameterization of the sphere for spherical quad-trees.
// \author Brad King
// \date February 2007

//: Define a parameterization of the sphere for spherical quad-trees.
//
// An axis-aligned cube circumscribing the unit sphere divides the
// sphere up into regions corresponding to the six faces of the cube.
// Each face is given two parameters (u,v) each in the range [-1,+1].
// Each parameter is proportional to an angle in the range
// [-pi/4,+pi/4] indicating the angle of rotation around an axis
// perpendicular to the axis of the face.  This parameterization
// results in a good ratio (about 0.7) of the smallest cell size to
// the largest cell size on the sphere surface in a regular grid
// formed in parameter space.  It also has planar facets separating
// adjacent cells.
class rgtl_sqt_base
{
public:
  //: Enumerate the 6 faces of the axis-aligned circumscribed cube.
  enum face_index_type
  {
    face_negative_x = 0,
    face_positive_x = 1,
    face_negative_y = 2,
    face_positive_y = 3,
    face_negative_z = 4,
    face_positive_z = 5
  };

  //: Get the index of the face through which the given direction points.
  static face_index_type direction_to_face(double const d[3]);

  //: Convert the given direction to its spherical parameters.
  static void direction_to_parameters(double const d[3],
                                      face_index_type& face,
                                      double& u, double& v);

  //: Convert the given spherical parameters to a direction.
  static void parameters_to_direction(face_index_type face,
                                      double u, double v, double d[3]);

  //: Get the normal to a plane of constant "u" in a face.
  //  The normal is a unit vector and points toward increasing "u".
  static void plane_normal_u(face_index_type face, double u, double n[3]);
  static void plane_normal_u(face_index_type face, double u, float n[3]);

  //: Get the normal to a plane of constant "v" in a face.
  //  The normal is a unit vector and points toward increasing "v".
  static void plane_normal_v(face_index_type face, double v, double n[3]);
  static void plane_normal_v(face_index_type face, double v, float n[3]);
};

#endif
