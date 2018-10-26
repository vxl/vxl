// This is gel/vsol/vsol_polyline_2d.h
#ifndef vsol_polyline_2d_h_
#define vsol_polyline_2d_h_
//*****************************************************************************
//:
// \file
// \brief Generic polyline in 2D for drawing simple curves
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
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_compiler.h>

//: General Polyline class, part of the vsol_curve_2d hierarchy

class vsol_polyline_2d : public vsol_curve_2d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  // Description: List of vsol_point_2d
  //---------------------------------------------------------------------------
  std::vector<vsol_point_2d_sptr> *storage_;

    //---------------------------------------------------------------------------
  //: First point of the curve : just to conform to vsol_curve_2d standard
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0_;

  //---------------------------------------------------------------------------
  //: Last point of the curve
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1_;

 public:

  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Default Constructor
  //---------------------------------------------------------------------------
  vsol_polyline_2d();

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector of points
  //---------------------------------------------------------------------------
  vsol_polyline_2d(std::vector<vsol_point_2d_sptr> const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polyline_2d(vsol_polyline_2d const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_polyline_2d() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  // See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_2d* clone() const override;

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the first point of `this';  pure virtual of vsol_curve_2d
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p0() const override { return p0_; }

  //---------------------------------------------------------------------------
  //: Return the last point of `this';  pure virtual of vsol_curve_2d
  //---------------------------------------------------------------------------
  vsol_point_2d_sptr p1() const override { return p1_; }

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
  virtual bool operator==(vsol_polyline_2d const& other) const;
  bool operator==(vsol_spatial_object_2d const& obj) const override; // virtual of vsol_spatial_object_2d

  //---------------------------------------------------------------------------
  //: Has `this' the same points than `other' in the same order ?
  //---------------------------------------------------------------------------
  inline bool operator!=(vsol_polyline_2d const& o) const {return !operator==(o);}


  //***************************************************************************
  // Status setting
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Set the first point of the curve
  //  REQUIRE: in(new_p0)
  //---------------------------------------------------------------------------
  void set_p0(vsol_point_2d_sptr const& new_p0) override;

  //---------------------------------------------------------------------------
  //: Set the last point of the curve
  //  REQUIRE: in(new_p1)
  //---------------------------------------------------------------------------
  void set_p1(vsol_point_2d_sptr const& new_p1) override;

  //---------------------------------------------------------------------------
  //: Add another point to the curve
  //---------------------------------------------------------------------------
  void add_vertex(vsol_point_2d_sptr const& new_p);

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return `this' if `this' is a polyline, 0 otherwise
  //---------------------------------------------------------------------------
  vsol_polyline_2d const*cast_to_polyline()const override{return this;}
  vsol_polyline_2d *cast_to_polyline() override {return this;}

 private: // has been superseded by is_a()
  //: Return the curve type
  vsol_curve_2d_type curve_type() const override { return vsol_curve_2d::POLYLINE; }

 public:
  //---------------------------------------------------------------------------
  //: Return the length of `this'
  //---------------------------------------------------------------------------
  double length() const override; // pure virtual of vsol_curve_2d

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
  std::string is_a() const override { return std::string("vsol_polyline_2d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const { return cls==is_a(); }
};

//: Binary save vsol_polyline_2d* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polyline_2d* p);

//: Binary load vsol_polyline_2d* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polyline_2d* &p);

#endif // vsol_polyline_2d_h_
