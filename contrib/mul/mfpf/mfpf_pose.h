#ifndef mfpf_pose_h_
#define mfpf_pose_h_
//:
// \file
// \brief Defines position and scale/orientation of an object
// \author Tim Cootes


#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//: Feature point, scale and orientation
//  Basis for object given by u(),v().
//  Thus point (i,j) in model given by p()+i*u() + j*v() in world;
class mfpf_pose
{
private:
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

  //: Default constructor
  mfpf_pose() : p_(0,0),u_(1,0) {};
};

inline bool operator==(const mfpf_pose& p1, 
                       const mfpf_pose& p2)
{
  const double tol=1e-6;
  return (vcl_fabs(p1.p().x()-p2.p().x())<tol &&
          vcl_fabs(p1.p().y()-p2.p().y())<tol &&
          vcl_fabs(p1.u().x()-p2.u().x())<tol &&
          vcl_fabs(p1.u().y()-p2.u().y())<tol);
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
  os<<"{("<<p.p().x()<<","<<p.p().y()<<") u:";
  os<<p.u().x()<<","<<p.u().y()<<")}";
  return os;
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
      vcl_cerr << "vsl_b_read(bfs,mfpf_pose): ";
      vcl_cerr << "Unexpected version number " << version << vcl_endl;
      abort();
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
      vcl_cerr << "vsl_b_read(bfs,vcl_vector<mfpf_pose>): ";
      vcl_cerr << "Unexpected version number " << version << vcl_endl;
      abort();
  }
}

#endif // mfpf_pose_h_

