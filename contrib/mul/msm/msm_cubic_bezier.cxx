//:
// \file
// \brief Basic implementation of a cubic poly-bezier
// \author Tim Cootes

#include <iostream>
#include <sstream>
#include <cstdlib>
#include "msm_cubic_bezier.h"
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <cassert>
#include <vgl/io/vgl_io_point_2d.h>
#include <vgl/io/vgl_io_vector_2d.h>
#include <vnl/algo/vnl_cholesky.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return position at t (in [0,1]) given end point q
vgl_point_2d<double> msm_cubic_bezier_node::point(double t, const vgl_point_2d<double>& q) const
{
  double s=1.0-t;
  double t2=t*t,s2=s*s;
  double a=s*s2, b=3.0*t*s2, c=3.0*t2*s, d=t*t2;

  return {a*p.x() + b*c1.x() + c*c2.x() + d*q.x(),
                              a*p.y() + b*c1.y() + c*c2.y() + d*q.y()};
}

//: Return tangent to curve at t (in [0,1]) given end point q
vgl_vector_2d<double> msm_cubic_bezier_node::tangent(double t, const vgl_point_2d<double>& q) const
{
  double s = 1.0 - t;
  double t2 = t*t;
  double s2 = s*s;
  double st = s*t;

  return {
    3.0*(-s2*p.x() + (s2-2*st)*c1.x() + (2*st - t2)*c2.x() + t2*q.x()),
    3.0*(-s2*p.y() + (s2-2*st)*c1.y() + (2*st - t2)*c2.y() + t2*q.y())};
}

//: Estimate approximate length of curve by piece-wise linear curve with k extra points
double msm_cubic_bezier_node::approx_length(const vgl_point_2d<double>& q, unsigned k) const
{
  if (k==0) return (q-p).length();
  vgl_point_2d<double> p1=p,p2;
  double sum=0.0;
  double dt = 1.0/(k+1);
  for (unsigned i=1;i<=k;++i)
  {
    p2=point(i*dt,q);
    sum+=(p2-p1).length();
    p1=p2;
  }
  sum+=(q-p1).length();
  return sum;
}

// True if d is in box aligned with r of half width t*|r|. On entyr, L2=|r|^2
inline bool is_in_box(const vgl_vector_2d<double>& r, const vgl_vector_2d<double>& d, double L2, double t)
{
  // Require 0<=dot(r,d)<=L2
  double rd=r.x()*d.x() + r.y()*d.y();
  if (rd<0 || rd>L2) return false;

  double re=r.y()*d.x()-r.x()*d.y();
  return  (std::fabs(re)<=t*L2);
}

//: True if sufficiently straight, assuming q is end point.
bool msm_cubic_bezier_node::is_straight(const vgl_point_2d<double>& q, double thresh) const
{
  if (p==q) return true;
  vgl_vector_2d<double>  r = q-p;
  double L2 = r.x()*r.x() + r.y()*r.y();  // Square length p-q

  // Chech that c1 and c2 are in a narrow box around the vector r=q-p
  if (!is_in_box(r,c1-p,L2,thresh)) return false;
  return is_in_box(r,c2-p,L2,thresh);
}

//: Set to a straight line from p to q
void msm_cubic_bezier_node::set_to_line(const vgl_point_2d<double>& q)
{
  vgl_vector_2d<double> v=(q-p)/3.0;
  c1=p+v;
  c2=q-v;
}


// Default Constructor
msm_cubic_bezier::msm_cubic_bezier() = default;

//: Construct from set of points. Curve will pass through these.
msm_cubic_bezier::msm_cubic_bezier(const std::vector<vgl_point_2d<double> >&pts, bool closed)
{
  set_points(pts,closed);
}

//: Construct from set of points. Curve will pass through these.
void msm_cubic_bezier::set_points(const std::vector<vgl_point_2d<double> >&pts, bool closed)
{
  bnode_.resize(pts.size());
  for (unsigned i=0;i<pts.size();++i) bnode_[i].p=pts[i];

  closed_=closed;
  if (closed)
    smooth_closed();
  else
    smooth_open();
}

//: Return position at t (in [0,1]) in segment i of curve
vgl_point_2d<double> msm_cubic_bezier::point(unsigned i, double t) const
{
  assert(i<bnode_.size());
  return bnode_[i].point(t,bnode_[(i+1)%bnode_.size()].p);
}

//: Return tangent to curve at t (in [0,1]) in segment i of curve
vgl_vector_2d<double> msm_cubic_bezier::tangent(unsigned i, double t) const
{
  assert(i<bnode_.size());
  return bnode_[i].tangent(t,bnode_[(i+1)%bnode_.size()].p);
}

//: Compute control points so as to generate a smooth closed curve
void msm_cubic_bezier::smooth_closed()
{
  unsigned n = bnode_.size();
  if (n==1) return;

  if (n==2)
  {
    // Set to rough circle
    vgl_vector_2d<double> du = point(1)-point(0);
    vgl_vector_2d<double> dv(-du.y(),du.x());
    dv*=(2.0/3.0);
    bnode_[0].c1=point(0)+dv;
    bnode_[0].c2=point(1)+dv;
    bnode_[1].c1=point(1)-dv;
    bnode_[1].c2=point(0)-dv;
    return;
  }

  // Linear equations with a circulant symmetric matrix with kernel (1 4 1)

  // Solve this using the algorithm described in:
  // "A new method for solving symmetric circulant tridiagonal systems of linear equations"
  // O. Rojo, Com.Math Applications, Vol.20, No.12 pp.61-67 1990

  double mu=-2.0-std::sqrt(3.0);

  // Set up right hand sides to solve for control point 1
  vnl_vector<double> rx(n),ry(n);
  for (unsigned i=0;i<n;++i)
  {
    const vgl_point_2d<double>& pi=point(i);
    const vgl_point_2d<double>& pi1=point((i+1)%n);
    rx[i]=4*pi.x()+2*pi1.x();
    ry[i]=4*pi.y()+2*pi1.y();
  }

  vnl_vector<double> wx(n),wy(n),zx(n),zy(n);
  wx[0]=0.25*rx[n-1]-rx[0];
  wy[0]=0.25*ry[n-1]-ry[0];
  for (unsigned i=1;i<n-2;++i)
  {
    wx[i]=wx[i-1]/mu -rx[i];
    wy[i]=wy[i-1]/mu -ry[i];
  }
  wx[n-2]= -rx[n-2] +0.25*rx[n-1] + wx[n-3]/mu;
  wy[n-2]= -ry[n-2] +0.25*ry[n-1] + wy[n-3]/mu;

  zx[n-2]=wx[n-2]/mu;
  zy[n-2]=wy[n-2]/mu;
  for (int i=n-3;i>=0;--i)
  {
    zx[i]=(wx[i]+zx[i+1])/mu;
    zy[i]=(wy[i]+zy[i+1])/mu;
  }

  // Compute powers of 1/mu
  double rmu=1.0/mu, mu2=mu*mu;
  vnl_vector<double> u(n+1);
  u[0]=1.0;
  for (unsigned i=1;i<=n;++i) u[i]=rmu*u[i-1];

  double k1 = (1.0-u[n])*(mu2-1)*mu;
  double k2 = 1.0/k1;

  vnl_vector<double> x(n),y(n);  // For solution
  for (unsigned i=0;i<n-1;++i)
  {
    double f1=(mu2-1.0)*u[n-1-i];
    double f2=mu*u[i] + mu2*u[n-1-i];
    x[i]=zx[i]+k2*(f1*zx[0] + f2*zx[n-2]);
    y[i]=zy[i]+k2*(f1*zy[0] + f2*zy[n-2]);
  }
  x[n-1]= -0.25*(-rx[n-1]+x[0]+x[n-2]);
  y[n-1]= -0.25*(-ry[n-1]+y[0]+y[n-2]);

  // Set the nodes
  for (unsigned i=0;i<n;++i)
  {
    bnode_[i].c1.set(x[i],y[i]);
  }

  // Set c2[i] = 2p[i+1]-c1[i+1]
  for (unsigned i=0;i<n;++i)
  {
    const vgl_point_2d<double>& pi1=point((i+1)%n);
    const vgl_point_2d<double>& ci1=bnode_[(i+1)%n].c1;
    bnode_[i].c2.set(2*pi1.x()-ci1.x(),2*pi1.y()-ci1.y());
  }
}

/*
//: Compute control points so as to generate a smooth closed curve
void msm_cubic_bezier::smooth_closed()
{
  std::cout<<"Slow"<<std::endl;
  unsigned n = bnode_.size();
  if (n==1) return;


  if (n==2)
  {
    // Set to rough circle
    vgl_vector_2d<double> du = point(1)-point(0);
    vgl_vector_2d<double> dv(-du.y(),du.x());
    dv*=(2.0/3.0);
    bnode_[0].c1=point(0)+dv;
    bnode_[0].c2=point(1)+dv;
    bnode_[1].c1=point(1)-dv;
    bnode_[1].c2=point(0)-dv;
    return;
  }

  // Set up matrix for calculating solution
  // A circulant symmetric matrix with kernel (1 4 1)
  vnl_matrix<double> M(n,n,0.0);
  for (unsigned i=0;i<n;++i)
  {
    M(i,(i+n-1)%n)=1.0; M(i,i)=4.0; M(i,(i+1)%n)=1.0;
  }

  // This is a sparse, symmetric circulant matrix with all eigenvalues > 2
  // There are probably more efficient ways to solve linear equations, but this works
  vnl_cholesky chol(M);

  // Set up right hand sides to solve for control point 1
  vnl_vector<double> rx(n),ry(n);
  for (unsigned i=0;i<n;++i)
  {
    const vgl_point_2d<double>& pi=point(i);
    const vgl_point_2d<double>& pi1=point((i+1)%n);
    rx[i]=4*pi.x()+2*pi1.x();
    ry[i]=4*pi.y()+2*pi1.y();
  }

  vnl_vector<double> c1x(n),c1y(n);
  chol.solve(rx,&c1x);
  chol.solve(ry,&c1y);

  for (unsigned i=0;i<n;++i)
  {
    bnode_[i].c1.set(c1x[i],c1y[i]);
  }

  // Set c2[i] = 2p[i+1]-c1[i+1]
  for (unsigned i=0;i<n;++i)
  {
    const vgl_point_2d<double>& pi1=point((i+1)%n);
    const vgl_point_2d<double>& ci1=bnode_[(i+1)%n].c1;
    bnode_[i].c2.set(2*pi1.x()-ci1.x(),2*pi1.y()-ci1.y());
  }
}
*/

/*
//: Compute control points so as to generate a smooth open curve
void msm_cubic_bezier::smooth_open()
{
  unsigned n = bnode_.size();
  if (n==1) return;

  // Set control points on first (n-1) nodes.
  // Only the node point is used for last spline node.

  // Solve linear equation with tri-diagonal matrix of form
  // (2 1 0 ...  )
  // (1 4 1 ...  )
  // (0 1 4 1 .. )
  // ...
  // (       1 4 1  )
  // (       0 1 3.5)

  // Set up matrix for calculating solution
  vnl_matrix<double> M(n-1,n-1,0.0);
  for (unsigned i=1;i<n-2;++i)
  {
    M(i,i-1)=1.0; M(i,i)=4.0; M(i,i+1)=1.0;
  }
  M(0,0)=2; M(0,1)=1;
  M(n-2,n-3)=1; M(n-2,n-2)=3.5;

  // This is a sparse, symmetric tridiagonal matrix, hopefully well conditioned
  // There are probably more efficient ways to solve linear equations, but this works
  vnl_cholesky chol(M);

  // Set up right hand sides to solve for control point 1
  vnl_vector<double> rx(n-1),ry(n-1);
  for (unsigned i=1;i<n-2;++i)
  {
    const vgl_point_2d<double>& pi=point(i);
    const vgl_point_2d<double>& pi1=point(i+1);
    rx[i]=4*pi.x()+2*pi1.x();
    ry[i]=4*pi.y()+2*pi1.y();
  }
  rx[0] = point(0).x() + 2*point(1).x();
  ry[0] = point(0).y() + 2*point(1).y();
  rx[n-2] = 4*point(n-2).x() + 0.5*point(n-1).x();
  ry[n-2] = 4*point(n-2).y() + 0.5*point(n-1).y();

  vnl_vector<double> c1x(n-1),c1y(n-1);
  chol.solve(rx,&c1x);
  chol.solve(ry,&c1y);

  for (unsigned i=0;i<n-1;++i)
  {
    bnode_[i].c1.set(c1x[i],c1y[i]);
  }

  // Set c2[i] = 2p[i+1]-c1[i+1]
  for (unsigned i=0;i<n-2;++i)
  {
    const vgl_point_2d<double>& pi1=point(i+1);
    const vgl_point_2d<double>& ci1=bnode_[i+1].c1;
    bnode_[i].c2.set(2*pi1.x()-ci1.x(),2*pi1.y()-ci1.y());
  }

  const vgl_point_2d<double>& pn1=point(n-1);
  const vgl_point_2d<double>& cn2=bnode_[n-2].c1;
  bnode_[n-2].c2.set(0.5*(pn1.x()+cn2.x()), 0.5*(pn1.y()+cn2.y()));
}
*/

//: Compute control points so as to generate a smooth open curve
void msm_cubic_bezier::smooth_open()
{
  unsigned n = bnode_.size();
  if (n==1) return;

  if (n==2)  // Two points define a straight line
  {
    bnode_[0].set_to_line(bnode_[1].p);
    return;
  }

  // Set control points on first (n-1) nodes.
  // Only the node point is used for last spline node.

  // Solve linear equation with tri-diagonal matrix of form
  // (2 1 0 ...  )
  // (1 4 1 ...  )
  // (0 1 4 1 .. )
  // ...
  // (       1 4 1  )
  // (       0 1 3.5)
  //
  // Do this with a variant of the Thomas algorithm

  // Set up right hand sides to solve for control point 1
  vnl_vector<double> rx(n-1),ry(n-1);
  for (unsigned i=1;i<n-2;++i)
  {
    const vgl_point_2d<double>& pi=point(i);
    const vgl_point_2d<double>& pi1=point(i+1);
    rx[i]=4*pi.x()+2*pi1.x();
    ry[i]=4*pi.y()+2*pi1.y();
  }
  rx[0] = point(0).x() + 2*point(1).x();
  ry[0] = point(0).y() + 2*point(1).y();
  rx[n-2] = 4*point(n-2).x() + 0.5*point(n-1).x();
  ry[n-2] = 4*point(n-2).y() + 0.5*point(n-1).y();

  vnl_vector<double> c(n-1),dx(n-1),dy(n-1);
  c[0]=0.5; dx[0]=0.5*rx[0]; dy[0]=0.5*ry[0];
  for (unsigned int i=1; i<n-2; ++i)
    {
    c[i]=1.0/(4-c[i-1]);
    }
  c[n-2]=1.0/(3.5-c[n-3]);

  for (unsigned int i=1; i<n-1; ++i)
    {
    dx[i]=c[i]*(rx[i]-dx[i-1]);
    dy[i]=c[i]*(ry[i]-dy[i-1]);
    }

  // Set c1 for each node
  // Solution given by  x[n-1]=dx[n-2]; x[i]=dx[i]-c[i]*x[i+1]

  vgl_point_2d<double> c1(dx[n-2],dy[n-2]);
  bnode_[n-2].c1=c1;
  for (int i=n-3;i>=0;--i)
  {
    bnode_[i].c1.set(dx[i]-c[i]*c1.x(),dy[i]-c[i]*c1.y());
    c1=bnode_[i].c1;
  }

  // Set c2 for each node.
  // Set c2[i] = 2p[i+1]-c1[i+1]  for all but last node
  for (unsigned i=0;i<n-2;++i)
  {
    const vgl_point_2d<double>& pi1=point(i+1);
    const vgl_point_2d<double>& ci1=bnode_[i+1].c1;
    bnode_[i].c2.set(2*pi1.x()-ci1.x(),2*pi1.y()-ci1.y());
  }

  // Last node is a special case
  const vgl_point_2d<double>& pn1=point(n-1);
  const vgl_point_2d<double>& cn2=bnode_[n-2].c1;
  bnode_[n-2].c2.set(0.5*(pn1.x()+cn2.x()), 0.5*(pn1.y()+cn2.y()));

}

//: Create n_pts points equally spaced between start and end nodes (inclusive)
//  new_pts[0]=point(start), new_pts[n_pts-1]=point(end)
//  For closed curves, use wrap-around (so if end<start, assume it wraps round)
//  To do the integration, each curve approximated by pieces of length no more than min_len
void msm_cubic_bezier::equal_space(unsigned start, unsigned end, unsigned n_pts, double min_len,
                  std::vector<vgl_point_2d<double> >& new_pts) const
{
  if (!closed_ && end<start)
  {
    std::cerr<<"msm_cubic_bezier::equal_space() Can't have start>end for open curve."<<std::endl;
    std::abort();
  }
  unsigned n_nodes=bnode_.size();
  assert(start<n_nodes);
  assert(end<n_nodes);

  if (end<=start) end+=n_nodes;
  unsigned n=end-start;  // Number of Bezier segments

  // Split curve into multiple small fragments, to allow integration

  // Count number of points at which to evaluate each segment, each of which should be approx min_len long
  std::vector<unsigned> n_per_seg(n);
  unsigned n_total=1;
  for (unsigned i=0;i<n;++i)
  {
    double L = bnode_[(start+i)%n_nodes].approx_length(point((start+i+1)%n_nodes),5);
    n_per_seg[i] = 1+unsigned(L/min_len);
    n_total+=n_per_seg[i];
  }

  // Create lookup table such that point(index(j),t(j)) is curve length length(j) around from start
  std::vector<unsigned> index(n_total);
  std::vector<double> length(n_total), t(n_total);

  index[0]=start; t[0]= 0.0; length[0]=0.0;
  vgl_point_2d<double> p1=point(start);
  unsigned j=1;
  for (unsigned i=0;i<n;++i)
  {
    double dt=1.0/n_per_seg[i];
    for (unsigned k=1;k<n_per_seg[i];++k)
    {
      index[j]=(start+i)%n_nodes;
      t[j]=dt*k;
      vgl_point_2d<double> p2=point(index[j],t[j]);
      length[j]=length[j-1]+ (p2-p1).length();
      p1=p2;
      ++j;

    }

    // Last node (=next spline point)
    index[j]=(start+i+1)%n_nodes;
    const vgl_point_2d<double>& p2=point(index[j]);
    t[j]=0.0; length[j]=length[j-1]+ (p2-p1).length();
    p1=p2;
    ++j;
  }
  assert(j==n_total);
  double total_length = length[j-1];

  // Separation for equal spacing:
  double sep = total_length/(n_pts-1);

  new_pts.resize(n_pts);
  new_pts[0]=point(start);
  new_pts[n_pts-1]=point(end%n_nodes);

  j=0;
  for (unsigned i=1;i<n_pts-1;++i)
  {
    double target_len=sep*i;
    // Search through until find index,t wrapping target_len
    while (length[j+1]<target_len) ++j;

    // Calculate proportion along this fragment
    double L = length[j+1]-length[j];
    if (L<1e-8) L=1e-8;  // Avoid divide by zero.
    double f = (target_len-length[j])/L;
    double t2=t[j+1];  if (t2==0.0) t2=1.0;  // start of next segment
    new_pts[i] = point(index[j],(1.0-f)*t[j]+f*t2);
  }
}

//: Generate set of points along the curve, retaining control points.
//  Creates sufficient intermediate points so that their spacing is approx_sep.
// \param new_normals[i] the normal to the curve at new_pts[i]
// \param control_pt_index[i] gives element of new_pts for control point i
void msm_cubic_bezier::get_extra_points(double approx_sep,
                        std::vector<vgl_point_2d<double> >& new_pts,
                        std::vector<vgl_vector_2d<double> >& new_normals,
                        std::vector<unsigned>& control_pt_index) const
{
  // Compute number in each segment
  unsigned n_nodes = size();
  control_pt_index.resize(n_nodes);
  control_pt_index[0]=0;  // First new_pt is control point zero

  unsigned n=n_nodes;
  if (!closed_) n--;
  std::vector<unsigned> n_per_seg(n);
  unsigned n_new_pts=1;
  for (unsigned i=0;i<n;++i)
  {
    // Compute number of points for segment starting at i (not including start pt)
    double L = bnode_[i].approx_length(point((i+1)%n_nodes),5);
    n_per_seg[i]=unsigned(L/approx_sep+0.5);
    if (n_per_seg[i]<1) n_per_seg[i]=1;
    n_new_pts+=n_per_seg[i];
  }
  if (closed_) n_new_pts-=1; // Avoid double counting first point

  // == Compute intermediate points for each segment ==
  new_pts.resize(n_new_pts);
  new_normals.resize(n_new_pts);

  // Create lookup table s.t. length[i] gives dist around segment for t[i]
  unsigned n_in_seg=5;
  std::vector<double> length(n_in_seg+1),t(n_in_seg+1);
  length[0]=0.0;t[0]=0.0;
  double dt=1.0/n_in_seg;
  unsigned index=0;
  for (unsigned i=0;i<n;++i)
  {
    new_pts[index]=bnode_[i].p;
    new_normals[index]=normal(i,0);
    control_pt_index[i]=index;
    index++;

    if (n_per_seg[i]==1) continue;  // No new points in this segment

    vgl_point_2d<double> p0=bnode_[i].p;
    for (unsigned j=1;j<=n_in_seg;++j)
    {
      t[j]=t[j-1]+dt;
      vgl_point_2d<double> p1=point(i,t[j]);
      length[j]=length[j-1]+(p1-p0).length();
      p0=p1;
    }

    // Use this lookup to compute intermediate points
    double sep = length[n_in_seg]/n_per_seg[i];
    unsigned k=0;
    for (unsigned j=1;j<n_per_seg[i];++j)
    {
      double target_len = sep*j;

      // Search through until find index,t wrapping target_len
      while (length[k+1]<target_len) ++k;

      // Calculate proportion along this fragment
      double L = length[k+1]-length[k];
      if (L<1e-8) L=1e-8;  // Avoid divide by zero.
      double f = (target_len-length[k])/L;
      double t1=(1.0-f)*t[k]+f*t[k+1];
      new_pts[index] = point(i,t1);
      new_normals[index]=normal(i,t1);
      index++;
    }
  }
  if (!closed_)
  {
    // Add last node to open curve
    new_pts[index]=point(n_nodes-1);
    new_normals[index]=normal(n_nodes-2,1.0);
    control_pt_index[n_nodes-1]=index;
  }
}

//=======================================================================
// Method: print
//=======================================================================

void msm_cubic_bezier::print_summary(std::ostream& os) const
{
  if (closed_) os<<" Closed curve.";
  else         os<<" Open curve.";
  os<<" N.Nodes: "<<bnode_.size();

}

//=======================================================================
// Method: save
//=======================================================================
void msm_cubic_bezier::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,short(1)); // Version
  vsl_b_write(bfs,closed_);
  vsl_b_write(bfs,unsigned(bnode_.size()));
  for (const auto & i : bnode_)
  {
    vsl_b_write(bfs,i.p);
    vsl_b_write(bfs,i.c1);
    vsl_b_write(bfs,i.c2);
  }
}

//=======================================================================
// Method: load
//=======================================================================
void msm_cubic_bezier::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,closed_);
      vsl_b_read(bfs,n);
      bnode_.resize(n);
      for (auto & i : bnode_)
      {
        vsl_b_read(bfs,i.p);
        vsl_b_read(bfs,i.c1);
        vsl_b_read(bfs,i.c2);
      }
      break;
    default:
      std::cerr << "msm_cubic_bezier::b_read() :\n"
               << "Unexpected version number " << version << '\n';
      bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
      return;
  }
}


//=======================================================================
// Associated function: operator<<
//=======================================================================

void vsl_b_write(vsl_b_ostream& bfs, const msm_cubic_bezier& b)
{
  b.b_write(bfs);
}

//=======================================================================
// Associated function: operator>>
//=======================================================================

void vsl_b_read(vsl_b_istream& bfs, msm_cubic_bezier& b)
{
  b.b_read(bfs);
}

//=======================================================================
// Associated function: operator<<
//=======================================================================

std::ostream& operator<<(std::ostream& os,const msm_cubic_bezier& b)
{
  b.print_summary(os);
  return os;
}

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_cubic_bezier& b)
{
 os << b;
}
