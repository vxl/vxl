#ifndef msm_closest_pt_h_
#define msm_closest_pt_h_

//:
// \file
// \brief Functions to calculate closest points on curves (poly-lines) to target point
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <msm/msm_points.h>
#include <msm/msm_curve.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Define position of point on a line segment between points with index i0,i1.
//  Point is (1-alpha)*pts[i0] + alpha*pts[i1]
//  If alpha==0, then point is just pts[i0]
struct msm_line_seg_pt
{
  int i0, i1;
  double alpha;

  //: 1 Construct as undefined
  msm_line_seg_pt() : i0(-1),i1(-1),alpha(0.0) {}

  //: 2 Construct as single point, index
  msm_line_seg_pt(int index) : i0(index),i1(-1),alpha(0.0) {}


  msm_line_seg_pt(int i0a, int i1a, double a) : i0(i0a)
  {
    if (a==0) { alpha=0.0; i1=-1; return; }
    i1=i1a; alpha=a;
  }

  vgl_point_2d<double> point(const std::vector<vgl_point_2d<double> >& pts) const
  {
    if (i0<0) return {};
    if (i1<0) return pts[i0];
    double b=1.0-alpha;
    return {b*pts[i0].x()+alpha*pts[i1].x(),
                                b*pts[i0].y()+alpha*pts[i1].y() };
  }
};

//: Return square of distance to closest point on line segment [pt0-pt1]
//  On exit, closest point on line segment to pt is given by
//  (1-alpha)*p0 + alpha*p1
inline double msm_sqr_dist_to_line_segment(const vgl_point_2d<double>& pt0,
                                const vgl_point_2d<double>& pt1,
                                const vgl_point_2d<double>& pt,
                                double& alpha)
{
  vgl_vector_2d<double> u=pt1-pt0;
  vgl_vector_2d<double> dp=pt-pt0;
  double pu = u.x()*dp.x() + u.y()*dp.y();
  if (pu<=0)
  {
    alpha=0.0;
    return dp.sqr_length();  // pt is closest to pt0
  }
  double Lu2=u.sqr_length();
  if (pu>=Lu2)
  {
    alpha=1.0;
    return (pt-pt1).sqr_length(); // pt is closest to pt1
  }

  // pt is closest to some point between pt0 and pt1
  alpha = pu/Lu2;

  // Use pythagorus  :   dp^2 = d^2 + sqr(pu/u.length)
  return std::max(dp.sqr_length() - (pu*pu)/Lu2,0.0);
}

//: Compute the point on the poly-line given by curve to pt
//  The curve is treated as a polygon connecting subset of given points.
//  On exit, sqr_dist gives the square of the distance from pt to that point.
inline msm_line_seg_pt msm_closest_seg_pt_on_curve(const msm_points& all_points,
                         const msm_curve& curve,
                         const vgl_point_2d<double>& pt,
                         double& sqr_dist)
{
  if (curve.size()==0)
  {
    sqr_dist=9.9e9;
    return {};
  }

  // If only one point, then find distance to it from pt
  if (curve.size()==1)
  {
    sqr_dist = (all_points[curve[0]]-pt).sqr_length();

    // Single point only
    return {static_cast<int>(curve[0])};
  }

  // Compute distance between each line segment and the point
  double alpha;
  sqr_dist=msm_sqr_dist_to_line_segment(all_points[curve[0]],all_points[curve[1]],pt,alpha);
  msm_line_seg_pt best_seg_pt(curve[0],curve[1],alpha);

  unsigned n=curve.size();
  for (unsigned i=2;i<n;++i)
  {
    double d2 = msm_sqr_dist_to_line_segment(all_points[curve[i-1]],all_points[curve[i]],pt,alpha);
    if (d2<sqr_dist)
    {
      sqr_dist=d2;
      best_seg_pt=msm_line_seg_pt(curve[i-1],curve[i],alpha);
    }
  }

  if (!curve.open())
  {  // Curve is closed, so check segment joining point [n-1] to point [0]
    double d2 = msm_sqr_dist_to_line_segment(all_points[curve[n-1]],all_points[curve[0]],pt,alpha);
    if (d2<sqr_dist)
    {
      sqr_dist=d2;
      best_seg_pt=msm_line_seg_pt(curve[n-1],curve[0],alpha);
    }
  }

  return best_seg_pt;
}


#endif // msm_closest_pt_h_
