#include <iostream>
#include <cmath>
#include "bdgl_peano_curve.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>

std::vector<vgl_point_2d<double> > rotate_2d(std::vector<vgl_point_2d<double> > in, double angle)
{
  std::vector< vgl_point_2d<double> >  pnew;
  for (auto & i : in)
  {
    double x=i.x(); double y=i.y();
    double xnew=std::cos(angle)*x+std::sin(angle)*y;
    double ynew=-std::sin(angle)*x+std::cos(angle)*y;
    pnew.emplace_back(xnew,ynew);
  }
  return pnew;
}

std::vector<vgl_point_2d<double> > trans_2d(std::vector<vgl_point_2d<double> > in, double tx, double ty)
{
  std::vector< vgl_point_2d<double> >  pnew;
  for (auto & i : in)
  {
    double x=i.x();
    double y=i.y();
    double xnew=x+tx;
    double ynew=y+ty;
    pnew.emplace_back(xnew,ynew);
  }
  return pnew;
}

std::vector<vgl_point_2d<double> > scale_2d(std::vector<vgl_point_2d<double> > in, double s)
{
  std::vector< vgl_point_2d<double> >  pnew;
  for (auto & i : in)
  {
    double x=i.x();
    double y=i.y();
    double xnew=x*s;
    double ynew=y*s;
    pnew.emplace_back(xnew,ynew);
  }
  return pnew;
}

#if 0 // commented out
std::vector<vgl_point_2d<double> > flip_y(std::vector<vgl_point_2d<double> > in)
{
  std::vector< vgl_point_2d<double> >  pnew;
  for (unsigned i=0;i<in.size();i++)
  {
    double x=in[i].x();
    double y=in[i].y();
    double xnew=-x;
    double ynew=y;
    pnew.push_back(vgl_point_2d<double>(xnew,ynew));
  }
  return pnew;
}

std::vector<vgl_point_2d<double> > flip_x(std::vector<vgl_point_2d<double> > in)
{
  std::vector< vgl_point_2d<double> >  pnew;
  for (unsigned i=0;i<in.size();i++)
  {
    double x=in[i].x();
    double y=in[i].y();
    double xnew=x;
    double ynew=-y;
    pnew.push_back(vgl_point_2d<double>(xnew,ynew));
  }
  return pnew;
}
#endif // 0

std::vector<vgl_point_2d<double> >  recurse_peano_curve(unsigned level)
{
  std::vector<vgl_point_2d<double> >  p;
  if (level==0)
  {
    p.emplace_back(0,0);
    return p;
  }
  std::vector<vgl_point_2d<double> >  p1,p2;
  p1=recurse_peano_curve(level-1);
  std::reverse(p1.begin(),p1.end());
  p2=trans_2d(rotate_2d(p1,vnl_math::pi_over_2),-1,-1);

  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  p2=trans_2d(p1,-1,1);
  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  p2=trans_2d(p1,1,1);
  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  std::reverse(p1.begin(),p1.end());
  p2=trans_2d(rotate_2d(p1,-vnl_math::pi_over_2),1,-1);
  p.insert(p.end(),p2.begin(),p2.end());

  return scale_2d(p, 0.5);
}

//: peano_curve on cube of length 2
std::vector<vgl_point_3d<double> >  peano_curve_on_cube(unsigned level)
{
  std::vector<vgl_point_2d<double> > local_peano=recurse_peano_curve(level);

  // 6 faces
  std::vector<vgl_point_3d<double> > pc;

  // high y face
  std::vector<vgl_point_3d<double> > high_y;
  high_y.reserve(local_peano.size());
for (auto & i : local_peano)
    high_y.emplace_back(i.x(),1,i.y());
  // high x face
  std::vector<vgl_point_3d<double> > high_x;
  high_x.reserve(local_peano.size());
for (auto & i : local_peano)
     high_x.emplace_back(1,-i.x(),i.y());
  // low z face
  std::vector<vgl_point_3d<double> > low_z;
  for (int i=local_peano.size()-1;i>=0;i--)
     low_z.emplace_back(local_peano[i].x(),local_peano[i].y(),-1);
  // low x face
  std::vector<vgl_point_3d<double> > low_x;
  for (int i=local_peano.size()-1;i>=0;i--)
     low_x.emplace_back(-1,local_peano[i].y(),-local_peano[i].x());
  // High z face
  std::vector<vgl_point_3d<double> > high_z;
  for (int i=local_peano.size()-1;i>=0;i--)
    high_z.emplace_back(-local_peano[i].x(),local_peano[i].y(),1);
  // Low y face
  std::vector<vgl_point_3d<double> > low_y;
  for (int i=local_peano.size()-1;i>=0;i--)
    low_y.emplace_back(local_peano[i].x(),-1,-local_peano[i].y());

  pc.insert(pc.end(),high_y.begin(),high_y.end());
  pc.insert(pc.end(),high_x.begin(),high_x.end());
  pc.insert(pc.end(),low_z.begin(),low_z.end());
  pc.insert(pc.end(),low_x.begin(),low_x.end());
  pc.insert(pc.end(),high_z.begin(),high_z.end());
  pc.insert(pc.end(),low_y.begin(),low_y.end());

  return pc;
}
