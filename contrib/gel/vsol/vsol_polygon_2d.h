// This is gel/vsol/vsol_polygon_2d.h
#ifndef vsol_polygon_2d_h_
#define vsol_polygon_2d_h_
//*****************************************************************************
//:
// \file
// \brief Polygon in 2D space
//
// The vertices are to be defined in counterclockwise order.
//
// \author François BERTEL
// \date   2000/05/09
//
// \verbatim
//  Modifications
//   2000/05/09 François BERTEL Creation
//   2000/06/17 Peter Vanroose  Implemented all operator==()s and type info
//   2001/07/03 Peter Vanroose  Corrected the implementation of is_convex()
//   2003/11/05 Amir Tamrakar   Added Safe casting methods
//   2004/05/01 Joseph Mundy    Added binary I/O
//   2004/05/14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_vector.h>
#include <vsl/vsl_binary_io.h>
class vsol_triangle_2d;
class vsol_rectangle_2d;

class vsol_polygon_2d : public vsol_region_2d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: List of vertices
  //---------------------------------------------------------------------------
  vcl_vector<vsol_point_2d_sptr> *storage_;

  //***************************************************************************
  // Initialization
  //***************************************************************************
 public:
  //---------------------------------------------------------------------------
  //: Default constructor.
  //---------------------------------------------------------------------------
  vsol_polygon_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from a vcl_vector (not a geometric vector but a list of points)
  //  REQUIRE: new_vertices.size()>=3
  //---------------------------------------------------------------------------
  explicit vsol_polygon_2d(const vcl_vector<vsol_point_2d_sptr> &new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polygon_2d(const vsol_polygon_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_polygon_2d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone(void) const;

  //---------------------------------------------------------------------------
  //: Safe casting
  //---------------------------------------------------------------------------

  virtual vsol_polygon_2d* cast_to_polygon_2d(void);
  virtual const vsol_polygon_2d* cast_to_polygon_2d(void) const;

  virtual vsol_triangle_2d* cast_to_triangle_2d(void);
  virtual const vsol_triangle_2d* cast_to_triangle_2d(void) const;

  virtual vsol_rectangle_2d* cast_to_rectangle_2d(void);
  virtual const vsol_rectangle_2d* cast_to_rectangle_2d(void) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return vertex `i'
  //  REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr vertex(const int i) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(const vsol_polygon_2d &other) const;
  virtual bool operator==(const vsol_spatial_object_2d& obj) const; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' not the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  inline bool operator!=(const vsol_polygon_2d &o)const{return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the region type of a polygon.  Its spatial type is a REGION
  //---------------------------------------------------------------------------
  vsol_region_2d_type region_type(void) const { return vsol_region_2d::POLYGON; }

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return storage_->size(); }

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  virtual double area(void) const; // virtual of vsol_region_2d

  //---------------------------------------------------------------------------
  //: Is `this' convex ?
  //---------------------------------------------------------------------------
  virtual bool is_convex(void) const;

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(unsigned int i) const { return i<storage_->size(); }

  //---------------------------------------------------------------------------
  //: Are `new_vertices' valid vertices to build a polygon of the current type?
  //  All vertex sets are valid for a general polygon.
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const vcl_vector<vsol_point_2d_sptr> ) const;


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
  vcl_string is_a() const { return vcl_string("vsol_polygon_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;
};

//: Binary save vsol_polygon_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polygon_2d* p);

//: Binary load vsol_polygon_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polygon_2d* &p);

#endif // vsol_polygon_2d_h_
