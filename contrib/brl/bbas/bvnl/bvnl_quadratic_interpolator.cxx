#include "bvnl_quadratic_interpolator.h"
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for fabs()

void bvnl_quadratic_interpolator::add_data_point(const double px,
                                                 const double py,
                                                 const double v)
{
  px_.push_back(px);
  py_.push_back(py);
  v_.push_back(v);
}

void bvnl_quadratic_interpolator::clear()
{
  px_.clear();
  py_.clear();
  v_.clear();
}

int bvnl_quadratic_interpolator::n_points()
{
  return px_.size();
}

void bvnl_quadratic_interpolator::fill_scatter_matrix()
{
  int n = this->n_points();
  double x4 = 0, x3y = 0, x2y2 = 0, x3=0, x2y=0, vx2=0, x2 =0;
  double xy3 = 0, xy2=0, vxy=0, xy=0;
  double y4 = 0, y3 = 0, vy2 =0, y2 = 0;
  double vx = 0, x =0;
  double vy = 0, y =0;
  double v2 =0, v =0;
  for (int i=0; i<n; i++)
  {
    double pxi = px_[i];     x   += pxi;
    double pyi = py_[i];     y   += pyi;
    double vi = v_[i];       v   += vi;
    double xi2 = pxi*pxi;    x2  += xi2;
    double xi3 = xi2*pxi;    x3  += xi3;
    double xi4 = xi3*pxi;    x4  += xi4;
    double yi2 = pyi*pyi;    y2  += yi2;
    double yi3 = yi2*pyi;    y3  += yi3;
    double yi4 = yi3*pyi;    y4  += yi4;
    double xiyi = pxi*pyi;   xy  += xiyi;
    double xi2yi = pxi*xiyi; x2y += xi2yi;
    double xi3yi = xi3*pyi;  x3y += xi3yi;
    double xi2yi2 = xi2*yi2; x2y2+= xi2yi2;
    double xiyi2 = pxi*yi2;  xy2 += xiyi2;
    double xiyi3 = pxi*yi3;  xy3 += xiyi3;
    double vixi2 = vi*xi2;   vx2 += vixi2;
    double vixiyi = vi*xiyi; vxy += vixiyi;
    double viyi2 = vi*yi2;   vy2 += viyi2;
    double vixi = vi*pxi;    vx  += vixi;
    double viyi = vi*pyi;    vy  += viyi;
    double vi2 = vi*vi;      v2  += vi2;
  }
  //solution vector is in the order
  s_ = vnl_matrix<double>(7,7);
  s_.put(0,0,x4);  s_.put(0,1,x3y); s_.put(0,2,x2y2);s_.put(0,3,x3); s_.put(0,4,x2y);s_.put(0,5,-vx2);s_.put(0,6,x2);
  s_.put(1,0,x3y); s_.put(1,1,x2y2);s_.put(1,2,xy3); s_.put(1,3,x2y);s_.put(1,4,xy2);s_.put(1,5,-vxy);s_.put(1,6,xy);
  s_.put(2,0,x2y2);s_.put(2,1,xy3); s_.put(2,2,y4);  s_.put(2,3,xy2);s_.put(2,4,y3); s_.put(2,5,-vy2);s_.put(2,6,y2);
  s_.put(3,0,x3);  s_.put(3,1,x2y); s_.put(3,2,xy2); s_.put(3,3,x2); s_.put(3,4,xy); s_.put(3,5,-vx); s_.put(3,6,x);
  s_.put(4,0,x2y); s_.put(4,1,xy2); s_.put(4,2,y3);  s_.put(4,3,xy); s_.put(4,4,y2); s_.put(4,5,-vy); s_.put(4,6,y);
  s_.put(5,0,-vx2);s_.put(5,1,-vxy);s_.put(5,2,-vy2);s_.put(5,3,-vx);s_.put(5,4,-vy);s_.put(5,5,v2);  s_.put(5,6,-v);
  s_.put(6,0,x2);  s_.put(6,1,xy);  s_.put(6,2,y2);  s_.put(6,3,x);  s_.put(6,4,y);  s_.put(6,5,-v);  s_.put(6,6,n);
}

bool bvnl_quadratic_interpolator::solve()
{
  if (this->n_points() < 6)
    return false;
  this->fill_scatter_matrix();
  vnl_svd<double> svd(s_);
  vnl_vector<double> nv = svd.nullvector();
  vcl_cout << "W: " << svd.W() << '\n'
           << "NV: " << nv << '\n';
  if (vcl_fabs(nv[5])<1e-06)
    return false;
  // NV = [Ixx Ixy Iyy Ix Iy 1 I0]
  double Ixx = nv[0]/nv[5], Ixy = nv[1]/nv[5], Iyy = nv[2]/nv[5];
  double Ix = nv[3]/nv[5], Iy = nv[4]/nv[5], I0 = nv[6]/nv[5];
  vcl_cout << "coef (" << Ixx << ' ' << Ixy << ' ' << Iyy << ' ' << Ix
           << ' ' << Iy << ' ' << I0 << ")\n";
  //solve for extremum
  double det = 4*Ixx*Iyy-Ixy*Ixy;
  //Is the system singular?
  if (vcl_fabs(det)<1e-06)
    return false;
  //Is the Det Negative?
  if (det<0)
    return false;
  //Is the Trace Negative?
  double tr = Ixx + Iyy;
  if (tr>=0)
    return false;

  //                 -           -
  // px_ext      1  | 2Iyy   -Ixy | Ix
  //        = - --- |             |
  // py_ext     det |-Ixy    2Ixx | Iy
  //                 -           -
  //  px_ext  = -1/det ( 2Iyy*Ix - Ixy*Iy)
  //  py_ext  = -1/det (-Ixy*Ix + 2Ixx*Iy)
  //
  px_ext_ = -( 2*Iyy*Ix - Ixy*Iy)/det;
  py_ext_ = -(-Ixy*Ix + 2*Ixx*Iy)/det;
  return true;
}

void bvnl_quadratic_interpolator::extremum(double& px, double& py)
{
  px = px_ext_;
  py = py_ext_;
}

void bvnl_quadratic_interpolator::print()
{
  vcl_cout << "P / V\n";
  for (int i = 0; i<this->n_points(); i++)
    vcl_cout << px_[i] << '\t' << py_[i] << '\t' << v_[i] << '\n';
  vcl_cout << vcl_flush;
}
