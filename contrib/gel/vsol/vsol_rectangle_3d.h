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
// \author François BERTEL
// \date   2000/05/08
//
// \verbatim
//  Modifications
//   2000/05/08 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

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
  vsol_rectangle_3d(const vsol_point_3d_sptr &new_p0,
                    const vsol_point_3d_sptr &new_p1,
                    const vsol_point_3d_sptr &new_p2);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_rectangle_3d(const vsol_rectangle_3d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_rectangle_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_sptr clone(void) const;

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
  //: Return the third vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p2(void) const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p3(void) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_rectangle_3d &other) const;
  inline bool operator!=(const vsol_rectangle_3d &other)const{return !operator==(other);}
  virtual bool operator==(const vsol_polygon_3d &other) const; // virtual of vsol_polygon_3d
  virtual bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the width
  //---------------------------------------------------------------------------
  double width(void) const;

  //---------------------------------------------------------------------------
  //: Return the height
  //---------------------------------------------------------------------------
  double height(void) const;

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const;

  //---------------------------------------------------------------------------
  //: Are `new_vertices' valid to build a rectangle ?
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const vcl_vector<vsol_point_3d_sptr> new_vertices) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(const vsol_point_3d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'. Have to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> normal_at_point(const vsol_point_3d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;
};

#endif // vsol_rectangle_3d_h_
