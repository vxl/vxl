// This is gel/vdgl/vdgl_edgel.h
#ifndef vdgl_edgel_h
#define vdgl_edgel_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Represents a 2D image edgel
//
// \author
//    Geoff Cross

#include <vcl_iosfwd.h>
#include <vgl/vgl_point_2d.h>

class vdgl_edgel
{
 public:

  // Constructors/Destructor---------------------------------------------------
  vdgl_edgel() {}
  vdgl_edgel( const double x, const double y, const double grad= -1, const double theta= 0 );
  ~vdgl_edgel() {}

  // Operators-----------------------------------------------------------------

  vdgl_edgel& operator=(const vdgl_edgel& that);

  friend bool operator==( const vdgl_edgel &e1, const vdgl_edgel &e2);
  friend vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel& p);

  // Data Access---------------------------------------------------------------

  // getters
  inline vgl_point_2d<double> get_pt() const { return p_; }
  inline double get_x() const { return p_.x(); }
  inline double get_y() const { return p_.y(); }
  inline double get_grad() const { return grad_; }
  inline double get_theta() const { return theta_; }
  inline double x() const { return p_.x(); }
  inline double y() const { return p_.y(); }

  // setters
  inline void set_x( const double x) { p_.set(x,p_.y()); }
  inline void set_y( const double y) { p_.set(p_.x(),y); }
  inline void set_grad( const double grad) { grad_= grad; }
  inline void set_theta( const double theta) { theta_= theta; }

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------

  vgl_point_2d<double> p_;
  double grad_;
  double theta_;
};

bool operator==( const vdgl_edgel &e1, const vdgl_edgel &e2);
vcl_ostream& operator<<(vcl_ostream& s, const vdgl_edgel& p);

#endif // vdgl_edgel_h
