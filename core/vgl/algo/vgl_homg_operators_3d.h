#ifndef vgl_homg_operators_3d_h
#define vgl_homg_operators_3d_h
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_homg_operators_3d
// .LIBRARY vgl-algo
// .INCLUDE vgl/algo/vgl_homg_operators_3d.h
// .FILE vgl_homg_operators_3d.txx
// .SECTION Author
//    Don Hamilton, Peter Tu
// Created: Feb 16 2000
// .SECTION Modifications
//   31-oct-00 Peter Vanroose - implementations fixed, and vgl_homg_line_3d typedef'd

#include <vcl/vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_homg_line_3d_2_points.h>

template <class Type> class vgl_homg_point_3d;
template <class Type> class vgl_homg_line_3d_2_points;
template <class Type> class vgl_homg_plane_3d;

//: 3D homogeneous operations
template <class Type>
class vgl_homg_operators_3d {
  typedef vgl_homg_line_3d_2_points<Type> vgl_homg_line_3d;

public:
  
  // method to get a vnl_vector rep of a homogeneous object
  
  static vnl_vector<Type> get_vector(vgl_homg_point_3d<Type> const& p);
  static vnl_vector<Type> get_vector(vgl_homg_plane_3d<Type> const& p);

  static double angle_between_oriented_lines (const vgl_homg_line_3d& line1, const vgl_homg_line_3d& line2);
  static Type distance (const vgl_homg_point_3d<Type>& point1, const vgl_homg_point_3d<Type>& point2);
  static Type distance_squared (const vgl_homg_point_3d<Type>& point1, const vgl_homg_point_3d<Type>& point2);
  static vgl_homg_point_3d<Type> intersect_line_and_plane (const vgl_homg_line_3d&, const vgl_homg_plane_3d<Type>&);
  static vgl_homg_point_3d<Type> lines_to_point (const vgl_homg_line_3d& line1, const vgl_homg_line_3d& line2);
  static vgl_homg_point_3d<Type> lines_to_point (const vcl_vector<vgl_homg_line_3d >& line_list);
  static double perp_distance_squared (const vgl_homg_line_3d& line, const vgl_homg_point_3d<Type>& point);
  static vgl_homg_line_3d perp_line_through_point (const vgl_homg_line_3d& line, const vgl_homg_point_3d<Type>& point);
  static vgl_homg_point_3d<Type> perp_projection (const vgl_homg_line_3d& line, const vgl_homg_point_3d<Type>& point);
  static vgl_homg_line_3d planes_to_line (const vgl_homg_plane_3d<Type>& plane1, const vgl_homg_plane_3d<Type>& plane2);
  static vgl_homg_line_3d planes_to_line (const vcl_vector<vgl_homg_plane_3d<Type> >& plane_list);
  static vgl_homg_line_3d points_to_line (const vgl_homg_point_3d<Type>& point1, const vgl_homg_point_3d<Type>& point2);
  static vgl_homg_line_3d points_to_line (const vcl_vector<vgl_homg_point_3d<Type> >& point_list);

  static vgl_homg_plane_3d<Type> points_to_plane (const vgl_homg_point_3d<Type>& , const vgl_homg_point_3d<Type>& , const vgl_homg_point_3d<Type>& );
  static vgl_homg_plane_3d<Type> points_to_plane (const vcl_vector<vgl_homg_point_3d<Type> >& point_list);
  static vgl_homg_point_3d<Type> intersection_point (const vgl_homg_plane_3d<Type>& , const vgl_homg_plane_3d<Type>& , const vgl_homg_plane_3d<Type>& );
  static vgl_homg_point_3d<Type> intersection_point (const vcl_vector<vgl_homg_plane_3d<Type> >&);

    
  // cross ratio of four colinear points, or four planes through a common line
  static double CrossRatio(const vgl_homg_point_3d<Type>& p1, const vgl_homg_point_3d<Type>& p2,
                           const vgl_homg_point_3d<Type>& p3, const vgl_homg_point_3d<Type>& p4);
};

#endif // _vgl_homg_operators_3d_h
