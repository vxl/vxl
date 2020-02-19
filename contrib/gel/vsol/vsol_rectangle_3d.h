// This is gel/vsol/vsol_rectangle_3d.h
#ifndef vsol_rectangle_3d_h_
#define vsol_rectangle_3d_h_
//*****************************************************************************
//:
// \file
// \brief Rectangle in 3D space
//
// The 3 vertices are to be defined in counterclockwise order,
// with a 90 degree corner between v1-v2 and v1-v3.
//
// \author Francois BERTEL
// \date   2000-05-08
//
// \verbatim
//  Modifications
//   2000-05-08 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004-05-14 Peter Vanroose  Added describe()
//   2004-09-06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <iostream>
#include <iosfwd>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vsol_rectangle_3d : public vsol_polygon_3d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from 3 points.
  //  `new_p0' is the origin of the rectangle. `new_p1' defines the abscissa
  //  axis and the width. `new_p2' defines the ordinate axis and the height.
  //  REQUIRE: valid_vertices(new_p0,new_p1,new_p2)
  //---------------------------------------------------------------------------
  vsol_rectangle_3d(vsol_point_3d_sptr const& new_p0,
                    vsol_point_3d_sptr const& new_p1,
                    vsol_point_3d_sptr const& new_p2);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_rectangle_3d(vsol_rectangle_3d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_rectangle_3d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d *clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p0() const;

  //---------------------------------------------------------------------------
  //: Return the second vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p1() const;

  //---------------------------------------------------------------------------
  //: Return the third vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p2() const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p3() const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_rectangle_3d const& other) const;
  inline bool operator!=(vsol_rectangle_3d const& other)const{return !operator==(other);}
  bool operator==(vsol_polygon_3d const& other) const override; // virtual of vsol_polygon_3d
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the width
  //---------------------------------------------------------------------------
  double width() const;

  //---------------------------------------------------------------------------
  //: Return the height
  //---------------------------------------------------------------------------
  double height() const;

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  double area() const override;

  //---------------------------------------------------------------------------
  //: Are `new_vertices' valid to build a rectangle ?
  //---------------------------------------------------------------------------
  bool valid_vertices(const std::vector<vsol_point_3d_sptr> new_vertices) const override;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  bool in(vsol_point_3d_sptr const& p) const override;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'. Have to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  vgl_vector_3d<double> normal_at_point(vsol_point_3d_sptr const& p) const override;

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
  std::string is_a() const override { return "vsol_rectangle_3d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vsol_polygon_3d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;
};

#endif // vsol_rectangle_3d_h_
