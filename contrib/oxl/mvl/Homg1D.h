// This is oxl/mvl/Homg1D.h
#ifndef Homg1D_h_
#define Homg1D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief Base class for 1D homogeneous primitives
//
// Homg1D is the base class for one-dimensional homogeneous primitives.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//-----------------------------------------------------------------------------

#include <mvl/Homg.h>
#include <vnl/vnl_double_2.h>

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
  Homg1D& operator=(const Homg1D& that) { vnl_double_2::operator=(that); return *this; }
  void set (double px, double pw) { data[0] = px; data[1] = pw; }

  // Operations----------------------------------------------------------------

//: Data access
  double get_x() const { return (*this)[0]; }
//: Data access
  double get_w() const { return (*this)[1]; }

  const vnl_double_2& get_vector() const { return *this; }
};

#endif // Homg1D_h_
