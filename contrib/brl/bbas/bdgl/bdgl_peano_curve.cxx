#include "bdgl_peano_curve.h"
//:
// \file

#include <vcl_cmath.h>
#include <vnl/vnl_math.h>

vcl_vector<vgl_point_2d<double> > rotate_2d(vcl_vector<vgl_point_2d<double> > in, double angle)
{
  vcl_vector< vgl_point_2d<double> >  pnew;
  for (unsigned i=0;i<in.size();i++)
  {
    double x=in[i].x(); double y=in[i].y();
    double xnew=vcl_cos(angle)*x+vcl_sin(angle)*y;
    double ynew=-vcl_sin(angle)*x+vcl_cos(angle)*y;
    pnew.push_back(vgl_point_2d<double>(xnew,ynew));
  }
  return pnew;
}

vcl_vector<vgl_point_2d<double> > trans_2d(vcl_vector<vgl_point_2d<double> > in, double tx, double ty)
{
  vcl_vector< vgl_point_2d<double> >  pnew;
  for (unsigned i=0;i<in.size();i++)
  {
    double x=in[i].x();
    double y=in[i].y();
    double xnew=x+tx;
    double ynew=y+ty;
    pnew.push_back(vgl_point_2d<double>(xnew,ynew));
  }
  return pnew;
}

vcl_vector<vgl_point_2d<double> > scale_2d(vcl_vector<vgl_point_2d<double> > in, double s)
{
  vcl_vector< vgl_point_2d<double> >  pnew;
  for (unsigned i=0;i<in.size();i++)
  {
    double x=in[i].x();
    double y=in[i].y();
    double xnew=x*s;
    double ynew=y*s;
    pnew.push_back(vgl_point_2d<double>(xnew,ynew));
  }
  return pnew;
}

#if 0 // commented out
vcl_vector<vgl_point_2d<double> > flip_y(vcl_vector<vgl_point_2d<double> > in)
{
  vcl_vector< vgl_point_2d<double> >  pnew;
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

vcl_vector<vgl_point_2d<double> > flip_x(vcl_vector<vgl_point_2d<double> > in)
{
  vcl_vector< vgl_point_2d<double> >  pnew;
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

vcl_vector<vgl_point_2d<double> >  recurse_peano_curve(unsigned level)
{
  vcl_vector<vgl_point_2d<double> >  p;
  if (level==0)
  {
    p.push_back(vgl_point_2d<double>(0,0));
    return p;
  }
  vcl_vector<vgl_point_2d<double> >  p1,p2;
  p1=recurse_peano_curve(level-1);
  vcl_reverse(p1.begin(),p1.end());
  p2=trans_2d(rotate_2d(p1,vnl_math::pi_over_2),-1,-1);

  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  p2=trans_2d(p1,-1,1);
  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  p2=trans_2d(p1,1,1);
  p.insert(p.end(),p2.begin(),p2.end());

  p1=recurse_peano_curve(level-1);
  vcl_reverse(p1.begin(),p1.end());
  p2=trans_2d(rotate_2d(p1,-vnl_math::pi_over_2),1,-1);
  p.insert(p.end(),p2.begin(),p2.end());

  return scale_2d(p, 0.5);
}

//: peano_curve on cube of length 2
vcl_vector<vgl_point_3d<double> >  peano_curve_on_cube(unsigned level)
{
  vcl_vector<vgl_point_2d<double> > local_peano=recurse_peano_curve(level);

  // 6 faces
  vcl_vector<vgl_point_3d<double> > pc;

  // high y face
  vcl_vector<vgl_point_3d<double> > high_y;
  for (unsigned i=0;i<local_peano.size();i++)
    high_y.push_back(vgl_point_3d<double>(local_peano[i].x(),1,local_peano[i].y()));
  // high x face
  vcl_vector<vgl_point_3d<double> > high_x;
  for (unsigned i=0;i<local_peano.size();i++)
     high_x.push_back(vgl_point_3d<double>(1,-local_peano[i].x(),local_peano[i].y()));
  // low z face
  vcl_vector<vgl_point_3d<double> > low_z;
  for (int i=local_peano.size()-1;i>=0;i--)
     low_z.push_back(vgl_point_3d<double>(local_peano[i].x(),local_peano[i].y(),-1));
  // low x face
  vcl_vector<vgl_point_3d<double> > low_x;
  for (int i=local_peano.size()-1;i>=0;i--)
     low_x.push_back(vgl_point_3d<double>(-1,local_peano[i].y(),-local_peano[i].x()));
  // High z face
  vcl_vector<vgl_point_3d<double> > high_z;
  for (int i=local_peano.size()-1;i>=0;i--)
    high_z.push_back(vgl_point_3d<double>(-local_peano[i].x(),local_peano[i].y(),1));
  // Low y face
  vcl_vector<vgl_point_3d<double> > low_y;
  for (int i=local_peano.size()-1;i>=0;i--)
    low_y.push_back(vgl_point_3d<double>(local_peano[i].x(),-1,-local_peano[i].y()));

  pc.insert(pc.end(),high_y.begin(),high_y.end());
  pc.insert(pc.end(),high_x.begin(),high_x.end());
  pc.insert(pc.end(),low_z.begin(),low_z.end());
  pc.insert(pc.end(),low_x.begin(),low_x.end());
  pc.insert(pc.end(),high_z.begin(),high_z.end());
  pc.insert(pc.end(),low_y.begin(),low_y.end());

  return pc;
}
