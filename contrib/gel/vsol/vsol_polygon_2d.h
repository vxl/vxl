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
// \author Francois BERTEL
// \date   2000-05-09
//
// \verbatim
//  Modifications
//   2000-05-09 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Corrected the implementation of is_convex()
//   2003-11-05 Amir Tamrakar   Added Safe casting methods
//   2004-05-01 Joseph Mundy    Added binary I/O
//   2004-05-14 Peter Vanroose  Added describe()
// \endverbatim
//*****************************************************************************

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
  std::vector<vsol_point_2d_sptr> *storage_;

  //***************************************************************************
  // Initialization
  //***************************************************************************
 public:
  //---------------------------------------------------------------------------
  //: Default constructor.
  //---------------------------------------------------------------------------
  vsol_polygon_2d(void);

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector (not a geometric vector but a list of points)
  //  REQUIRE: new_vertices.size()>=3
  //---------------------------------------------------------------------------
  explicit vsol_polygon_2d(const std::vector<vsol_point_2d_sptr> &new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polygon_2d(const vsol_polygon_2d &other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_polygon_2d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone(void) const override;

  //---------------------------------------------------------------------------
  //: Safe casting
  //---------------------------------------------------------------------------

  vsol_polygon_2d* cast_to_polygon(void) override;
  const vsol_polygon_2d* cast_to_polygon(void) const override;

  virtual vsol_triangle_2d* cast_to_triangle(void);
  virtual const vsol_triangle_2d* cast_to_triangle(void) const;

  virtual vsol_rectangle_2d* cast_to_rectangle(void);
  virtual const vsol_rectangle_2d* cast_to_rectangle(void) const;

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
  bool operator==(const vsol_spatial_object_2d& obj) const override; // virtual of vsol_spatial_object_2d

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
  vsol_region_2d_type region_type(void) const override { return vsol_region_2d::POLYGON; }

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box(void) const override;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return static_cast<int>(storage_->size()); }

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  double area(void) const override; // virtual of vsol_region_2d

  //---------------------------------------------------------------------------
  //: Return the centroid of `this'
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr centroid(void) const override;

  //---------------------------------------------------------------------------
  //: Is `this' convex ?
  //---------------------------------------------------------------------------
  bool is_convex(void) const override;

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(unsigned int i) const { return i<storage_->size(); }

  //---------------------------------------------------------------------------
  //: Are `new_vertices' valid vertices to build a polygon of the current type?
  //  All vertex sets are valid for a general polygon.
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const std::vector<vsol_point_2d_sptr> ) const;


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
  std::string is_a() const override { return std::string("vsol_polygon_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(std::string const& cls) const override
  { return cls==is_a() || vsol_region_2d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;
};

//: Binary save vsol_polygon_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polygon_2d* p);

//: Binary load vsol_polygon_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polygon_2d* &p);

#endif // vsol_polygon_2d_h_
