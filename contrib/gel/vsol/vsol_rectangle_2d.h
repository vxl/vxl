// This is gel/vsol/vsol_rectangle_2d.h
#ifndef vsol_rectangle_2d_h_
#define vsol_rectangle_2d_h_
//*****************************************************************************
//:
// \file
// \brief Rectangle in 2D space
//
//  The "width/abcissa" (long axis) of the rectangle is along p0-p1, p2-p3,
//  the "height/ordinate" (short axis)  of the rectangle is along p1-p2, p3-p0.
//
// Note, a constructor from 3 vertices is also defined
// The 3 vertices are to be defined in counterclockwise order,
// with a 90 degree corner between p_c-p_abs and p_c-p_ord.
//
//              p3            p_ord        p2
//               o ------------o------------o
//               |             |            |
//               |             |            |
//               |             o------------o p_abs
//               |            p_c           |
//               |                          |
//               o -------------------------o
//               p0                         p1
//
// \author François BERTEL
// \date   2000/05/08
//
// \verbatim
//  Modifications
//   2000/05/08 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2001/07/03 Peter Vanroose  Replaced vnl_double_2 by vgl_vector_2d
//   2004/05/11 Joseph Mundy    Changed internal representation to 4 verts,
//                              which allows consistent polygon operations.
//                              3 vert constructor is maintained
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_iosfwd.h>

class vsol_rectangle_2d : public vsol_polygon_2d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor - needed for binary I/O
  //---------------------------------------------------------------------------
  vsol_rectangle_2d();
  
  //---------------------------------------------------------------------------
  //: Constructor from 4 points, the corners of the rectangle
  //---------------------------------------------------------------------------
  vsol_rectangle_2d(const vsol_point_2d_sptr &new_p0,
                    const vsol_point_2d_sptr &new_p1,
                    const vsol_point_2d_sptr &new_p2,
                    const vsol_point_2d_sptr &new_p3);
    

  //---------------------------------------------------------------------------
  //: Constructor from 3 points.
  //  `new_pc' is the origin of the rectangle. `new_pabs' defines the abscissa
  //  axis and the width/2. `new_pord' defines the ordinate axis and the 
  //  height/2.
  //  REQUIRE: valid_vertices(new_pc,new_pabs,new_pord)  
  //---------------------------------------------------------------------------
  vsol_rectangle_2d(const vsol_point_2d_sptr &new_pc,
                    const vsol_point_2d_sptr &new_pabs,
                    const vsol_point_2d_sptr &new_pord);


  //---------------------------------------------------------------------------
  //: Constructor from center, half_width, half_height,
  //  angle(ccw from x axis, in deg/rad)
  //---------------------------------------------------------------------------
  vsol_rectangle_2d(const vsol_point_2d_sptr &center,
                    const double half_width,
                    const double half_height,
                    const double angle,
                    const bool deg = true);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_rectangle_2d(const vsol_rectangle_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_rectangle_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0(void) const;

  //---------------------------------------------------------------------------
  //: Return the second vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1(void) const;

  //---------------------------------------------------------------------------
  //: Return the third vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p2(void) const;

  //---------------------------------------------------------------------------
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p3(void) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_rectangle_2d &other) const;
  inline bool operator!=(const vsol_rectangle_2d &other)const{return !operator==(other);}
  virtual bool operator==(const vsol_polygon_2d &other) const; // virtual of vsol_polygon_2d
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

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

  virtual vsol_rectangle_2d* cast_to_rectangle_2d(void) { return this; }
  virtual vsol_rectangle_2d const* cast_to_rectangle_2d(void) const { return this; }
  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "vsol_rectangle_2d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const;

  //---------------------------------------------------------------------------
  //: Are `new_vertices' valid to build a rectangle ?
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const vcl_vector<vsol_point_2d_sptr> new_vertices) const;

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;
};


//: Binary save vsol_rectangle_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_rectangle_2d* r);

//: Binary load vsol_rectangle_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_rectangle_2d* &r);

#endif // vsol_rectangle_2d_h_
