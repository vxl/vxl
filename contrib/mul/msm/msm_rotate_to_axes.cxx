#include <msm/msm_rotate_to_axes.h>

//: Translate shape to origin then rotate so main axis is along angle A (radians)
// If ref_pt>=0 then ensures that points[ref_pt] is on the positive size
// when measured along direction A.
void msm_rotate_to_axes(msm_points& points, double A, int ref_pt)
{
  // Translate to origin
  vgl_point_2d<double> cog = points.cog();
  points.translate_by(-cog.x(),-cog.y());

  // Compute elements of covariance matrix
  double m11=0,m12=0,m22=0;
  for (unsigned i=0;i<points.size();++i)
  {
    vgl_point_2d<double> p = points[i];
    m11 +=p.x()*p.x();
    m12 +=p.x()*p.y();
    m22 +=p.y()*p.y();
  }

  // Compute eigenvalues and first evec:
  double dac=m11-m22;
  double d=0.5*std::sqrt(dac*dac+4*m12*m12);
  if (d<1e-6) return;  // Eigenvalues are almost the same so no main axis.

  double hac = 0.5*(m11+m22);
  double eval1=hac+d;
  double eval2=hac-d;

  vgl_vector_2d<double> evec1(m12,eval1-m11);
  double L=evec1.length();
  if (L>1e-6) evec1/=L;
  else evec1=vgl_vector_2d<double>(1,0);

  // evec1 has arbitrary direction.
  // If ref_pt defined, then arrange that points[ref_pt].x()>=0
  if (ref_pt>=0 && ref_pt<points.size())
  {
    vgl_point_2d<double> p = points[ref_pt];
    if (p.x()*evec1.x()+p.y()*evec1.y()<0) evec1 *=-1.0;
  }

  // Apply rotation
  double sA=std::sin(A),cA=std::cos(A);
  evec1=vgl_vector_2d<double>(cA*evec1.x()+sA*evec1.y(),
                              cA*evec1.y()-sA*evec1.x());
  vgl_vector_2d<double> evec2(-evec1.y(),evec1.x());


  // Rotate all points so that main axis is along angle A
  for (unsigned i=0;i<points.size();++i)
  {
    vgl_point_2d<double> p = points[i];
    points.set_point(i,p.x()*evec1.x()+p.y()*evec1.y(),
                       p.x()*evec2.x()+p.y()*evec2.y());
  }
}
