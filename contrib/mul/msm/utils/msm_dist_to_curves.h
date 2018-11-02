#ifndef msm_dist_to_curves_h_
#define msm_dist_to_curves_h_

//:
// \file
// \brief Functions to calculate distance from points to curves (poly-lines)
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <msm/msm_points.h>
#include <msm/msm_curve.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return square of distance to closest point on line segment [pt0-pt1]
inline double msm_sqr_dist_to_line_segment(const vgl_point_2d<double>& pt0,
                                const vgl_point_2d<double>& pt1,
                                const vgl_point_2d<double>& pt)
{
  vgl_vector_2d<double> u=pt1-pt0;
  vgl_vector_2d<double> dp=pt-pt0;
  double pu = u.x()*dp.x() + u.y()*dp.y();
  if (pu<=0) return dp.sqr_length();  // pt is closest to pt0
  double Lu2=u.sqr_length();
  if (pu>=Lu2) return (pt-pt1).sqr_length(); // pt is closest to pt1

  // pt is closest to some point between pt0 and pt1
  // Use pythagorus  :   dp^2 = d^2 + sqr(pu/u.length)
  return std::max(dp.sqr_length() - (pu*pu)/Lu2,0.0);
}

//: Compute the distance between pt and nearest point on given curve
//  The curve is treated as a polygon connecting subset of given points.
//  As long as the points are dense enough, this is a good enough approximation
//  to fitting a smooth curve through the points.
inline double msm_dist_to_curve(const msm_points& all_points,
                         const msm_curve& curve,
                         const vgl_point_2d<double>& pt)
{
  if (curve.size()==0) return 0;

  // If only one point, then find distance to it from pt
  if (curve.size()==1) return (all_points[curve[0]]-pt).length();

  // Compute distance between each line segment and the point
  double min_d2=msm_sqr_dist_to_line_segment(all_points[curve[0]],all_points[curve[1]],pt);
  unsigned n=curve.size();
  for (unsigned i=2;i<n;++i)
  {
    double d2 = msm_sqr_dist_to_line_segment(all_points[curve[i-1]],all_points[curve[i]],pt);
    if (d2<min_d2) min_d2=d2;
  }

  if (!curve.open())
  {  // Curve is closed, so check segment joining point [n-1] to point [0]
    double d2 = msm_sqr_dist_to_line_segment(all_points[curve[n-1]],all_points[curve[0]],pt);
    if (d2<min_d2) min_d2=d2;
  }

  return std::sqrt(min_d2);
}

//: Compute the distance between pt and nearest point on any of the curves through points
//  The curves are treated as a polygon connecting subset of given points.
inline double msm_dist_to_curves(const msm_points& points,
                         const msm_curves& curves,
                         const vgl_point_2d<double>& pt)
{
  if (curves.size()==0) return 0.0;
  double min_d = msm_dist_to_curve(points,curves[0],pt);
  for (unsigned c=1;c<curves.size();++c)
  {
    double d=msm_dist_to_curve(points,curves[c],pt);
    if (d<min_d) min_d=d;
  }
  return min_d;
}

//: Find the mean of closest distance between each point in points and the curves through ref_points
inline double msm_mean_dist_to_curves(const msm_points& ref_points,
                               const msm_curves& curves,
                               const msm_points& points)
{
  if (points.size()==0) return 0.0;
  double sum=0.0;
  for (unsigned i=0;i<points.size();++i)
    sum += msm_dist_to_curves(ref_points,curves,points[i]);

  return sum/points.size();
}

//: Find the mean of closest distance between each point in points and matching ref. curve
//  Assumes points.size()==ref_points.size().
//  Goes through each point listed in curves and finds the closest distance to equivalent
//  curve through ref_points.
inline double msm_mean_dist_to_matching_curves(const msm_points& ref_points,
                               const msm_curves& curves,
                               const msm_points& points)
{
  if (curves.size()==0) return 0.0;
  if (points.size()==0) return 0.0;
  assert(ref_points.size()==points.size());
  assert(curves.max_index()<ref_points.size());

  double sum=0.0;
  unsigned n_pts=0;

  for (unsigned c=0;c<curves.size();++c)
  {
    // Compare each point on curve c through points, with curve c through ref_points
    n_pts += curves[c].size();
    for (unsigned i=0;i<curves[c].size();++i)
      sum += msm_dist_to_curve(ref_points,curves[c],points[curves[c][i]]);
  }
  if (n_pts==0) return 0.0;
  return sum/n_pts;
}


#endif // msm_dist_to_curves_h_
