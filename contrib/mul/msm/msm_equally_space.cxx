//:
// \file
// \brief Functions to equally space points along smooth curves
// \author Tim Cootes

#include <msm/msm_equally_space.h>
#include <msm/msm_cubic_bezier.h>

//: Slide points along curve through current points to equally space them.
// If curve is open, the first and last points are fixed.
// If curve is closed, the first point is fixed.
// All points not on curve are left unchanged
void msm_equally_space(msm_points& points,
                       msm_curve& curve,
                       double min_bez_sep)
{
  // Construct a smooth curve through selected points
  std::vector<vgl_point_2d<double> > pts(curve.size());
  for (unsigned i=0;i<curve.size();++i) pts[i]=points[curve[i]];
  msm_cubic_bezier bezier(pts,!curve.open());
  unsigned n_pts = pts.size();

  std::vector<vgl_point_2d<double> > new_pts;
  int last_index=n_pts-1;
  if (curve.open())
    bezier.equal_space(0,n_pts-1,n_pts,min_bez_sep,new_pts);
  else
  {
    bezier.equal_space(0,0,n_pts,min_bez_sep,new_pts);
    last_index=n_pts;
  }

  // Replace the points on the curve with pts
  for (unsigned i=1;i<last_index;++i)
  {
    points.set_point(curve[i],new_pts[i]);
  }
}

//: Slide points along each curve through current points to equally space them.
// If curve is open, the first and last points are fixed.
// If curve is closed, the first point is fixed.
// All points not on curves are left unchanged
void msm_equally_space(msm_points& points,
                       msm_curves& curves,
                       double min_bez_sep)
{
  for (unsigned i=0;i<curves.size();++i)
    msm_equally_space(points,curves[i],min_bez_sep);
}
