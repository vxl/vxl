// This is gel/vsol/vsol_spatial_object_2d.h
#ifndef vsol_spatial_object_2d_h_
#define vsol_spatial_object_2d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Base class of 2D spatial entities (topology geometry group)
//
//   vsol_spatial_object_2d is the base class of all 2d spatial entities.
//   It provides ref counting, timestamps, a bounding box, ...
//
// \author
//     Patricia A. Vrobel
//
// \verbatim
//  Modifications
//   1995/12/xx JLM
//           Replaced the SpatialGroup pointer by an IUBox bounding volume.
//           See the discussion in SpatialGroup.h for the rationale.
//           The bounding volume is updated according to a timestamp
//           mechanism similar to that used in 3D display. That is,
//           if the vsol_spatial_object_2d is modified it is "touched" to
//           set the timestamp.  Then, any request to bounding information
//           automatically updates the bounding volume if necessary.
//   1996/12/16 Peter VANROOSE  made destructor protected
//   2000/05/03 Peter TU        ported to vxl
//   2000/05/10 François BERTEL add cast_to_group() because VXL is not compiled
//                              with -frtti :-(
//   2002/12/12 Peter Vanroose added cast_to_region()
//   2003/01/08 Peter Vanroose made bounding box "mutable" and changed interface
//   2003/01/09 Peter Vanroose deprecated set_min_x() etc. and replaced with
//                       more safe set_bounding_box() and add_to_bounding_box().
//                     (The old setup only worked correctly when (0,0) in bbox.)
//   2004/09/21 Ming-Ching Chang  Make clear distinction between 2D and 3D.
//                                Remove the postfix _2d _3d from the cast_to functions.
//   2004/09/27 Peter Vanroose added empty_bounding_box(), set_bounding_box(box)
//                             and add_to_bounding_box(box)
//   2004/11/15 H.Can Aras added inheritance from vsol_spatial_object, which introduces
//                         inheritance from vul_timestamp, ref_count and vsol_flags_id
//                         classes. members related to id, flags and tag are moved to
//                         newly-created vsol_flags_id class.
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vsol/vsol_spatial_object.h>
#include <vsl/vsl_fwd.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
class vtol_topology_object;
class vsol_spatial_object_2d;
class vsol_point_2d;
class vsol_curve_2d;
class vsol_region_2d;
class vsol_group_2d;

class vsol_spatial_object_2d : public vsol_spatial_object
{
 protected:
  // Data Members--------------------------------------------------------------
  
 private:
  mutable vsol_box_2d_sptr bounding_box_; // rectangular bounding area

 public:
  enum vsol_spatial_object_2d_type
  {
      SPATIAL_NO_TYPE=0,
      TOPOLOGYOBJECT,
      POINT,
      CURVE,
      REGION,
      SPATIALGROUP,
      VOLUME,
      NUM_SPATIALOBJECT_TYPES
  };

  static const char *SpatialTypes[];
  static const float eps;

  // Constructors/Destructors--------------------------------------------------
  virtual ~vsol_spatial_object_2d();

 protected:
  //: constructor initializes basic vsol_spatial_object_2d attributes.
  //   bounding_box is set to NULL.
  vsol_spatial_object_2d();
  vsol_spatial_object_2d(vsol_spatial_object_2d const& other);
  void not_applicable(vcl_string const& message) const
  {
      vcl_cerr <<message<<"() function call not applicable\tfor 2d spatial object "
               <<get_name()<<" !\n";
  }

 public:
  // Data Access---------------------------------------------------------------

  //: get the spatial type
  virtual vsol_spatial_object_2d_type spatial_type() const=0;

  const char *get_name() const; // derived from spatial_type()

  //: unprotect the object
  void un_protect() { this->unref(); }

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* clone() const=0;

  // Binary I/O------------------------------------------------------------------

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const=0;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  virtual void print(vcl_ostream &strm=vcl_cout) const { describe(strm); }
  virtual void describe(vcl_ostream& =vcl_cout, int /*blanking*/=0) const { not_applicable("describe"); }

  friend inline vcl_ostream &operator<<(vcl_ostream &, vsol_spatial_object_2d const&);
  friend inline vcl_ostream &operator<<(vcl_ostream &, vsol_spatial_object_2d const*);

  //Operators
  virtual bool operator==(vsol_spatial_object_2d const& obj) const { return this==&obj; }
  bool operator!=(vsol_spatial_object_2d const& obj) { return !(*this==obj); }

  // Data Control--------------------------------------------------------------

  vsol_box_2d_sptr get_bounding_box() const { check_update_bounding_box(); return bounding_box_; }

  double get_min_x() const;
  double get_max_x() const;
  double get_min_y() const;
  double get_max_y() const;

 protected:
  //: make the bounding box empty; often first step in bounding box calculation
  void empty_bounding_box() const; // mutable const
  //: set the bounding box; to be used in bounding box calculation
  void set_bounding_box(vsol_box_2d_sptr const& box) const; // mutable const
  //: set the bounding box to a single point, discarding the old bounding box
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  void set_bounding_box(double x, double y) const;
  //: add a point to the bounding box and take the convex union
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  void add_to_bounding_box(double x, double y) const;
  //: set the existing bounding box to the convex union of it with the given box
  void add_to_bounding_box(vsol_box_2d_sptr const& box) const; // mutable const
  //: grow to the largest dim. of this and \a box, i.e., take the convex union
  void grow_minmax_bounds(vsol_box_2d_sptr const& b) const{ add_to_bounding_box(b); }
  //: compute bounding box, do nothing in this case except touching the box
  virtual void compute_bounding_box() const;
  //: Test consistency of bound
  void check_update_bounding_box() const;

 public:
  //---------------------------------------------------------------------------
  //: The same behavior than dynamic_cast<>.
  // Needed because VXL is not necessarily compiled with -frtti
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_2d* cast_to_spatial_object() { return this; }
  virtual vsol_spatial_object_2d const* cast_to_spatial_object() const{return this;}

  virtual vtol_topology_object* cast_to_topology_object() {return 0;}
  virtual vtol_topology_object const* cast_to_topology_object()const{return 0;}

  virtual vsol_spatial_object_2d* cast_to_vsol_spatial_object() { return 0; }
  virtual vsol_spatial_object_2d const* cast_to_vsol_spatial_object() const { return 0; }
  virtual vsol_point_2d* cast_to_point() { return 0; }
  virtual vsol_point_2d const* cast_to_point() const { return 0; }
  virtual vsol_curve_2d *cast_to_curve() { return 0; }
  virtual vsol_curve_2d const* cast_to_curve() const { return 0; }
  virtual vsol_region_2d* cast_to_region() { return 0; }
  virtual vsol_region_2d const* cast_to_region() const { return 0; }
  virtual vsol_group_2d *cast_to_group() { return 0; }
  virtual vsol_group_2d const* cast_to_group() const { return 0; }
};

// inline member functions

inline vcl_ostream &operator<<(vcl_ostream &strm, vsol_spatial_object_2d const& so)
{
  so.print(strm);
  return strm;
}

inline vcl_ostream &operator<<(vcl_ostream &strm, vsol_spatial_object_2d const* so)
{
  if (so)
    so->print(strm);
  else
    strm << "NULL Spatial Object.\n";
  return strm;
}

//: Stream output operator for class pointer
inline void vsl_print_summary(vcl_ostream& os, vsol_spatial_object_2d const* so)
{
  os << so;
}

//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr)".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(vsol_spatial_object_2d const& b);

#endif // vsol_spatial_object_2d_h_
