
#include <vcl/vcl_iostream.h>

#include <vcl/vcl_list.h>
#include <vcl/vcl_vector.h>

#include <vnl/vnl_matops.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vnl/vnl_matrix_fixed.h>

//-----------------------------------------------------------------------------
//
// -- Return the angle between the (oriented) lines (in radians)
//
#if 0 // Why??
template <class Type>
Type vgl_homg_operators_3d<Type>::angle_between_oriented_lines (const vgl_homg_line_3d<Type>& l1,
                                                                const vgl_homg_line_3d<Type>& l2)
{
  vgl_homg_point_3d<Type> const& dir1 = l1.get_point_infinite();
  vgl_homg_point_3d<Type> const& dir2 = l2.get_point_infinite();
  double n = dir1.x()*dir1.x()+dir1.y()*dir1.y()+dir1.z()*dir1.z();
  n       *= dir2.x()*dir2.x()+dir2.y()*dir2.y()+dir2.z()*dir2.z();
  // dot product of unit direction vectors:
  n = (dir1.x()*dir2.x()+dir1.y()*dir2.y()+dir1.z()*dir2.z())/sqrt(n);
  return acos(n);
}
#endif


//-----------------------------------------------------------------------------
//
// -- Return the squared distance between the points
// 
template <class Type>
Type vgl_homg_operators_3d<Type>::distance_squared (const vgl_homg_point_3d<Type>& point1, 
						    const vgl_homg_point_3d<Type>& point2)
{
  Type mag = 0;
  Type d;

  d = point1.x() - point2.x();
  mag += d*d;

  d = point1.y() - point2.y();
  mag += d*d;

  d = point1.z() - point2.z();
  mag += d*d;

  // double d =point1.w() -point2.w();
  // mag = mag+d*d;
  
  // return (point1.get_double3() - point2.get_double3()).magnitude();
  
  return mag;

}

//-----------------------------------------------------------------------------
//
// -- Return the intersection point of the line and plane
// 
template <class Type>
vgl_homg_point_3d<Type> vgl_homg_operators_3d<Type>::intersect_line_and_plane (const vgl_homg_line_3d<Type> &line, const vgl_homg_plane_3d<Type>& plane)
{
  // 
  /* use P.(S + lambda D) = 0 to find lambda, and hence a point on the plane. */

  // TODO should have methods for DoubleVector from a point

  const vnl_vector<Type> x1 = get_vector(line.get_point_finite());
  const vnl_vector<Type> x2 = get_vector(line.get_point_infinite());
  const vnl_vector<Type>  p = get_vector(plane);

  // FIXME: this works for double and smaller, but not complex. it might happen.
  
  double numerator = -dot_product (x1, p);  // find out if dot_product is ok 
  double denominator = dot_product (x2, p);

  // Scale for conditioning
  double scale = 1.0/(numerator + denominator);
  numerator *= scale;
  denominator *= scale;
  
  return denominator * x1 + numerator * x2;
}

//-----------------------------------------------------------------------------
//
// - Compute the intersection point of the lines, or the mid-point
// of the common perpendicular if the lines are skew
// 
template <class Type>
vgl_homg_point_3d<Type> vgl_homg_operators_3d<Type>::lines_to_point (const vgl_homg_line_3d<Type>& , const vgl_homg_line_3d<Type>& )
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::lines_to_point() not yet implemented\n";
  return vgl_homg_point_3d<Type>();
}


//-----------------------------------------------------------------------------
//
// - Compute the best fit intersection point of the lines
// 
template <class Type>
vgl_homg_point_3d<Type> vgl_homg_operators_3d<Type>::lines_to_point (const vcl_vector<vgl_homg_line_3d<Type> >& )
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::lines_to_point() not yet implemented\n";
  return vgl_homg_point_3d<Type>();
}

//-----------------------------------------------------------------------------
//
// - Return the squared perpendicular distance between the line and point
// 
template <class Type>
double
vgl_homg_operators_3d<Type>::perp_distance_squared (const vgl_homg_line_3d<Type>& , const vgl_homg_point_3d<Type>& )
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::perp_distance_squared() not yet implemented\n";
  return 0;
}

//-----------------------------------------------------------------------------
//
// - Return the line which is perpendicular to *line and passes
// through *point
// 
template <class Type>
vgl_homg_line_3d<Type> vgl_homg_operators_3d<Type>::perp_line_through_point (const vgl_homg_line_3d<Type>& , const vgl_homg_point_3d<Type>& ) 
{ return vgl_homg_line_3d<Type>(); }


//-----------------------------------------------------------------------------
//
// - Compute the line which is the perpendicular projection of *point
// onto *line
// 
template <class Type>
vgl_homg_point_3d<Type>  vgl_homg_operators_3d<Type>::perp_projection (const vgl_homg_line_3d<Type>& , const vgl_homg_point_3d<Type>& )
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::perp_projection() not yet implemented\n";
  return vgl_homg_point_3d<Type>();
}


//-----------------------------------------------------------------------------
//
// -- Return the intersection line of the planes
// 
template <class Type>
vgl_homg_line_3d<Type> vgl_homg_operators_3d<Type>::planes_to_line (const vgl_homg_plane_3d<Type>& plane1, const vgl_homg_plane_3d<Type>& plane2)
{
  // TODO need equivilent of get_vector
  vnl_matrix<Type> M(2,4);
  M.set_row(0, get_vector(plane1));
  M.set_row(1, get_vector(plane2));
  vnl_svd<Type> svd(M);
  M = svd.nullspace(2);
  vgl_homg_point_3d<Type> p1(M.get_column(0));
  vgl_homg_point_3d<Type> p2(M.get_column(1));
  return vgl_homg_line_3d<Type>(p1, p2);
}


//-----------------------------------------------------------------------------
//
// - Compute the best-fit intersection line of the planes
// 
template <class Type>
vgl_homg_line_3d<Type> vgl_homg_operators_3d<Type>::planes_to_line (const vcl_vector<vgl_homg_plane_3d<Type> >&)
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::planes_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}


//-----------------------------------------------------------------------------
//
// - Return the line through the points
// 
template <class Type>
vgl_homg_line_3d<Type> vgl_homg_operators_3d<Type>::points_to_line (const vgl_homg_point_3d<Type>&, const vgl_homg_point_3d<Type>&)
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}

//-----------------------------------------------------------------------------
//
// - Compute the best-fit line through the points
//
template <class Type>
vgl_homg_line_3d<Type> vgl_homg_operators_3d<Type>::points_to_line (const vcl_vector<vgl_homg_point_3d<Type> >&)
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_line() not yet implemented\n";
  return vgl_homg_line_3d<Type>();
}

//-----------------------------------------------------------------------------
//
// - Return the plane through the points
//
template <class Type>
vgl_homg_plane_3d<Type> 
vgl_homg_operators_3d<Type>::points_to_plane (const vgl_homg_point_3d<Type>&, const vgl_homg_point_3d<Type>&, const vgl_homg_point_3d<Type>&)
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_plane() not yet implemented\n";
  return vgl_homg_plane_3d<Type>();
}


//-----------------------------------------------------------------------------
//
// - Compute the best-fit plane through the points
//
template <class Type>
vgl_homg_plane_3d<Type> 
vgl_homg_operators_3d<Type>::points_to_plane (const vcl_vector<vgl_homg_point_3d<Type> >&)
{
  cerr << "Warning: vgl_homg_operators_3d<Type>::points_to_plane() not yet implemented\n";
  return vgl_homg_plane_3d<Type>();
}

// -- Compute best-fit intersection of planes in a point.

template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::intersection_point (const vgl_homg_plane_3d<Type>& plane1, const vgl_homg_plane_3d<Type>& plane2, const vgl_homg_plane_3d<Type>& plane3)
{
  vnl_matrix_fixed<Type,3,4> A;
  A(0,0) = plane1.nx();
  A(0,1) = plane1.ny();
  A(0,2) = plane1.nz();
  A(0,3) = plane1.d();

  A(1,0) = plane2.nx();
  A(1,1) = plane2.ny();
  A(1,2) = plane2.nz();
  A(1,3) = plane2.d();

  A(2,0) = plane3.nx();
  A(2,1) = plane3.ny();
  A(2,2) = plane3.nz();
  A(2,3) = plane3.d();
  
  vnl_svd<Type> svd(A);
  // TODO find out how to get data out of svd.nullvector 
  // fsm: call vnl_vector<Type>::begin().
  return vgl_homg_point_3d<Type>(svd.nullvector().begin());
}

template <class Type>
vgl_homg_point_3d<Type>
vgl_homg_operators_3d<Type>::intersection_point (const vcl_vector<vgl_homg_plane_3d<Type> >& planes)
{
  int n = planes.size();
  vnl_matrix<Type> A(planes.size(), 4);

  for(int i =0; i < n; ++i) {
    A(i,0) = planes[i].nx();
    A(i,1) = planes[i].ny();
    A(i,2) = planes[i].nz();
    A(i,3) = planes[i].d();
  }

  vnl_svd<Type> svd(A);
  return vgl_homg_point_3d<Type>(svd.nullvector());
}

//-----------------------------------------------------------------------------
// -- Calculates the crossratio of four collinear points p1, p2, p3 and p4.
// This number is projectively invariant, and it is the coordinate of p4
// in the reference frame where p2 is the origin (coordinate 0), p3 is
// the unity (coordinate 1) and p1 is the point at infinity.
// This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
// equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
// and is calculated as
//                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
//                      ------- : --------  =  --------------
//                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
//
// In principle, any single nonhomogeneous coordinate from the four points
// can be used as parameters for CrossRatio (but of course the same for all
// points). The most reliable answer will be obtained when the coordinate with
// the largest spacing is used, i.e., the one with smallest slope.
//


template <class Type>
vnl_vector<Type> vgl_homg_operators_3d<Type>::get_vector(vgl_homg_point_3d<Type> &p)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(4);
  v.put(0,p.x());
  v.put(1,p.y());
  v.put(2,p.z());
  v.put(3,p.w());

  return v;
}

template <class Type>
vnl_vector<Type> vgl_homg_operators_3d<Type>::get_vector(vgl_homg_plane_3d<Type> &p)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(4);
  v.put(0,p.nx());
  v.put(1,p.ny());
  v.put(2,p.nz());
  v.put(3,p.d());

  return v;
}
  


template <class Type>
double vgl_homg_operators_3d<Type>::CrossRatio(const vgl_homg_point_3d<Type>& a, const vgl_homg_point_3d<Type>& b, const vgl_homg_point_3d<Type>& c, const vgl_homg_point_3d<Type>& d)
{
  double x1 = a.x(), y1 = a.y(), z1 = a.z(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), z2 = b.z(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), z3 = c.z(), w3 = c.w();
  double x4 = d.x(), y4 = d.y(), z4 = d.z(), w4 = d.w();
  double x = x1 - x2; if (x<0) x = -x; // assuming a != b ;-)
  double y = y1 - y2; if (y<0) y = -y;
  double z = z1 - z2; if (z<0) z = -z;
  double n = (x>y && x>z) ? (x1*w3-x3*w1)*(x2*w4-x4*w2) :
             (y>z)        ? (y1*w3-y3*w1)*(y2*w4-y4*w2) :
                            (z1*w3-z3*w1)*(z2*w4-z4*w2);
  double m = (x>y && x>z) ? (x1*w4-x4*w1)*(x2*w3-x3*w2) :
             (y>z)        ? (y1*w4-y4*w1)*(y2*w3-y3*w2) :
                            (z1*w4-z4*w1)*(z2*w3-z3*w2);
  if (n == 0 && m == 0)
    cerr << "CrossRatio not defined: three of the given points coincide" << endl;
  return n/m;
}

