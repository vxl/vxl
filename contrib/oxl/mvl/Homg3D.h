// This is oxl/mvl/Homg3D.h
#ifndef Homg3D_h_
#define Homg3D_h_
//:
// \file
// \brief Base class for 3D homogeneous features
//
// Base class for 3D homogeneous features.
//
// \author
//   Paul Beardsley, 29.03.96
//   Oxford University, UK
//
// \verbatim
//  Modifications
//   210297 AWF Switched to fixed-length vectors for speed.
//   110397 Peter Vanroose - added operator==
//   100904 Peter Vanroose - Inlined all 1-line methods in class decl
// \endverbatim
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_vector.h>
#include <vcl_cassert.h>

class Homg3D : public Homg
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  Homg3D() {}
  Homg3D(const Homg3D& that):homg_vector_(that.homg_vector_) {}
  Homg3D(double px, double py, double pz, double pw = 1): homg_vector_(px,py,pz,pw) {}
  Homg3D(const vnl_vector<double>& v): homg_vector_(v) { }
  Homg3D(const vnl_vector_fixed<double,4>& v): homg_vector_(v) { }
 ~Homg3D() {}

  Homg3D& operator=(const Homg3D& that) { homg_vector_ = that.homg_vector_; return *this; }
  // default ok. nope, egcs chokes

  // Data Access---------------------------------------------------------------

  vnl_double_4 get_vector() const { return vnl_double_4(x(),y(),z(),w()); }
  vnl_double_4& asVector() { return homg_vector_; }

  // get x,y,z,w.  Do not try to fill null pointers.
  void get(double *x_ptr, double *y_ptr, double *z_ptr, double *w_ptr) const
  {
    if (x_ptr) *x_ptr = x();
    if (x_ptr) *y_ptr = y();
    if (x_ptr) *z_ptr = z();
    if (x_ptr) *w_ptr = w();
  }

  //: Get x.
  inline double x() const { return homg_vector_[0]; }

  //: Get y.
  inline double y() const { return homg_vector_[1]; }

  //: Get z.
  inline double z() const { return homg_vector_[2]; }

  //: Get w.
  inline double w() const { return homg_vector_[3]; }

 private:
  //: deprecated
  double get_x() const { return homg_vector_[0]; }
  //: deprecated
  double get_y() const { return homg_vector_[1]; }
  //: deprecated
  double get_z() const { return homg_vector_[2]; }
  //: deprecated
  double get_w() const { return homg_vector_[3]; }
 public:

  //: Set x,y,z,w.
  void set(double px, double py, double pz, double pw = 1) {
    homg_vector_[0] = px;
    homg_vector_[1] = py;
    homg_vector_[2] = pz;
    homg_vector_[3] = pw;
  }

  //: Set from vector
  void set(const vnl_vector_fixed<double,4>& v) { set(v[0],v[1],v[2],v[3]); }

  //: Set from vector
  void set(const vnl_vector<double>& v) { set(v[0],v[1],v[2],v[3]); }

  //: Set element.
  void set(unsigned int idx, double v) { assert(idx<=3); homg_vector_[idx]=v; }

  // Utility Methods-----------------------------------------------------------
  bool operator==(Homg3D const& p) const { return homg_vector_ == p.get_vector(); }

  // INTERNALS-----------------------------------------------------------------
 protected:
  // Data Members--------------------------------------------------------------
  vnl_double_4 homg_vector_;
};

#endif // Homg3D_h_
