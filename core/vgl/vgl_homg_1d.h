// This is vxl/vgl/vgl_homg_1d.h
#ifndef vgl_homg_1d_h_
#define vgl_homg_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for 1D homogeneous primitives
//
// vgl_homg_1d is the base class for one-dimensional homogeneous primitives.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Oct 96
//
// \verbatim
//  Modifications
//   23 Oct 2001 - Peter Vanroose - made templated and ported to vgl
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_homg.h>
#include <vgl/vgl_homg_point_1d.h>

template <class T>
class vgl_homg_1d : public vgl_homg<T>
{
 public:
  // Constructors/Destructors--------------------------------------------------

//: Default constructor
  vgl_homg_1d() {}

//: Construct point $(px, pw)$ where $pw = 1$ by default.
  vgl_homg_1d(T px, T pw = 1) : x_(px), w_(pw) {}

//: Copy constructor
  vgl_homg_1d(vgl_homg_1d<T> const& that) : x_(that.x()), w_(that.w()) {}

//: Constructor from a vgl_homg_point_1d
  vgl_homg_1d(vgl_homg_point_1d<T> const& that) : x_(that.x()), w_(that.w()) {}

//: Destructor
 ~vgl_homg_1d() {}

//: Equality
  bool operator==(const vgl_homg_1d<T>& that) const { return w()*that.x()==x()*that.w(); }

//: Assignment
  vgl_homg_1d& operator=(const vgl_homg_1d<T>& that) { set(that.x(), that.w()); return *this; }
  void set (T px, T pw) { x_ = px; w_ = pw; }

  T x() const { return x_; }
  T w() const { return w_; }

 protected:
  T x_;
  T w_;
};

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vgl_homg_1d<T> const& h);

#endif // vgl_homg_1d_h_
