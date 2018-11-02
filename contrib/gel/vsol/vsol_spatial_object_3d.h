// This is gel/vsol/vsol_spatial_object_3d.h
#ifndef vsol_spatial_object_3d_h_
#define vsol_spatial_object_3d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Base class of 3D spatial entities (topology geometry group)
//
//   vsol_spatial_object_3d is the base class of all 3d spatial entities.
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
//           if the vsol_spatial_object_3d is modified it is "touched" to
//           set the timestamp.  Then, any request to bounding information
//           automatically updates the bounding volume if necessary.
//   1996/12/16 Peter VANROOSE  made destructor protected
//   2000/05/03 Peter TU        ported to vxl
//   2000/05/12 Francois BERTEL add cast_to_group() because VXL is not compiled
//                              with -frtti :-(
//   2002/12/12 Peter Vanroose added cast_to_region()
//   2003/01/08 Peter Vanroose made bounding box "mutable" and changed interface
//   2003/01/09 Peter Vanroose deprecated set_min_x() etc. and replaced with
//                       more safe set_bounding_box() and add_to_bounding_box().
//                     (The old setup only worked correctly when (0,0) in bbox.)
//   2004/09/06 Peter Vanroose  Added safe cast methods to surface_3d
//   2004/09/17 MingChing Chang  Add cast_to_region().
//   2004/09/21 Ming-Ching Chang  Make clear distinction between 2D and 3D.
//                                Add some missing parts to 3D classes.
//   2004/09/27 Peter Vanroose added empty_bounding_box(), set_bounding_box(box)
//                             and add_to_bounding_box(box)
//   2004/11/15 H.Can Aras added inheritance from vsol_spatial_object, which introduces
//                         inheritance from vul_timestamp, ref_count and vsol_flags_id
//                         classes. members related to id, flags and tag are moved to
//                         newly-created vsol_flags_id class.
// \endverbatim
//-----------------------------------------------------------------------------

#include <string>
#include <iostream>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsol/vsol_spatial_object.h>
#include <vsl/vsl_fwd.h>
#include <vsol/vsol_spatial_object_3d_sptr.h>
#include <vsol/vsol_box_3d_sptr.h>
class vtol_topology_object;
class vsol_spatial_object_3d;
class vsol_point_3d;
class vsol_curve_3d;
class vsol_surface_3d;
class vsol_region_3d;
class vsol_volume_3d;
class vsol_group_3d;

#ifndef vsol_spatial_object_flags_
#define vsol_spatial_object_flags_

// system flags
const unsigned int VSOL_UNIONBIT       = 0x80000000;
const unsigned int VSOL_SYSTEM_FLAG1   = 0x01000000;
// user flags
const unsigned int VSOL_FLAG1          = 0x40000000;
const unsigned int VSOL_FLAG2          = 0x20000000;
const unsigned int VSOL_FLAG3          = 0x1000000;
const unsigned int VSOL_FLAG4          = 0x08000000;
const unsigned int VSOL_FLAG5          = 0x04000000;
const unsigned int VSOL_FLAG6          = 0x02000000;

// mask for last three bytes of tag field
const unsigned int VSOL_DEXID_BITS     = 0x00FFFFFF;
const unsigned int VSOL_FLAG_BITS      = 0xFF000000;

#endif // vsol_spatial_object_flags_

class vsol_spatial_object_3d : public vsol_spatial_object
{
 protected:
  // Data Members--------------------------------------------------------------

 private:
  mutable vsol_box_3d_sptr bounding_box_; // 3d rectangular bounding area

 public:
  enum vsol_spatial_object_3d_type
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
  ~vsol_spatial_object_3d() override;

 protected:
  //: constructor initializes basic vsol_spatial_object_3d attributes.
  //   bounding_box is set to NULL.
  vsol_spatial_object_3d();
  vsol_spatial_object_3d(vsol_spatial_object_3d const& other);
  void not_applicable(std::string const& message) const
  {
      std::cerr <<message<<"() function call not applicable\tfor 3d spatial object "
               <<get_name()<<" !\n";
  }

 public:
  // Data Access---------------------------------------------------------------

  //: get the spatial type
  virtual vsol_spatial_object_3d_type spatial_type() const=0;

  const char *get_name() const; // derived from spatial_type()

  //: unprotect the object
  void un_protect() { this->unref(); }

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* clone() const=0;

  // Binary I/O------------------------------------------------------------------

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const=0;

  //: Return IO version number;
  short version() const;

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  virtual void print(std::ostream &strm=std::cout) const { describe(strm); }
  virtual void describe(std::ostream& =std::cout, int /*blanking*/=0) const { not_applicable("describe"); }

  friend inline std::ostream &operator<<(std::ostream &, vsol_spatial_object_3d const&);
  friend inline std::ostream &operator<<(std::ostream &, vsol_spatial_object_3d const*);

  //Operators
  virtual bool operator==(vsol_spatial_object_3d const& obj) const { return this==&obj; }
  bool operator!=(vsol_spatial_object_3d const& obj) { return !(*this==obj); }

  // Data Control--------------------------------------------------------------

  vsol_box_3d_sptr get_bounding_box() const { check_update_bounding_box(); return bounding_box_; }

  double get_min_x() const;
  double get_max_x() const;
  double get_min_y() const;
  double get_max_y() const;
  double get_min_z() const;
  double get_max_z() const;

 protected:
  //: make the bounding box empty; often first step in bounding box calculation
  void empty_bounding_box() const; // mutable const
  //: set the bounding box; to be used in bounding box calculation
  void set_bounding_box(vsol_box_3d_sptr const& box) const; // mutable const
  //: set the bounding box to a single point, discarding the old bounding box
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  void set_bounding_box(double x, double y, double z) const;
  //: add a point to the bounding box and take the convex union
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  void add_to_bounding_box(double x, double y, double z) const;
  //: set the existing bounding box to the convex union of it with the given box
  void add_to_bounding_box(vsol_box_3d_sptr const& box) const; // mutable const
  //: grow to the largest dim. of this and \a box, i.e., take the convex union
  void grow_minmax_bounds(vsol_box_3d_sptr const& b) const{ add_to_bounding_box(b); }
  //: compute bounding box, do nothing in this case except touching the box
  virtual void compute_bounding_box() const;
  //: Test consistency of bound
  void check_update_bounding_box() const;

 public:
  //---------------------------------------------------------------------------
  //: The same behavior than dynamic_cast<>.
  // Needed because VXL is not necessarily compiled with -frtti
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d* cast_to_spatial_object() { return this; }
  virtual vsol_spatial_object_3d const* cast_to_spatial_object() const{return this;}

  virtual vtol_topology_object* cast_to_topology_object() {return nullptr;}
  virtual vtol_topology_object const* cast_to_topology_object()const{return nullptr;}

  virtual vsol_spatial_object_3d* cast_to_vsol_spatial_object() { return nullptr; }
  virtual vsol_spatial_object_3d const* cast_to_vsol_spatial_object() const { return nullptr; }
  virtual vsol_point_3d* cast_to_point() { return nullptr; }
  virtual vsol_point_3d const* cast_to_point() const { return nullptr; }
  virtual vsol_curve_3d *cast_to_curve() { return nullptr; }
  virtual vsol_curve_3d const* cast_to_curve() const { return nullptr; }
  virtual vsol_surface_3d* cast_to_surface() { return nullptr; }
  virtual vsol_surface_3d const* cast_to_surface() const { return nullptr; }
  virtual vsol_volume_3d* cast_to_volume() { return nullptr; }
  virtual vsol_volume_3d const* cast_to_volume() const { return nullptr; }
  virtual vsol_region_3d* cast_to_region() { return nullptr; }
  virtual vsol_region_3d const* cast_to_region() const { return nullptr; }
  virtual vsol_group_3d *cast_to_group() { return nullptr; }
  virtual vsol_group_3d const* cast_to_group() const { return nullptr; }
};

// inline member functions

inline std::ostream &operator<<(std::ostream &strm, vsol_spatial_object_3d const& so)
{
  so.print(strm);
  return strm;
}

inline std::ostream &operator<<(std::ostream &strm, vsol_spatial_object_3d const* so)
{
  if (so)
    so->print(strm);
  else
    strm << "NULL Spatial Object.\n";
  return strm;
}

//: Stream output operator for class pointer
inline void vsl_print_summary(std::ostream& os, vsol_spatial_object_3d const* so)
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
void vsl_add_to_binary_loader(vsol_spatial_object_3d const& b);

#endif // vsol_spatial_object_3d_h_
