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
//    210297 AWF Switched to fixed-length vectors for speed.
//    110397 Peter Vanroose - added operator==
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3.h>
#include <mvl/Homg.h>

class Homg2D : public vnl_double_3, public Homg
{
 public:

  // Constructors/Initializers/Destructors-----------------------------------

  //: Default constructor
  Homg2D () {}

  //: Copy constructor
  Homg2D (const Homg2D& that) { *this = that; }

  //: Construct a Homg2D from three doubles.
  Homg2D (double px, double py, double pw) { set(px,py,pw); }

  //: Construct from 3-vector.
  Homg2D (const vnl_vector_fixed<double,3>& v) { set(v); }

  //: Construct from 3-vector.
  Homg2D (const vnl_vector<double>& v) { set(v); }

  //: Destructor
 ~Homg2D () {}

  //: Assignment
  Homg2D& operator=(const Homg2D& that) {
    vnl_double_3::operator=(that);
    return *this;
  }

  // Data Access-------------------------------------------------------------

  const vnl_double_3& get_vector() const { return *this; }

  //: Retrieve components.
  void get (double *x_ptr, double *y_ptr, double *w_ptr) const {
    *x_ptr = (*this)[0];
    *y_ptr = (*this)[0];
    *w_ptr = (*this)[0];
  }

  //: Return x
  double get_x () const { return (*this)[0]; }

  //: Return y
  double get_y () const { return (*this)[1]; }

  //: Return w
  double get_w () const { return (*this)[2]; }

  //: Set x,y,w.
  void set (double px, double py, double pw) {
    (*this)[0] = px;
    (*this)[1] = py;
    (*this)[2] = pw;
  }

  //: Set from vector
  void set (const vnl_vector_fixed<double,3>& v) { vnl_double_3::operator = (v); }

  //: Set from vector
  void set (const vnl_vector<double>& v) { vnl_double_3::operator = (v); }

  //: Set element.
  void set (unsigned int element_index, double element) {
    (*this)[element_index] = element;
  }

  // Data Control------------------------------------------------------------

// @{ ACCESS TO COMPONENTS: @}

  //:
  double& x() { return (*this)[0]; }
  //:
  double  x() const { return (*this)[0]; }

  //:
  double& y() { return (*this)[1]; }
  //:
  double  y() const { return (*this)[1]; }

  //:
  double& w() { return (*this)[2]; }
  //:
  double  w() const { return (*this)[2]; }
};

#endif // _Homg2D_h
