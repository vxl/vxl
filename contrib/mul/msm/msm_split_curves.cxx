//:
// \file
// \brief Functions to split curves at particular points
// \author Tim Cootes

#include "msm_split_curves.h"
#include <cmath>


//: Sets is_junc[i] to true if any point mentioned in >1 curve
void msm_get_junctions(const msm_curves& curves,
                       unsigned n_pts,
                       std::vector<bool>& is_junc)
{
  if (curves.max_index()>=n_pts)
  {
    std::cerr<<"msm_get_junctions : "
             <<"curves include more points than supplied\n"
             <<"(max index: "<<curves.max_index()<<")"<<std::endl;
    return;
  }

  std::vector<unsigned> count(n_pts,0);
  for (unsigned c=0;c<curves.size();++c)
  {
    for (unsigned i=0;i<curves[c].size();++i)
      count[curves[c][i]]+=1;
  }

  is_junc.resize(n_pts);
  for (unsigned i=0;i<n_pts;++i)
    is_junc[i]=(count[i]>1);
}

//: Splits curve at internal point p with end_pt[p] true
//  Appends new curves to new_curves.
void msm_split_curve(const msm_curve& curve,
                      const std::vector<bool>& end_pt,
                      msm_curves& new_curves)
{
  // Find where split points are along curve
  std::vector<unsigned> end_index;
  unsigned n=curve.size();
  for (unsigned i=0;i<n;++i)
    if (end_pt[curve[i]]) end_index.push_back(i);

  if (end_index.empty()) {
    // No split points, so curve unchanged
    new_curves.push_back(curve);
    return;
  }

  if (curve.open())
  {
    if (end_index.size()==1)
    {
      if (end_pt[curve[0]] || end_pt[curve[n-1]])
      {
        // No need to split at end
        new_curves.push_back(curve);
        return;
      }
    }
    std::vector<unsigned> index;
    index.push_back(curve[0]);  // Start of curve
    for (unsigned i=1;i<n;++i)
    {
      index.push_back(curve[i]);
      if (end_pt[curve[i]] || i==(n-1))
      {
        msm_curve new_crv=curve;
        new_crv.set(index,true,curve.name());
        new_curves.push_back(new_crv);
        index.resize(1);  // Start a new curve
        index[0]=curve[i];
      }
    }
    return;
  }
  else // curve is closed
  {
    // Split closed curve into one or more open curves.
    // Find index of first end_pt
    unsigned i0=end_index[0];
    std::vector<unsigned> index(1);
    index[0]=curve[i0];
    for (unsigned i=1;i<=n;++i)  // Extra point (first==last) in closed
    {
      unsigned pi=curve[(i0+i)%n];
      index.push_back(pi);
      if (end_pt[pi] || i==n)
      {
        msm_curve new_crv=curve;
        new_crv.set(index,true,curve.name());
        new_curves.push_back(new_crv);
        index.resize(1);  // Start a new curve
        index[0]=pi;
      }
    }
  }
}

//: Splits any curve at internal points p where end_pt[p] is true
// Thus closed curves may become one or more open curves.
// For example, if end_pt[3] is true, and one open curve is 1-2-3-4-5,
// then it will be split into two curves 1-2-3 and 3-4-5.
// If the curve was closed, it would be turned into the open curve 3-4-5-1-2-3
void msm_split_curves(const msm_curves& curves,
                      const std::vector<bool>& end_pt,
                      msm_curves& new_curves)
{
  new_curves.resize(0);
  for (unsigned c=0;c<curves.size();++c)
  {
    msm_split_curve(curves[c],end_pt,new_curves);
  }
}

inline double cos_angle(vgl_point_2d<double> p1,
                    vgl_point_2d<double> p2,
                    vgl_point_2d<double> p3)
{
  vgl_vector_2d<double> u=p1-p2, v=p3-p2;
  double dot=u.x()*v.x() + u.y()*v.y();
  double L2=u.sqr_length() * v.sqr_length();
  if (L2<1e-6) return 0;
  return dot/std::sqrt(L2);
}

//: Set corner[p]==true if p is on curve and arcs to p have cos<cos_thresh
//  Consider three points along the curve, p1-p2-p3.  Let a be the unit vector
//  along p2-p1 and b be the unit vector along p2-p3.  Thus a.b gives the cosine
//  of the angle at p2.  If a.b>cos_thresh this is considered a corner.
void msm_find_corners(const msm_curve& curve,
                      const std::vector<vgl_point_2d<double> >& pts,
                      double cos_thresh,
                      std::vector<bool>& corner)
{
  if (corner.size()!=pts.size())
    corner.resize(pts.size(),false);

  unsigned n=curve.size();
  unsigned n1=n-1;
  if (!curve.open()) n1=n+1;  // Closed curves consider two more points
  for (unsigned i=1;i<n1;++i)
  {
    double ca=cos_angle(pts[curve[(i-1)%n]],
                        pts[curve[i%n]],
                        pts[curve[(i+1)%n]]);
    if (ca>cos_thresh) corner[curve[i%n]]=true;
  }
}

//: Set corner[p]==true if p is on curves and arcs to p have cos<cos_thresh
//  Consider three points along the curve, p1-p2-p3.  Let a be the unit vector
//  along p2-p1 and b be the unit vector along p2-p3.  Thus a.b gives the cosine
//  of the angle at p2.  If a.b>cos_thresh this is considered a corner.
void msm_find_corners(const msm_curves& curves,
                      const std::vector<vgl_point_2d<double> >& pts,
                      double cos_thresh,
                      std::vector<bool>& corner)
{
  for (unsigned c=0;c<curves.size();++c)
    msm_find_corners(curves[c],pts,cos_thresh,corner);
}

//: Splits any curve at junctions or where curvature is high.
//  Junctions are internal points belonging to more than one curve.
//  curvature at a point is measured by the angle between two arcs going
//  from that point to its neighbours.  High curvature points are those with
//  cos(angle)>cos_thresh.
void msm_split_curves(const msm_curves& curves,
                      const std::vector<vgl_point_2d<double> >& pts,
                      double cos_thresh,
                      msm_curves& new_curves)
{
  std::vector<bool> split_pts;
  msm_get_junctions(curves,pts.size(),split_pts);
  msm_find_corners(curves,pts,cos_thresh,split_pts);
  msm_split_curves(curves,split_pts,new_curves);
}
