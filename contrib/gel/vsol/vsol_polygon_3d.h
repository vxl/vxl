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
// \author Francois BERTEL
// \date   2000-05-09
//
// \verbatim
//  Modifications
//   2000-05-09 Francois BERTEL Creation
//   2000-06-17 Peter Vanroose  Implemented all operator==()s and type info
//   2001-07-03 Peter Vanroose  Replaced vnl_double_3 by vgl_vector_3d
//   2001-07-03 Peter Vanroose  Corrected the implementation of is_convex()
//   2004-05-14 Peter Vanroose  Added describe()
//   2004-09-06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vsol/vsol_region_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_fwd.h> // vgl_vector_3d
#include <vgl/vgl_homg_plane_3d.h>
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
  std::vector<vsol_point_3d_sptr> *storage_;

  //***************************************************************************
  // Initialization
  //***************************************************************************

 public:
  //---------------------------------------------------------------------------
  //: Default constructor. Do nothing.
  //---------------------------------------------------------------------------
  vsol_polygon_3d(void);

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector (not a geometric vector but a list of points)
  //  REQUIRE: new_vertices.size()>=3 and valid_vertices(new_vertices)
  //---------------------------------------------------------------------------
  explicit vsol_polygon_3d(std::vector<vsol_point_3d_sptr> const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polygon_3d(vsol_polygon_3d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_polygon_3d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone(void) const override;

  //---------------------------------------------------------------------------
  //: Safe down-casting methods
  //---------------------------------------------------------------------------
  vsol_polygon_3d *cast_to_polygon(void) override {return this;}
  vsol_polygon_3d const* cast_to_polygon(void) const override {return this;}

  virtual vsol_triangle_3d* cast_to_triangle(void) {return nullptr;}
  virtual const vsol_triangle_3d* cast_to_triangle(void) const {return nullptr;}

  virtual vsol_rectangle_3d* cast_to_rectangle(void) {return nullptr;}
  virtual const vsol_rectangle_3d* cast_to_rectangle(void) const {return nullptr;}

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
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

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
  vsol_region_3d_type region_type(void) const override { return vsol_region_3d::POLYGON; }

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box(void) const override;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return storage_->size(); }

  //---------------------------------------------------------------------------
  //: Return the area of `this'
  //---------------------------------------------------------------------------
  double area(void) const override; // virtual of vsol_region_3d

  //---------------------------------------------------------------------------
  //: Return the plane where 'this' polygon resides
  //---------------------------------------------------------------------------
  vgl_homg_plane_3d<double> plane(void) const { return plane_; }

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
  //  That is: are all vertices in the same plane ?
  //---------------------------------------------------------------------------
  virtual bool valid_vertices(const std::vector<vsol_point_3d_sptr> new_vertices) const;

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

  //---------------------------------------------------------------------------
  //: Return the normal vector
  //---------------------------------------------------------------------------
  vgl_vector_3d<double> normal() const;

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
  std::string is_a() const override { return std::string("vsol_polygon_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(std::string const& cls) const override
  { return cls==is_a() || vsol_region_3d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;

 protected:
  void compute_plane();
  vgl_homg_plane_3d<double> plane_;
};

//: Binary save vsol_polygon_3d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polygon_3d* p);

//: Binary load vsol_polygon_3d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polygon_3d* &p);

#endif // vsol_polygon_3d_h_
