// This is gel/vsol/vsol_triangle_2d.h
#ifndef vsol_triangle_2d_h_
#define vsol_triangle_2d_h_
//*****************************************************************************
//:
// \file
// \brief Triangle in 2D space.
//
// \author François BERTEL
// \date   2000/05/02
//
// \verbatim
//  Modifications
//   2000/05/02 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2004/05/11 Joseph Mundy Implemented binary I/O
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_iosfwd.h>


class vsol_triangle_2d : public vsol_polygon_2d
{
 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor - needed for binary I/O
  //---------------------------------------------------------------------------
  vsol_triangle_2d();

  //---------------------------------------------------------------------------
  //: Constructor from 3 points
  //---------------------------------------------------------------------------
  vsol_triangle_2d(const vsol_point_2d_sptr &new_p0,
                   const vsol_point_2d_sptr &new_p1,
                   const vsol_point_2d_sptr &new_p2);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_triangle_2d(const vsol_triangle_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_triangle_2d();

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
  //: Return the last vertex
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p2(void) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_triangle_2d &other) const;
  inline bool operator!=(const vsol_triangle_2d &other)const{return !operator==(other);}
  virtual bool operator==(const vsol_polygon_2d &other) const; // virtual of vsol_polygon_2d
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

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
  void set_p0(const vsol_point_2d_sptr &new_p0);

  //---------------------------------------------------------------------------
  //: Set the second vertex
  //---------------------------------------------------------------------------
  void set_p1(const vsol_point_2d_sptr &new_p1);

  //---------------------------------------------------------------------------
  //: Set the last vertex
  //---------------------------------------------------------------------------
  void set_p2(const vsol_point_2d_sptr &new_p2);

  virtual vsol_triangle_2d* cast_to_triangle(void) { return this; }
  virtual vsol_triangle_2d const* cast_to_triangle(void) const { return this; }

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
  virtual vcl_string is_a() const { return "vsol_triangle_2d"; }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const
  { return cls==is_a() || vsol_polygon_2d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;
};

//: Binary save vsol_triangle_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_triangle_2d* p);

//: Binary load vsol_triangle_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_triangle_2d* &p);

#endif // vsol_triangle_2d_h_
