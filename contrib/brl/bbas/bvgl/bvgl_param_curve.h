// This is bbas/bvgl/bvgl_param_curve.h
#ifndef bvgl_param_curve_h_
#define bvgl_param_curve_h_
//:
// \file
// \brief A parametric curve that can be sampled via a parametric equation.
// \author Ozge Can Ozcanli (ozge@lems.brown.edu)
// \date 02/09/05
//
// \verbatim
// Modifications
// Aug 8 2005  Ricardo Fabbri: Changed convention of parametric query methods
//
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <typeinfo>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>


//: A 2D parametric curve
//
//  Parametric curves are required to represent continuous curves
//  and they enable sampling and calculation of basic differential
//  geometries.
//
//  bvgl_param_curve is the base class, various parametric curves
//  such as arcs, euler spirals, and polynomials shall derive from this base class.
//
//  A parametric curve has a start and an endpoint. It also enables parametric
//  access by at least the following methods:
//
//    point_at_length(s), with s in [0,length()], with 0 corresponding to starting
//    point, and length() to endpoint. This can be numerically demanding for certain
//    curves.
//
//    point_at(s), with s in [0,1],  with 0 corresponding to starting point, and 1 to
//    endpoint. Note that this is NOT normalized arclength, i.e., point_at(0.5)
//    may or MAY NOT be the point at arclength 0.5*length() from starting point.
//
//  Each derived curve could have a natural parameter which is not arclength. In
//  that case, e.g. for sake of efficiency, that class could _also_ define a
//  "point_at_xx" method where "xx" is a parameter name.
//
//  The same convention holds for tangent_at(s), curvature_at(s), and all other
//  parametric functions.
//
//  For example, to recover the starting point, use point_at(0). To recover the
//  endpoint, use point_at(1) or point_at_length(length()).
//
class bvgl_param_curve
{
 public:

  bvgl_param_curve() = default;

  //: copy constructor
  //bvgl_param_curve(const bvgl_param_curve()& that )
  //  : vbl_ref_count()
  //{    }   //suppress copying of reference count between objects

  virtual ~bvgl_param_curve() = default;

  static const std::type_info& type_id()
  { return typeid(bvgl_param_curve); }

  virtual bool is_type( const std::type_info& type ) const
  { return (typeid(bvgl_param_curve) == type)!=0; }

  //: comparison operator.
  //  Comparison is on the curve, two parametric curves are identical if their
  //  equations are equivalent
  virtual
  bool operator==(bvgl_param_curve const& c) const {return this == &c; }

  //: Write "<dbvgl_param_curve> to stream"
  // \relates dbvgl_param_curve
  //virtual
  //std::ostream&  operator<<(std::ostream& s);

  // Elementary geometric functions ----------------------------------

  //: length of parametric curve from start point to end point
  virtual double length() const = 0;

  //: Get sample point at value s of a parameter along the curve, s within [0,1]
  virtual vgl_point_2d<double> point_at(double s) const = 0;

  //: Get sample point at arclength s away from starting point
  virtual vgl_point_2d<double> point_at_length(double s) const = 0;

  //: Get tangent of the point at parameter s within [0,1]
  virtual vgl_vector_2d<double> tangent_at(double s) const = 0;

  //: Get tangent of the point at arclength s away from starting point
  virtual vgl_vector_2d<double> tangent_at_length(double s) const = 0;

  //: Gets tangent angle (in radian) in [0, 2PI) at parameter s within [0,1]
  virtual double tangent_angle_at(double s) const = 0;

  //: Gets tangent angle (in radian) in [0, 2PI)  at arclength s away
  // from the starting point
  virtual double tangent_angle_at_length(double s) const = 0;

  //: Get curvature of the point at s within [0,1]
  virtual double curvature_at(double s) const = 0;

  //: Get curvature of the point at s arclength away from starting point.
  virtual double curvature_at_length(double s) const = 0;

  virtual bvgl_param_curve& operator=( bvgl_param_curve const& ) = default;

  // This will be an abstract function...
  virtual bvgl_param_curve *clone() const = 0;// { return 0; }
};

//: Read parameters from stream
// \relates dbvgl_param_curve
//std::istream&  operator>>(std::istream& s, bvgl_param_curve const& c) const = 0;

#endif // bvgl_param_curve_h_
