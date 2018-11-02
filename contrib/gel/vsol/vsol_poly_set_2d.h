// This is gel/vsol/vsol_poly_set_2d.h
#ifndef vsol_poly_set_2d_h_
#define vsol_poly_set_2d_h_
//*****************************************************************************
//:
// \file
// \brief Generic poly_set in 2D for drawing simple curves
//
// This class inherits from vsol_curve_2d.
//
// \author Amir Tamrakar
// \date   2002-04-22
//
// \verbatim
//  Modifications
//   2002-04-22 Amir Tamrakar Creation
//   2004-05-09 Joseph Mundy Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <vector>
#include <string>
#include <iostream>
#include <iosfwd>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: General poly_set class, part of the vsol_curve_2d hierarchy

class vsol_poly_set_2d : public vsol_region_2d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: List of vsol_polygon_2d
  //---------------------------------------------------------------------------
  std::vector<vsol_polygon_2d_sptr> *storage_;

 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor
  //---------------------------------------------------------------------------
  vsol_poly_set_2d();

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector of points
  //---------------------------------------------------------------------------
  vsol_poly_set_2d(std::vector<vsol_polygon_2d_sptr> const& new_polys);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_poly_set_2d(vsol_poly_set_2d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_poly_set_2d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return vertex `i'
  //  REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_polygon_2d_sptr poly(const int i) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_poly_set_2d const& other) const;
  bool operator==(vsol_spatial_object_2d const& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_poly_set_2d const& o) const {return !operator==(o);}

  //---------------------------------------------------------------------------
  //: Add another polygon to the curve
  //---------------------------------------------------------------------------
  void add_poly(vsol_polygon_2d_sptr const& new_p);

  //***************************************************************************
  // Status report
  //***************************************************************************
  //---------------------------------------------------------------------------
  //: Return the region type of a polygon.  Its spatial type is a REGION
  //---------------------------------------------------------------------------
  vsol_region_2d_type region_type(void) const override { return vsol_region_2d::POLYGON_SET; }
  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a poly_set, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_poly_set_2d const*cast_to_poly_set()const override{return this;}
  vsol_poly_set_2d *cast_to_poly_set() override {return this;}

 public:

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box() const override;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size() const { return storage_->size(); }

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(unsigned int i) const { return i<storage_->size(); }

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
  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;

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
  std::string is_a() const override { return std::string("vsol_poly_set_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(std::string const& cls) const override { return cls==is_a(); }
};

//: Binary save vsol_polyline_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_poly_set_2d* p);

//: Binary load vsol_polyline_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_poly_set_2d* &p);

#endif // vsol_poly_set_2d_h_
