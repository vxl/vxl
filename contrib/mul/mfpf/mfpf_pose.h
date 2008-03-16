#ifndef mfpf_pose_h_
#define mfpf_pose_h_
//:
// \file
// \brief Defines position and scale/orientation of an object
// \author Tim Cootes


#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//: Feature point, scale and orientation
//  Basis for object given by u(),v().
//  Thus point (i,j) in model given by p()+i*u() + j*v() in world;
class mfpf_pose
{
  //: Position
  vgl_point_2d<double> p_;

  //: Scale and orientation defined by basis vector
  vgl_vector_2d<double> u_;
 public:
  //: Position
  const vgl_point_2d<double>& p() const { return p_; };

  //: Position
  vgl_point_2d<double>& p() { return p_; };

  //: Scale and orientation defined by basis vector
  const vgl_vector_2d<double>& u() const { return u_; }

  //: Vector of same size, orthogonal to u()
  vgl_vector_2d<double> v() const
  { return vgl_vector_2d<double>(-u_.y(),u_.x()); }

  //: Scale and orientation defined by basis vector
  vgl_vector_2d<double>& u() { return u_; }

  //: Constructor
  mfpf_pose(const vgl_point_2d<double>& p,
            const vgl_vector_2d<double>& u) : p_(p),u_(u) {};

  //: Constructor.  Defines translation (px,py), basis vector (ux,uy)
  mfpf_pose(double px, double py, double ux, double uy)
    : p_(px,py),u_(ux,uy) {};

  //: Default constructor
  mfpf_pose() : p_(0,0),u_(1,0) {};

  //: Square of scaling factor applied by this pose
  double sqr_scale() const { return u_.x()*u_.x()+u_.y()*u_.y(); }

  //: Scaling applied by this pose
  double scale() const
  { double s2 = u_.x()*u_.x()+u_.y()*u_.y();
    return s2>0.0 ? vcl_sqrt(s2) : 0.0;
  }

  //: Apply pose transformation (map from ref frame to pose)
  vgl_point_2d<double> operator()(double x, double y) const
  {  return vgl_point_2d<double>(p_.x()+x*u_.x()-y*u_.y(),
                                 p_.y()+x*u_.y()+y*u_.x()); }

  //: Apply pose transformation (map from ref frame to pose)
  vgl_point_2d<double> operator()(const vgl_point_2d<double>& q) const
  {  return vgl_point_2d<double>(p_.x()+q.x()*u_.x()-q.y()*u_.y(),
                                 p_.y()+q.x()*u_.y()+q.y()*u_.x()); }

  //: Apply inverse of pose transformation (map from pose frame -> ref)
  vgl_point_2d<double> apply_inverse(double x, double y) const
  {
    double dx=x-p_.x(), dy=y-p_.y(),s2=sqr_scale();
    return vgl_point_2d<double>((dx*u_.x()+dy*u_.y())/s2,
                                (dy*u_.x()-dx*u_.y())/s2);
  }

  //: Apply inverse of pose transformation (map from pose frame -> ref)
  vgl_point_2d<double> apply_inverse(const vgl_point_2d<double>& q) const
  {
    double dx=q.x()-p_.x(), dy=q.y()-p_.y(),s2=sqr_scale();
    return vgl_point_2d<double>((dx*u_.x()+dy*u_.y())/s2,
                                (dy*u_.x()-dx*u_.y())/s2);
  }

  //: Return pose in co-ordinates defined by this pose.
  //  I.e., pose(p) == *this(rel_pose(pose)(p))
  mfpf_pose rel_pose(const mfpf_pose& pose) const
  {
    vgl_point_2d<double> p1 = apply_inverse(pose.p());
    vgl_vector_2d<double> u1 = apply_inverse(pose.p()+pose.u())-p1;
    return mfpf_pose(p1,u1);
  }

  //: Return pose appling inverse of this pose
  mfpf_pose inverse() const
  {
    vgl_point_2d<double> p1 = apply_inverse(0,0);
    double s2=sqr_scale();
    vgl_vector_2d<double> u1(u_.x()/s2,-u_.y()/s2);
    return mfpf_pose(p1,u1);
  }
};

inline bool operator==(const mfpf_pose& p1,
                       const mfpf_pose& p2)
{
  const double tol=1e-6;
  return vcl_fabs(p1.p().x()-p2.p().x())<tol &&
         vcl_fabs(p1.p().y()-p2.p().y())<tol &&
         vcl_fabs(p1.u().x()-p2.u().x())<tol &&
         vcl_fabs(p1.u().y()-p2.u().y())<tol;
}

inline bool operator!=(const mfpf_pose& p1,
                       const mfpf_pose& p2)
{ return !(p1==p2); }

//: Return a measure of difference of position for two poses
//  Object assumed to have radius r.
//  Returns estimate of square of largest difference in
//  position for a point on the object (typically an extremal point)
inline double mfpf_max_sqr_diff(const mfpf_pose& p1,
                            const mfpf_pose& p2, double r)
{
  double dx = vcl_fabs(p1.p().x()-p2.p().x())
              + r*vcl_fabs(p1.u().x()-p2.u().x());
  double dy = vcl_fabs(p1.p().y()-p2.p().y())
              + r*vcl_fabs(p1.u().y()-p2.u().y());
  return dx*dx+dy*dy;
}

inline vcl_ostream& operator<<(vcl_ostream& os,
                               const mfpf_pose& p)
{
  os<<"{("<<p.p().x()<<','<<p.p().y()<<") u:"
    <<p.u().x()<<','<<p.u().y()<<")}";
  return os;
}

//: Treating poses as transformations, return p12=p1.p2.
//  I.e., p12(x) = p1(p2(x))
inline mfpf_pose operator*(const mfpf_pose& p1, const mfpf_pose& p2)
{
  vgl_point_2d<double> q = p1(p2.p());
  return mfpf_pose(q,p1(p2.p()+p2.u())-q);
}

inline void vsl_b_write(vsl_b_ostream& bfs,
                        const mfpf_pose& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,p.p().x());
  vsl_b_write(bfs,p.p().y());
  vsl_b_write(bfs,p.u().x());
  vsl_b_write(bfs,p.u().y());
}

inline void vsl_b_read(vsl_b_istream& bfs, mfpf_pose& p)
{
  short version;
  vsl_b_read(bfs,version);
  double ux,uy;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,p.p().x());
      vsl_b_read(bfs,p.p().y());
      vsl_b_read(bfs,ux);
      vsl_b_read(bfs,uy);
      p.u()=vgl_vector_2d<double>(ux,uy);
      break;
    default:
      vcl_cerr << "vsl_b_read(bfs,mfpf_pose): "
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
  }
}

//: Write vector of feature points to stream
inline void vsl_b_write(vsl_b_ostream& bfs,
                        const vcl_vector<mfpf_pose>& p)
{
  vsl_b_write(bfs,short(1));  // Version number
  vsl_b_write(bfs,unsigned(p.size()));
  for (unsigned i=0;i<p.size();++i)
    vsl_b_write(bfs,p[i]);
}

//: Read in vector of feature points from stream
inline void vsl_b_read(vsl_b_istream& bfs,
                       vcl_vector<mfpf_pose>& p)
{
  short version;
  vsl_b_read(bfs,version);
  unsigned n;
  switch (version)
  {
    case (1):
      vsl_b_read(bfs,n);
      p.resize(n);
      for (unsigned i=0;i<n;++i) vsl_b_read(bfs,p[i]);
      break;
    default:
      vcl_cerr << "vsl_b_read(bfs,vcl_vector<mfpf_pose>): "
               << "Unexpected version number " << version << vcl_endl;
      vcl_abort();
  }
}

#endif // mfpf_pose_h_

