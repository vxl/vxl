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
    *x_ptr = data[0];
    *y_ptr = data[1];
    *w_ptr = data[2];
  }

  //: Return x
  double get_x () const { return data[0]; }

  //: Return y
  double get_y () const { return data[1]; }

  //: Return w
  double get_w () const { return data[2]; }

  //: Set x,y,w.
  void set (double px, double py, double pw) {
    data[0] = px;
    data[1] = py;
    data[2] = pw;
  }

  //: Set from vector
  void set (const vnl_vector<double>& v) { vnl_double_3::operator = (v); }

  //: Set element.
  void set (unsigned int element_index, double element) {
    data[element_index] = element;
  }

  // Data Control------------------------------------------------------------

// @{ ACCESS TO COMPONENTS: @}

  //:
  double& x() { return data[0]; }
  //:
  double  x() const { return data[0]; }

  //:
  double& y() { return data[1]; }
  //:
  double  y() const { return data[1]; }

  //:
  double& w() { return data[2]; }
  //:
  double  w() const { return data[2]; }
};

#endif // _Homg2D_h
