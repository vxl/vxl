#ifndef vsol_rectangle_3d_h
#define vsol_rectangle_3d_h
//*****************************************************************************
//:
//  \file
// \brief Rectangle of a 3D space
//
// The vertices are defined in the counterclockwise.
//
// \author
// François BERTEL
//
// \verbatim
// Modifications
// 2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
// 2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
// 2000/05/08 François BERTEL Creation
// \endverbatim
//*****************************************************************************

class vsol_rectangle_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_rectangle_3d_sptr.h>
#include <vsol/vsol_polygon_3d.h>

#include <vsol/vsol_point_3d_sptr.h>

class vsol_rectangle_3d
  :public vsol_polygon_3d
{
  //***************************************************************************
  // Initialization
  //***************************************************************************
public:
  //---------------------------------------------------------------------------
  //: Constructor from 3 points.
  //  `new_p0' is the origin of the rectangle. `new_p1' defines the abscissa
  //  axis and the width. `new_p2' defines the ordinate axis and the height.
  //  REQUIRE: valid_vertices(new_p0,new_p1,new_p2)
  //---------------------------------------------------------------------------
  explicit vsol_rectangle_3d(const vsol_point_3d_sptr &new_p0,
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
  virtual vsol_point_3d_sptr p0(void) const;

  //---------------------------------------------------------------------------
  //: Return the second vertex
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr p1(void) const;

  //---------------------------------------------------------------------------
  //: Return the third vertex
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr p2(void) const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  virtual vsol_point_3d_sptr p3(void) const;

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
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void);

  //---------------------------------------------------------------------------
  //: Return the width
  //---------------------------------------------------------------------------
  virtual double width(void) const;

  //---------------------------------------------------------------------------
  //: Return the height
  //---------------------------------------------------------------------------
  virtual double height(void) const;

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
  virtual vgl_vector_3d<double>
  normal_at_point(const vsol_point_3d_sptr &p) const;
};

#endif // vsol_rectangle_3d_h
