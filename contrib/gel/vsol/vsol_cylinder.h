#ifndef vsol_cylinder_h_
#define vsol_cylinder_h_
//*****************************************************************************
//:
// \file
// \brief a class to hold cylinder data in 3D
//
// This class inherits from vsol_spatial_object_3d.
//
// \author Gamze D. Tunali
// \date   2005-11-07
//
// \verbatim
//  Modifications
//   2004-10-07 Gamze D. Tunali  Created
//   2006-04-04 K. Kang: a few setters are added
// \endverbatim
//*****************************************************************************

#include <string>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_cylinder.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vsol/vsol_cylinder_sptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsol_cylinder : public  vsol_spatial_object_3d
{
  vgl_cylinder<double> cyl_;

 public:
  vsol_cylinder() = default;

  vsol_cylinder(vgl_point_3d<double> centre, double radius, double length)
  : cyl_(centre, radius, length) {}

  vsol_cylinder(vgl_point_3d<double> centre, double radius, double length,
                vgl_vector_3d<double> orient)
  : cyl_(centre, radius, length){ cyl_.set_orientation(orient); }

  // copy constructor
  vsol_cylinder(vsol_cylinder const& cyl) = default;

  //: Return true iff the point p is inside (or on) this cylinder
  bool contains(vgl_point_3d<double> const& p);

  //: getters
  vgl_point_3d<double> center() { return cyl_.center(); }
  double radius() {return cyl_.radius(); }
  double length() {return cyl_.length(); }
  vgl_vector_3d<double> orientation() { return cyl_.orientation(); }

  //: setters
  void set_orientation(vgl_vector_3d<double> orient) {cyl_.set_orientation(orient);
  }

  void set_center(vgl_point_3d<double> const & c) {cyl_.set_center(c);}

  void set_length(double l) {cyl_.set_length(l);}

  void set_radius(double r) {cyl_.set_radius(r);}
  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  void describe(std::ostream &strm, int blanking) const override;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_cylinder"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const { return cls == is_a(); }

   // implementing virtual methods of vsol_spatial_object_3d
  vsol_spatial_object_3d_type spatial_type() const override { return vsol_spatial_object_3d::VOLUME; }

  vsol_spatial_object_3d* clone() const override { return new vsol_cylinder(*this); }

  ~vsol_cylinder(void) override = default;
};

//: Binary save vsol_cylinder* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_cylinder* p);

//: Binary load vsol_cylinder_sptr from stream.
void vsl_b_write(vsl_b_istream &is, vsol_cylinder_sptr &p);

//: Binary load vsol_cylinder* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_cylinder* &p);

//: Binary load vsol_cylinder_sptr from stream.
void vsl_b_read(vsl_b_istream &is, vsol_cylinder_sptr &p);

#endif // vsol_cylinder_h_
