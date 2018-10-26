// This is gel/vsol/vsol_triangle_3d.h
#ifndef vsol_triangle_3d_h_
#define vsol_triangle_3d_h_
//*****************************************************************************
//:
// \file
// \brief Triangle in 3D space.
//
// The vertices order gives the orientation of the triangle
//
// \author Francois BERTEL
// \date   2000-05-04
//
// \verbatim
//  Modifications
//   2000-05-04 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004-05-14 Peter Vanroose  Added describe()
//   2004-09-06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <iostream>
#include <iosfwd>
#include <vsol/vsol_polygon_3d.h>
#include <vgl/vgl_fwd.h>
#include <vcl_compiler.h>

class vsol_triangle_3d : public vsol_polygon_3d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from 3 points
  //---------------------------------------------------------------------------
  vsol_triangle_3d(vsol_point_3d_sptr const& new_p0,
                   vsol_point_3d_sptr const& new_p1,
                   vsol_point_3d_sptr const& new_p2);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_triangle_3d(vsol_triangle_3d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_triangle_3d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone(void) const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p0(void) const;

  //---------------------------------------------------------------------------
  //: Return the second vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p1(void) const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p2(void) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_triangle_3d const& other) const;
  inline bool operator!=(vsol_triangle_3d const& other)const{return !operator==(other);}
  bool operator==(vsol_polygon_3d const& other) const override; // virtual of vsol_polygon_3d
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  double area(void) const override;

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first vertex
  //---------------------------------------------------------------------------
  void set_p0(vsol_point_3d_sptr const& new_p0);

  //---------------------------------------------------------------------------
  //: Set the second vertex
  //---------------------------------------------------------------------------
  void set_p1(vsol_point_3d_sptr const& new_p1);

  //---------------------------------------------------------------------------
  //: Set the last vertex
  //---------------------------------------------------------------------------
  void set_p2(vsol_point_3d_sptr const& new_p2);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  bool in(vsol_point_3d_sptr const& p) const override;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'.
  //  Has to be deleted manually. Depends on the vertices order. If some
  //  vertices are aligned, the normal is the null vector.
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
  std::string is_a() const override { return "vsol_triangle_3d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const std::string& cls) const override
  { return cls==is_a() || vsol_polygon_3d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;
};

#endif // vsol_triangle_3d_h_
