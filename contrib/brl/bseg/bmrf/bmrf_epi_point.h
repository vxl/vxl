// This is brl/bseg/bmrf/bmrf_epi_point.h
#ifndef bmrf_epi_point_h_
#define bmrf_epi_point_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a point class to hold edgel and other pertinent info
//
//  An epi_point is created by intersecting an epipolar line with
//  an edgel curve.  The purpose of the epi_point is to hold attributes
//  for point matching across video frames.
//
// \author
//  J.L. Mundy - December 26, 2003
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>

class bmrf_epi_point:  public vbl_ref_count
{
 public:
  bmrf_epi_point(vgl_point_2d<double> &p,
                 const double alpha,
                 const double s,
                 const double grad_mag,
                 const double grad_ang,
                 const double tan_ang);

  bmrf_epi_point(const double x, const double y,
                 const double alpha,
                 const double s,
                 const double grad_mag,
                 const double grad_ang,
                 const double tan_ang);

  bmrf_epi_point();
  virtual ~bmrf_epi_point() {}

  //: accessors
  void set(const double x, const double y){p_.set(x,y);}
  void set_alpha(const double alpha){alpha_ = alpha;}
  void set_s(const double s){s_ = s;}
  void set_grad_mag(const double grad_mag){grad_mag_= grad_mag;}
  void set_grad_ang(const double grad_ang){grad_ang_= grad_ang;}
  void set_tan_ang(const double tan_ang){tan_ang_= tan_ang;}
  vgl_point_2d<double> p() const {return p_;}
  double x() const {return p_.x();}
  double y() const {return p_.y();}
  double alpha() const {return alpha_;}
  double s() const{return s_;}
  double grad_mag() const {return grad_mag_;}
  double grad_ang() const {return grad_ang_;}
  double tan_ang() const {return tan_ang_;}

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  vcl_string is_a() const;

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const;

 protected:
  vgl_point_2d<double> p_;
  double alpha_; //the epipolar line parameter
  double s_; //distance along the epipolar line
  double grad_mag_; //gradient magnitude
  double grad_ang_; //gradient angle
  double tan_ang_;  //curve tangent at point
};
vcl_ostream&  operator<<(vcl_ostream& s, bmrf_epi_point const& ep);

#include <bmrf/bmrf_epi_point_sptr.h>
//: Binary save bmrf_node* to stream.
void vsl_b_write(vsl_b_ostream &os, bmrf_epi_point_sptr const& n);

//: Binary load bmrf_node* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_epi_point_sptr &n);

#endif // bmrf_epi_point_h_
