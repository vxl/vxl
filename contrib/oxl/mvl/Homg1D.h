// This is oxl/mvl/Homg1D.h
#ifndef Homg1D_h_
#define Homg1D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for 1D homogeneous primitives
//
// Homg1D is the base class for one-dimensional homogeneous primitives.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_2.h>
#include <vcl_cassert.h>

class Homg1D : public vnl_double_2 , public Homg
{
 public:
  // Constructors/Destructors--------------------------------------------------

  //: Default constructor
  Homg1D() {}

  //: Construct point $(px, pw)$ where $pw = 1$ by default.
  Homg1D(double px, double pw = 1) : vnl_double_2(px, pw) {}

  //: Construct from first 2 components of a vector.
  Homg1D(const vnl_vector<double>& v): vnl_double_2(v) {}

  //: Copy constructor
  Homg1D(const Homg1D& that) { *this=that; }

  //: Destructor
 ~Homg1D() {}

  //: Assignment
  Homg1D& operator=(const Homg1D& that) {set(that.x(),that.w()); return *this;}

  // Operations----------------------------------------------------------------

  //: Data access
  inline double x() const { return (*this)[0]; }
  //: Return reference to x
  inline double& x() { return (*this)[0]; }
  //: deprecated
  double get_x() const { return (*this)[0]; }

  //: Data access
  inline double w() const { return (*this)[1]; }
  //: Return reference to w
  inline double& w() { return (*this)[1]; }
  //: deprecated
  double get_w() const { return (*this)[1]; }

  vnl_double_2 get_vector() const { return vnl_double_2(x(),w()); }
  vnl_double_2& asVector() { return *this; }

  //: Set x,w.
  void set(double px, double pw)
  {
    (*this)[0] = px;
    (*this)[1] = pw;
  }

  //: Set from vector
  void set(const vnl_vector_fixed<double,2>& v) { set(v[0],v[1]); }

  //: Set from vector
  void set(const vnl_vector<double>& v) { set(v[0],v[1]); }

  //: Set element.
  void set(unsigned int index, double v) {assert(index<=1); (*this)[index]=v;}
};

#endif // Homg1D_h_
