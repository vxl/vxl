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

#include <vsol/vsol_volume_3d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vcl_vector.h>
#include <vcl_iosfwd.h>
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
  vcl_vector<vsol_point_3d_sptr> storage_;

 public:
  //***************************************************************************
  // Initialization
  //***************************************************************************

  //---------------------------------------------------------------------------
  //: Constructor from a vcl_vector (i.e., a list of points)
  //  REQUIRE: new_vertices.size()>=4
  //---------------------------------------------------------------------------
  explicit vsol_polyhedron(vcl_vector<vsol_point_3d_sptr> const& new_vertices);

  //---------------------------------------------------------------------------
  //: Copy constructor
  //---------------------------------------------------------------------------
  vsol_polyhedron(vsol_polyhedron const& other);

  //---------------------------------------------------------------------------
  //: Destructor
  //---------------------------------------------------------------------------
  virtual ~vsol_polyhedron();

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone(void) const { return new vsol_polyhedron(*this); }

  //---------------------------------------------------------------------------
  //: Safe down-casting methods
  //---------------------------------------------------------------------------
  virtual vsol_polyhedron *cast_to_polyhedron(void) {return this;}
  virtual vsol_polyhedron const* cast_to_polyhedron(void) const {return this;}

  virtual vsol_tetrahedron* cast_to_tetrahedron(void) {return 0;}
  virtual const vsol_tetrahedron* cast_to_tetrahedron(void) const {return 0;}

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
  virtual bool operator==(vsol_spatial_object_3d const& obj) const; // virtual of vsol_spatial_object_3d

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
  vsol_volume_3d_type volume_type(void)const{return vsol_volume_3d::POLYHEDRON;}

  //---------------------------------------------------------------------------
  //: Compute the bounding box of `this'
  //---------------------------------------------------------------------------
  virtual void compute_bounding_box(void) const;

  //---------------------------------------------------------------------------
  //: Return the number of vertices
  //---------------------------------------------------------------------------
  unsigned int size(void) const { return storage_.size(); }
  unsigned int num_vertices(void) const { return storage_.size(); }

  //---------------------------------------------------------------------------
  //: Return the volume of `this'
  //---------------------------------------------------------------------------
  virtual double volume(void) const;

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
  virtual bool in(vsol_point_3d_sptr const& p) const;

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
  virtual vcl_string is_a() const { return vcl_string("vsol_polyhedron"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const
  { return cls==is_a() || vsol_volume_3d::is_class(cls); }

  //---------------------------------------------------------------------------
  //: output description to stream
  //---------------------------------------------------------------------------
  void describe(vcl_ostream &strm, int blanking=0) const;

 protected:
  //---------------------------------------------------------------------------
  //: Default constructor. Do nothing. Just to enable inheritance. Protected.
  //---------------------------------------------------------------------------
  vsol_polyhedron() {}
};

//: Binary save vsol_polyhedron* to stream.
void vsl_b_write(vsl_b_ostream &os, const vsol_polyhedron* p);

//: Binary load vsol_polyhedron* from stream.
void vsl_b_read(vsl_b_istream &is, vsol_polyhedron* &p);

#endif // vsol_polyhedron_h_
