// This is gel/vsol/vsol_point_3d.h
#ifndef vsol_point_3d_h_
#define vsol_point_3d_h_
//*****************************************************************************
//:
// \file
// \brief Point in 3D space
//
// \author Francois BERTEL
// \date   2000-05-03
//
// \verbatim
//  Modifications
//   2000-05-03 Francois BERTEL Creation
//   2000-05-12 Francois BERTEL Replacement of vnl_vector_fixed<double,3> by vnl_double_3
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2000-09-18 Peter Tu        connected to vgl
//   2001-06-30 Peter Vanroose  Added constructor from vgl_point_3d
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2001-07-03 Peter Vanroose  Replaced new/delete by vgl_point_3d as member
//   2004-05-14 Peter Vanroose  Added describe() and operator<<(ostream)
//   2004-09-06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <iostream>
#include <vsl/vsl_binary_io.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsol_point_3d : public vsol_spatial_object_3d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Coordinates of the point
  //---------------------------------------------------------------------------
  vgl_point_3d<double> p_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from vgl_point_3d (automatic cast)
  //---------------------------------------------------------------------------
  inline vsol_point_3d(vgl_point_3d<double> const& p) : p_(p) {}

  //---------------------------------------------------------------------------
  //: Constructor from cartesian coordinates `x', `y', `z'
  //---------------------------------------------------------------------------
  inline vsol_point_3d(double x, double y, double z) : p_(x,y,z) {}

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  inline vsol_point_3d(vsol_point_3d const& pt) : vsol_spatial_object_3d(*this), p_(pt.x(),pt.y(),pt.z()) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_point_3d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone(void) const override;

  //---------------------------------------------------------------------------
  //: Safe downcasting methods
  //---------------------------------------------------------------------------
  vsol_point_3d* cast_to_point(void) override { return this;}
  vsol_point_3d const* cast_to_point() const override { return this;}

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the abscissa
  //---------------------------------------------------------------------------
  inline double x(void) const { return p_.x(); }

  //---------------------------------------------------------------------------
  //: Return the ordinate
  //---------------------------------------------------------------------------
  inline double y(void) const { return p_.y(); }

  //---------------------------------------------------------------------------
  //: Return the cote
  //---------------------------------------------------------------------------
  inline double z(void) const { return p_.z(); }

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_point_3d const& other) const;
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_point_3d const& o) const {return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a point. It is a POINT
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d_type spatial_type(void) const override;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box(void) const override;

  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the abscissa
  //---------------------------------------------------------------------------
  virtual void set_x(const double new_x);

  //---------------------------------------------------------------------------
  //: Set the ordinate
  //---------------------------------------------------------------------------
  virtual void set_y(const double new_y);

  //---------------------------------------------------------------------------
  //: Set the cote
  //---------------------------------------------------------------------------
  virtual void set_z(const double new_z);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: return the point
  //---------------------------------------------------------------------------
  vgl_point_3d<double> get_p() const { return p_; }

  //---------------------------------------------------------------------------
  //: Return the distance (N2) between `this' and `other'
  //---------------------------------------------------------------------------
  virtual double distance(vsol_point_3d const& other) const;
  virtual double distance(vsol_point_3d_sptr other) const;

  //---------------------------------------------------------------------------
  //: Return the middle point between `this' and `other'
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr middle(vsol_point_3d const& other) const;

  //---------------------------------------------------------------------------
  //: Add `v' to `this'
  //---------------------------------------------------------------------------
  virtual void add_vector(vgl_vector_3d<double> const& v);

  //---------------------------------------------------------------------------
  //: Add `v' and `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr plus_vector(vgl_vector_3d<double> const& v) const;

  //---------------------------------------------------------------------------
  //: Return the vector `this',`other'.
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> to_vector(vsol_point_3d const& other) const;

  //---------------------------------------------------------------------------
  //: Return the vgl_homg_point_3d corresponding to *this
  //---------------------------------------------------------------------------
 inline vgl_homg_point_3d<double> homg_point()
  {return vgl_homg_point_3d<double>(p_);}

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const override;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is) override;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const override { return std::string("vsol_point_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const std::string& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(std::ostream &strm, int blanking=0) const override
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << '(' << x() << ' ' << y() << ' ' << z() << ')' << std::endl;
  }
};

//: Binary save vsol_point_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, vsol_point_3d const* p);

//: Binary load vsol_point_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_point_3d* &p);

//: Stream output operator
inline std::ostream&  operator<<(std::ostream& s, vsol_point_3d const& p)
{
  return s << '(' << p.x() << ' ' << p.y() << ' ' << p.z() << ')';
}

#endif // vsol_point_3d_h_
