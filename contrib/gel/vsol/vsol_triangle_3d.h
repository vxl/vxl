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
// \author François BERTEL
// \date   2000/05/04
//
// \verbatim
//  Modifications
//   2000/05/04 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_polygon_3d.h>

class vsol_triangle_3d : public vsol_polygon_3d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from its 3 vertices
  //---------------------------------------------------------------------------
  vsol_triangle_3d(const vsol_point_3d_sptr &new_p0,
                   const vsol_point_3d_sptr &new_p1,
                   const vsol_point_3d_sptr &new_p2);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_triangle_3d(const vsol_triangle_3d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_triangle_3d();

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
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr p2(void) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_triangle_3d &other) const;
  inline bool operator!=(const vsol_triangle_3d &other)const{return !operator==(other);}
  virtual bool operator==(const vsol_polygon_3d &other) const; // virtual of vsol_polygon_3d
  virtual bool operator==(const vsol_spatial_object_3d& obj) const; // virtual of vsol_spatial_object_3d

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const;

  //***************************************************************************
  // Element change
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first vertex
  //---------------------------------------------------------------------------
  void set_p0(const vsol_point_3d_sptr &new_p0);

  //---------------------------------------------------------------------------
  //: Set the second vertex
  //---------------------------------------------------------------------------
  void set_p1(const vsol_point_3d_sptr &new_p1);

  //---------------------------------------------------------------------------
  //: Set the last vertex
  //---------------------------------------------------------------------------
  void set_p2(const vsol_point_3d_sptr &new_p2);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(const vsol_point_3d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'.
  //  Has to be deleted manually. Depends on the vertices order. If some
  //  vertices are aligned, the normal is the null vector.
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> normal_at_point(const vsol_point_3d_sptr &p) const;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;
};

#endif // vsol_triangle_3d_h_
