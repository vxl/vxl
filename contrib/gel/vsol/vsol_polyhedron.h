// This is gel/vsol/vsol_polyhedron.h
#ifndef vsol_polyhedron_h_
#define vsol_polyhedron_h_
//*****************************************************************************
//:
// \file
// \brief Polyhedral volume in 3D space
//
//  Representation of an arbitrary polyhedral volume, i.e., a volume that
//  is bounded by any set of flat polygons.
//
//  A sufficient minimal representation for this is the set of corner points
//  together with the polygonal faces they form.
//
// \author Peter Vanroose
// \date   5 July 2000.
//
// \verbatim
//  Modifications
//   2004/05/14 Peter Vanroose  Added describe()
//   2004/09/06 Peter Vanroose  Added Binary I/O
// \endverbatim
//*****************************************************************************

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vsol/vsol_volume_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
class vsol_tetrahedron;

class vsol_polyhedron : public vsol_volume_3d
{
 protected:
  //***************************************************************************
  // Data members
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: List of vertices
  //---------------------------------------------------------------------------
  std::vector<vsol_point_3d_sptr> storage_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from a std::vector (i.e., a list of points)
  //  REQUIRE: new_vertices.size()>=4
  //---------------------------------------------------------------------------
  explicit vsol_polyhedron(std::vector<vsol_point_3d_sptr> const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polyhedron(vsol_polyhedron const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  ~vsol_polyhedron() override;

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* clone(void) const override { return new vsol_polyhedron(*this); }

  //---------------------------------------------------------------------------
  //: Safe down-casting methods
  //---------------------------------------------------------------------------
  vsol_polyhedron *cast_to_polyhedron(void) override {return this;}
  vsol_polyhedron const* cast_to_polyhedron(void) const override {return this;}

  virtual vsol_tetrahedron* cast_to_tetrahedron(void) {return nullptr;}
  virtual const vsol_tetrahedron* cast_to_tetrahedron(void) const {return nullptr;}

  //***************************************************************************
  // Access
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return vertex `i'
  //  REQUIRE: valid_index(i)
  //---------------------------------------------------------------------------
  vsol_point_3d_sptr vertex(int i) const;

  //***************************************************************************
  // Comparison
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `this' spanning the same the same volume than `other' ?
  //---------------------------------------------------------------------------
  virtual bool operator==(vsol_polyhedron const& other) const;
  bool operator==(vsol_spatial_object_3d const& obj) const override; // virtual of vsol_spatial_object_3d

  //---------------------------------------------------------------------------
  //: Negation of operator==
  //---------------------------------------------------------------------------
  bool operator!=(vsol_polyhedron const& o) const { return !operator==(o); }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Return the volume type of a polyhedron.  Its spatial type is a VOLUME.
  //---------------------------------------------------------------------------
  vsol_volume_3d_type volume_type(void)const override{return vsol_volume_3d::POLYHEDRON;}

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  void compute_bounding_box(void) const override;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return storage_.size(); }
  unsigned int num_vertices(void) const { return storage_.size(); }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  double volume(void) const override;

  //---------------------------------------------------------------------------
  //: Is `this' convex ?
  //---------------------------------------------------------------------------
  virtual bool is_convex(void) const;

  //---------------------------------------------------------------------------
  //: Is `i' a valid index for the list of vertices ?
  //---------------------------------------------------------------------------
  bool valid_index(unsigned int i) const { return i<storage_.size(); }

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Is `p' in `this' ?
  //---------------------------------------------------------------------------
  bool in(vsol_point_3d_sptr const& p) const override;

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
  std::string is_a() const override { return std::string("vsol_polyhedron"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  bool is_class(std::string const& cls) const override
  { return cls==is_a() || vsol_volume_3d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(std::ostream &strm, int blanking=0) const override;

 protected:
  //---------------------------------------------------------------------------
  //: Default constructor. Do nothing. Just to enable inheritance. Protected.
  //---------------------------------------------------------------------------
  vsol_polyhedron() = default;
};

//: Binary save vsol_polyhedron* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polyhedron* p);

//: Binary load vsol_polyhedron* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polyhedron* &p);

#endif // vsol_polyhedron_h_
