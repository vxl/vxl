// This is gel/vsol/vsol_polygon_3d.h
#ifndef vsol_polygon_3d_h_
#define vsol_polygon_3d_h_
//*****************************************************************************
//:
// \file
// \brief Polygon in 3D space
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
//   2001/07/03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2001/07/03 Peter Vanroose  Corrected the implementation of is_convex()
//   2004/05/14 Peter Vanroose  Added describe()
//   2004/09/06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <vsol/vsol_region_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_fwd.h> // vgl_vector_3d
class vsol_triangle_3d;
class vsol_rectangle_3d;

class vsol_polygon_3d : public vsol_region_3d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: List of vertices
  //---------------------------------------------------------------------------
  vcl_vector<vsol_point_3d_sptr> *storage_;

  //***************************************************************************
  // Initialization
  //***************************************************************************

 public:
  //---------------------------------------------------------------------------
  //: Constructor from a vcl_vector (not a geometric vector but a list of points)
  //  REQUIRE: new_vertices.size()>=3 and valid_vertices(new_vertices)
  //---------------------------------------------------------------------------
  explicit vsol_polygon_3d(vcl_vector<vsol_point_3d_sptr> const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polygon_3d(vsol_polygon_3d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_polygon_3d();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone(void) const;

  //---------------------------------------------------------------------------
  //: Safe down-casting methods
  //---------------------------------------------------------------------------
  virtual vsol_polygon_3d *cast_to_polygon(void) {return this;}
  virtual vsol_polygon_3d const* cast_to_polygon(void) const {return this;}

  virtual vsol_triangle_3d* cast_to_triangle(void) {return 0;}
  virtual const vsol_triangle_3d* cast_to_triangle(void) const {return 0;}

  virtual vsol_rectangle_3d* cast_to_rectangle(void) {return 0;}
  virtual const vsol_rectangle_3d* cast_to_rectangle(void) const {return 0;}

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return vertex `i'
  //  REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr vertex(const int i) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_polygon_3d const& other) const;
  virtual bool operator==(vsol_spatial_object_3d const& obj) const; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Has `this' not the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_polygon_3d const& o)const{return !operator==(o);}

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the region type of a polygon.  Its spatial type is a REGION
  //---------------------------------------------------------------------------
  vsol_region_3d_type region_type(void) const { return vsol_region_3d::POLYGON; }

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
  virtual double area(void) const; // virtual of vsol_region_3d

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
  //  That is: are all vertices in the same plane ?
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const vcl_vector<vsol_point_3d_sptr> new_vertices) const;

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  virtual bool in(vsol_point_3d_sptr const& p) const;

  //---------------------------------------------------------------------------
  //: Return the unit normal vector at point `p'. Have to be deleted manually
  //  REQUIRE: in(p)
  //---------------------------------------------------------------------------
  virtual vgl_vector_3d<double> normal_at_point(vsol_point_3d_sptr const& p) const;

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
  vcl_string is_a() const { return vcl_string("vsol_polygon_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(const vcl_string& cls) const { return cls==is_a(); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;

 protected:
  //---------------------------------------------------------------------------
  //: Default constructor. Do nothing. Just to enable inheritance.  Protected.
  //---------------------------------------------------------------------------
  vsol_polygon_3d();
};

//: Binary save vsol_polygon_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polygon_3d* p);

//: Binary load vsol_polygon_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polygon_3d* &p);

#endif // vsol_polygon_3d_h_
