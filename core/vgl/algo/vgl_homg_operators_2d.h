#ifndef vgl_homg_operations_2d_h
#define vgl_homg_operations_2d_h
#ifdef __GNUC__
#pragma interface
#endif


// .NAME vgl_homg_operators_2d
// .INCLUDE vgl/vgl_homg_operators_2d.h
// .FILE vgl_homg_operators_2d.txx
// .SECTION Author
//   Don Hamilton, Peter Tu
// Created: Feb 16 2000

#include <vnl/vnl_vector.h>
template <class Type> class vgl_homg_point_2d;
template <class Type> class vgl_homg_line_2d;
template <class Type> class vgl_homg_point_2d;
#include <vcl/vcl_vector.h>

//: 2D homogeneous operations
template <class Type>
class vgl_homg_operators_2d {
public:
  // method to get a vnl_vector rep of a homogeneous object
  static vnl_vector<Type> get_vector(vgl_homg_point_2d<Type> &p);
  static vnl_vector<Type> get_vector(vgl_homg_line_2d<Type> &p);

  static double dot(const vgl_homg_point_2d<Type>& a, const vgl_homg_point_2d<Type>& b);
  static void cross(const vgl_homg_point_2d<Type>& a, 
		    const vgl_homg_point_2d<Type>& b, 
		    vgl_homg_point_2d<Type>& a_cross_b);
  static void unitize(vgl_homg_point_2d<Type>& a);
  
  static double angle_between_oriented_lines (const vgl_homg_line_2d<Type>& line1,
					      const vgl_homg_line_2d<Type>& line2);
  static double abs_angle (const vgl_homg_line_2d<Type>& line1,
			   const vgl_homg_line_2d<Type>& line2);
  
  static double distance_squared (const vgl_homg_point_2d<Type>& point1, const vgl_homg_point_2d<Type>& point2);
  static double perp_distance_squared (const vgl_homg_line_2d<Type>& line,
				       const vgl_homg_point_2d<Type>& point);
  static double perp_dist_squared (const vgl_homg_point_2d<Type>& point, 
				   const vgl_homg_line_2d<Type>& line);
  static bool is_within_distance(const vgl_homg_point_2d<Type>& p1, 
				 const vgl_homg_point_2d<Type>& p2, double d) {
    return distance_squared(p1, p2) < d*d;
  }

  static double line_angle (const vgl_homg_line_2d<Type>& line);

  static vgl_homg_line_2d<Type> join (const vgl_homg_point_2d<Type>& point1, 
				      const vgl_homg_point_2d<Type>& point2);
  static vgl_homg_line_2d<Type> join_oriented (const vgl_homg_point_2d<Type>& point1,
					       const vgl_homg_point_2d<Type>& point2);
  static vgl_homg_point_2d<Type> intersection (const vgl_homg_line_2d<Type>& line1, 
					       const vgl_homg_line_2d<Type>& line2);
  static vgl_homg_line_2d<Type> perp_line_through_point (const vgl_homg_line_2d<Type>& line,
							 const vgl_homg_point_2d<Type>& point);

  static vgl_homg_point_2d<Type> perp_projection (const vgl_homg_line_2d<Type>& line, 
						  const vgl_homg_point_2d<Type>& point);
  static vgl_homg_point_2d<Type> midpoint (const vgl_homg_point_2d<Type>& p1, 
					   const vgl_homg_point_2d<Type>& p2);

#if 0
  //fsm: return type is not defined anywhere

  // Clip to lineseg. The infinite line is clipped against the viewport with
  // lower left corner (x0,y0) and upper right corner (x1,y1)
  static vgl_homg_line_seg_2d clip_line_to_lineseg (const vgl_homg_line_2d<Type>& line,
						    double x0, double y0,
						    double x1, double y1);
#endif

  // -- Intersect a set of 2D lines to find the least-square point of intersection.
  static vgl_homg_point_2d<Type> lines_to_point(const vcl_vector<vgl_homg_line_2d<Type> >& lines);

  // cross ratio of four colinear points, or four concurrent lines
  static double CrossRatio(const vgl_homg_point_2d<Type>& p1, const vgl_homg_point_2d<Type>& p2,
                           const vgl_homg_point_2d<Type>& p3, const vgl_homg_point_2d<Type>& p4);
  static vgl_homg_point_2d<Type> Conjugate(const vgl_homg_point_2d<Type>& a, 
					   const vgl_homg_point_2d<Type>& b, 
					   const vgl_homg_point_2d<Type>& c,
                          double cr);

  // compute most orthogonal vector
  static vnl_vector<double> most_orthogonal_vector(const vcl_vector<vgl_homg_line_2d<Type> >& inpoints);
  // compute most orthogonal vector with SVD
  static vnl_vector<double> most_orthogonal_vector_svd(const vcl_vector<vgl_homg_line_2d<Type> >& lines);
};

#endif
