// This is gel/vsol/vsol_point_3d.h
#ifndef vsol_point_3d_h_
#define vsol_point_3d_h_
//*****************************************************************************
//:
// \file
// \brief Point in 3D space
//
// \author François BERTEL
// \date   2000/05/03
//
// \verbatim
//  Modifications
//   2000/05/03 François BERTEL Creation
//   2000/05/12 François BERTEL Replacement of vnl_vector_fixed<double,3> by vnl_double_3
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2000/09/18 Peter Tu        connected to vgl
//   2001/06/30 Peter Vanroose  Added constructor from vgl_point_3d
//   2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2001/07/03 Peter Vanroose  Replaced new/delete by vgl_point_3d as member
//   2004/05/14 Peter Vanroose  Added describe() and operator<<(ostream)
// \endverbatim
//*****************************************************************************

class vsol_point_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_spatial_object_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vcl_iostream.h>

class vsol_point_3d : public vsol_spatial_object_3d
{
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //Description: Coordinates of the point
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
  //: Constructor from cartesian coordinates `new_x', `new_y', `new_z'
  //---------------------------------------------------------------------------
  inline vsol_point_3d(double x, double y, double z) : p_(x,y,z) {}

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  inline vsol_point_3d(const vsol_point_3d &pt) : p_(pt.x(),pt.y(),pt.z()) {}

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_point_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_sptr clone(void) const;

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
  virtual bool operator==(const vsol_point_3d &other) const;
  virtual bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Has `this' not the same coordinates than `other' ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_point_3d &o) const {return !operator==(o);}

  //---------------------------------------------------------------------------
  //: Safe downcasting methods
  //---------------------------------------------------------------------------
  virtual vsol_point_3d* cast_to_point(void) { return this;}
  virtual const vsol_point_3d* cast_to_point() const { return this;}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the real type of a point. It is a POINT
  //---------------------------------------------------------------------------
  enum vsol_spatial_object_3d_type spatial_type(void) const;

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

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
  //: Return the distance (N2) between `this' and `other'
  //---------------------------------------------------------------------------
  virtual double distance(const vsol_point_3d &other) const;
  virtual double distance(vsol_point_3d_sptr other) const;

  //---------------------------------------------------------------------------
  //: Return the middle point between `this' and `other'
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr middle(const vsol_point_3d &other) const;

  //---------------------------------------------------------------------------
  //: Add `v' to `this'
  //---------------------------------------------------------------------------
  virtual void add_vector(const vgl_vector_3d<double> &v);

  //---------------------------------------------------------------------------
  //: Add `v' and `this'
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr plus_vector(const vgl_vector_3d<double> &v) const;

  //---------------------------------------------------------------------------
  //: Return the vector `this',`other'.
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> to_vector(const vsol_point_3d &other) const;

  //---------------------------------------------------------------------------
  //: Return the vgl_homg_point_3d corresponding to *this
  //---------------------------------------------------------------------------
 inline vgl_homg_point_3d<double> homg_point()
  {return vgl_homg_point_3d<double>(p_);}

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  inline void describe(vcl_ostream &strm, int blanking=0) const
  {
    if (blanking < 0) blanking = 0; while (blanking--) strm << ' ';
    strm << '(' << x() << ' ' << y() << ' ' << z() << ')' << vcl_endl;
  }
};

//: Stream output operator
inline vcl_ostream&  operator<<(vcl_ostream& s, vsol_point_3d const& p)
{
  return s << '(' << p.x() << ' ' << p.y() << ' ' << p.z() << ')';
}

#endif // vsol_point_3d_h_
