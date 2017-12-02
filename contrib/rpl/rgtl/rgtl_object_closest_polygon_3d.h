#ifndef rgtl_object_closest_polygon_3d_h
#define rgtl_object_closest_polygon_3d_h
//:
// \file
// \brief Compute the closest point on a polygon in 3d.
// \author Brad King
// \date March 2007
// \copyright
// Copyright 2006-2009 Brad King, Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rgtl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//: Compute the closest point on a planar polygon in 3d to a query point.
//
//  The geometry of the polygon is specified by an object passed to
//  the constructor.  The interface of the Geometry template argument
//  must be
// \code
//   struct Geometry
//   {
//     unsigned int get_number_of_vertices() const;
//     double const* get_vertex(unsigned int) const;
//     double const* get_edge_tangent(unsigned int) const;
//     double const* get_edge_normal(unsigned int) const;
//     double const* get_face_normal() const;
//   }
// \endcode
//  The face normal and edge tangent directions must consistently
//  correspond to a right-handed walk around the polygon.
//  The edge normals must point to the interior of the face.
template <typename Geometry>
class rgtl_object_closest_polygon_3d
{
 public:
  rgtl_object_closest_polygon_3d(Geometry const& g): g_(g) {}

  //: Enumeration of possible closest feature types.
  enum feature_kind_type
  {
    feature_vertex,
    feature_edge,
    feature_face
  };

  //: Compute the closest point on the current polygon.
  void compute_closest_point(double const x[3], double y[3])
  {
    feature_kind_type feature_kind;
    int feature_index;
    this->compute_closest_point(x, y, feature_kind, feature_index);
  }

  //: Compute the closest point on the current polygon.
  //  Also get which feature has the closest point.
  void compute_closest_point(double const x[3], double y[3],
                             feature_kind_type& feature_kind,
                             int& feature_index)
    {
    // The query point is closest to the polygon interior, an edge, or
    // a vertex.  Planes containing the edges and normal separate the
    // interior voronoi region from the edge and vertex voronoi
    // regions.  Planes perpendicular to the edges positioned at the
    // vertices separate the edge voronoi regions from the vertex
    // voronoi regions.
    unsigned int n = g_.get_number_of_vertices();

    bool prev_edge_begin_interior = false;
    bool first_prev_edge_end_interior = false;

    // Keep track of the negative tangent direction of the edge
    // entering the current vertex in a right-handed walk.
    double prev_edge_tangent[3];

    // Traverse the vertices and edges of the face in right-hand order
    // testing the voronoi region of each primitive.
    for (unsigned int i=0; i < n; ++i)
    {
      // Get the current vertex.
      double const* vertex = g_.get_vertex(i);

      // Compute the direction from the vertex to the query point.
      double x_minus_vertex[3] = {x[0] - vertex[0],
                                  x[1] - vertex[1],
                                  x[2] - vertex[2]};

      // The voronoi boundary planes for the vertex have normals along
      // the edge directions.  Orient the normals to face toward the
      // edge interiors.
      if (i == 0)
      {
        this->copy_minus(g_.get_edge_tangent(n-1), prev_edge_tangent);
      }
      bool prev_edge_end_interior =
        this->dot(prev_edge_tangent, x_minus_vertex) > 0;

      // Check if the point is in the voronoi region of this edge.
      if (i != 0 &&
          prev_edge_begin_interior && prev_edge_end_interior)
      {
        // The query point projects onto an edge line segment.  The
        // voronoi boundary plane between the edge and the face
        // interior contains the edge and is normal to the face.
        // Orient the normal to point toward the face interior.
        double const* prev_edge_normal = g_.get_edge_normal(i-1);
        if (!(this->dot(prev_edge_normal, x_minus_vertex) > 0))
        {
          // Point is closest to this edge.
          this->closest_on_line(vertex, prev_edge_tangent, x, y);
          feature_kind = feature_edge;
          feature_index = i-1;
          return;
        }
      }

      double next_edge_tangent[3];
      this->copy_plus(g_.get_edge_tangent(i), next_edge_tangent);
      bool next_edge_begin_interior =
        this->dot(next_edge_tangent, x_minus_vertex) > 0;

      // Check if the point is in the voronoi region of this vertex.
      if (!prev_edge_end_interior && !next_edge_begin_interior)
      {
        // Point is closest to this vertex.
        this->copy_plus(vertex, y);
        feature_kind = feature_vertex;
        feature_index = i;
        return;
      }

      // If this is the first iteration save information for testing
      // the final edge.
      if (i == 0)
      {
        first_prev_edge_end_interior = prev_edge_end_interior;
      }

      // If this is the last iteration test the final edge.
      if (i == (n-1))
      {
        bool next_edge_end_interior = first_prev_edge_end_interior;
        if (next_edge_begin_interior && next_edge_end_interior)
        {
          // The query point projects onto an edge line segment.  The
          // voronoi boundary plane between the edge and the face
          // interior contains the edge and is normal to the face.
          // Orient the normal to point toward the face interior.
          double const* next_edge_normal = g_.get_edge_normal(i);
          if (!(this->dot(next_edge_normal, x_minus_vertex) > 0))
          {
            // Point is closest to this edge.
            this->closest_on_line(vertex, next_edge_tangent, x, y);
            feature_kind = feature_edge;
            feature_index = i;
            return;
          }
        }
      }

      // The next edge will be the previous edge on the next
      // iteration.  Flip the direction since we will be looking at
      // the other end.
      this->copy_minus(next_edge_tangent, prev_edge_tangent);
      prev_edge_begin_interior = next_edge_begin_interior;
    }

    // Point is closest to the face interior.
    this->closest_on_plane(g_.get_vertex(0), g_.get_face_normal(), x, y);
    feature_kind = feature_face;
    feature_index = 0;
    }

 private:
  // The geometry of the polygon.
  Geometry const& g_;

  // Math utilities.
  static void copy_plus(double const u[3], double v[3])
  {
    v[0] = u[0];
    v[1] = u[1];
    v[2] = u[2];
  }
  static void copy_minus(double const u[3], double v[3])
  {
    v[0] = -u[0];
    v[1] = -u[1];
    v[2] = -u[2];
  }
  static double dot(double const u[3], double const v[3])
  {
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
  }

  // Compute the closest point on a line to the given point.
  static void closest_on_line(double const p[3], double const t[3],
                              double const x[3], double y[3])
  {
    double v[3] = {x[0]-p[0], x[1]-p[1], x[2]-p[2]};
    double s = dot(t, v) / dot(t, t);
    for (unsigned int a=0; a < 3; ++a)
    {
      y[a] = p[a] + s*t[a];
    }
  }

  // Compute the closest point on a plane to the given point.
  static void closest_on_plane(double const p[3], double const n[3],
                               double const x[3], double y[3])
  {
    double v[3] = {x[0]-p[0], x[1]-p[1], x[2]-p[2]};
    double s = dot(n, v) / dot(n, n);
    for (unsigned int a=0; a < 3; ++a)
    {
      y[a] = x[a] - s*n[a];
    }
  }
};

#endif // rgtl_object_closest_polygon_3d_h
