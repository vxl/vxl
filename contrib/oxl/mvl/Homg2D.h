// This is oxl/mvl/Homg2D.h
#ifndef _Homg2D_h
#define _Homg2D_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Base class for 2D homogeneous features
//
// Base class for 2D homogeneous features.  This provides get/set.
//
// \author
//   Paul Beardsley, 29.03.96
//   Oxford University, UK
//
// \verbatim
//  Modifications
//   210297 AWF Switched to fixed-length vectors for speed.
//   110397 Peter Vanroose - added operator==
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_3.h>
#include <vcl_cassert.h>

class Homg2D : public vnl_double_3, public Homg
{
 public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor
  Homg2D() {}

  //: Copy constructor
  Homg2D(const Homg2D& that) : vnl_double_3(that) {}

  //: Construct a Homg2D from three doubles.
  Homg2D(double px, double py, double pw) { set(px,py,pw); }

  //: Construct from 3-vector.
  Homg2D (const vnl_vector<double>& v) { set(v); }

  //: Construct from 3-vector.
  Homg2D (const vnl_vector_fixed<double,3>& v) { set(v[0], v[1], v[2]); }

  //: Destructor
 ~Homg2D() {}

  //: Assignment
  Homg2D& operator=(const Homg2D& that) {
    vnl_double_3::operator=(that);
    return *this;
  }

  // Data Access-------------------------------------------------------------

  vnl_double_3 get_vector() const { return vnl_double_3(x(),y(),w()); }
  vnl_double_3& asVector() { return *this; }

  //: Retrieve components.  Do not attempt to write into null pointers.
  void get(double *x_ptr, double *y_ptr, double *w_ptr) const
  {
    if (x_ptr) *x_ptr = (*this)[0];
    if (y_ptr) *y_ptr = (*this)[1];
    if (w_ptr) *w_ptr = (*this)[2];
  }

// @{ ACCESS TO COMPONENTS: @}

  //: Return x
  inline double x() const { return (*this)[0]; }
  //: Return reference to x
  inline double& x() { return (*this)[0]; }

  //: Return y
  inline double y() const { return (*this)[1]; }
  //: Return reference to y
  inline double& y() { return (*this)[1]; }

  //: Return w
  inline double w() const { return (*this)[2]; }
  //: Return reference to w
  inline double& w() { return (*this)[2]; }

 private:
  //: deprecated
  double get_x() const { return (*this)[0]; }
  //: deprecated
  double get_y() const { return (*this)[1]; }
  //: deprecated
  double get_w() const { return (*this)[2]; }
 public:

  //: Set x,y,w.
  void set(double px, double py, double pw)
  {
    (*this)[0] = px;
    (*this)[1] = py;
    (*this)[2] = pw;
  }

  //: Set from vector
  void set(const vnl_vector_fixed<double,3>& v) { set(v[0],v[1],v[2]); }

  //: Set from vector
  void set(const vnl_vector<double>& v) { set(v[0],v[1],v[2]); }

  //: Set element.
  void set(unsigned int index, double v) {assert(index<=2); (*this)[index]=v;}
};

#endif // _Homg2D_h
