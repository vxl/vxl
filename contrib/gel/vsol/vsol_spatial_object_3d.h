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
//   2000/05/12 François BERTEL add cast_to_group() because VXL is not compiled
//                              with -frtti :-(
//   2002/12/12 Peter Vanroose added cast_to_region()
//   2003/01/08 Peter Vanroose made bounding box "mutable" and changed interface
//   2003/01/09 Peter Vanroose deprecated set_min_x() etc. and replaced with
//                       more safe set_bounding_box() and add_to_bounding_box().
//                     (The old setup only worked correctly when (0,0) in bbox.)
// \endverbatim
//-----------------------------------------------------------------------------

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_3d_sptr.h>

#include <vcl_iostream.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_3d_sptr.h>
#include <vsol/vsol_box_3d.h>
class vsol_point_3d;
class vsol_curve_3d;
class vsol_group_3d;
class vsol_region_3d;
class vsol_volume_3d;
class vtol_topology_object;
class vsol_spatial_object_3d;
extern void iu_delete(vsol_spatial_object_3d *);


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

//: base class for spatial object

class vsol_spatial_object_3d : public vul_timestamp, public vbl_ref_count
{
  // Data Members--------------------------------------------------------------

 protected:
  mutable vsol_box_3d_sptr bounding_box_; // rectangular bounding volume
  unsigned int  tag_;
  int           id_;
  static int    tagcount_;    // global count of all spatial objects.

 public:

  enum vsol_spatial_object_3d_type
  {
    SPATIAL_NO_TYPE=0,
    TOPOLOGYOBJECT,
    POINT,
    CURVE,
    REGION,
    VOLUME,
    SPATIALGROUP,
    NUM_SPATIALOBJECT_TYPES
  };
  static const char *SpatialTypes[];
  static const float eps;

 protected:

  // Constructors/Destructors

  virtual ~vsol_spatial_object_3d();
  inline vsol_spatial_object_3d(void);
  inline vsol_spatial_object_3d(vsol_spatial_object_3d const& s);
  inline void not_applicable(const char *message) const
    { vcl_cerr << message << "() function call not applicable"
               << "\tfor spatial object " << get_name() << " !\n";
    }
 public:

  // Data Access---------------------------------------------------------------

  //: data description
  virtual vsol_spatial_object_3d_type spatial_type(void) const=0;
  const char *get_name(void) const;

  //: bounding box stuff
  inline virtual void compute_bounding_box(void) const
  { if (!bounding_box_) bounding_box_=new vsol_box_3d();bounding_box_->touch();}

  vsol_box_3d_sptr get_bounding_box(void) const { check_update_bounding_box(); return bounding_box_; }

  //: get id of objects
  inline int get_id(void) const { return id_; }
  //: set id of objects
  inline void set_id(int i) { id_ = i; }

  //: unprotect the object
  inline void un_protect(void) { ref_count--; iu_delete(this); }

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_sptr clone(void) const=0;

  // Tag Flag and ID methods

  //: set user flag 1-6
  inline void set_user_flag(unsigned int flag);
  inline bool get_user_flag(unsigned int flag);
  inline void unset_user_flag(unsigned int flag);
  inline void set_tagged_union_flag();
  inline bool get_tagged_union_flag();
  inline void unset_tagged_union_flag();
  inline int get_tag_id();
  inline void set_tag_id(int id);

  // Data Control--------------------------------------------------------------

 public:

  void protected_destroy(void);

  //: bounding box accessors

  inline void check_update_bounding_box(void) const;  // Test consistency of bound
  //: grow to the largest dim. of this and comp_box, i.e., take the convex union
  void grow_minmax_bounds(vsol_box_3d & comp_box) const; // mutable const

  inline double get_min_x(void) const { check_update_bounding_box(); return bounding_box_->get_min_x(); }
  inline double get_max_x(void) const { check_update_bounding_box(); return bounding_box_->get_max_x(); }
  inline double get_min_y(void) const { check_update_bounding_box(); return bounding_box_->get_min_y(); }
  inline double get_max_y(void) const { check_update_bounding_box(); return bounding_box_->get_max_y(); }
  inline double get_min_z(void) const { check_update_bounding_box(); return bounding_box_->get_min_z(); }
  inline double get_max_z(void) const { check_update_bounding_box(); return bounding_box_->get_max_z(); }

  //: set the bounding box to a single point, discarding the old bounding box
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  inline void set_bounding_box(double x, double y, double z) const
  { bounding_box_=new vsol_box_3d; bounding_box_->add_point(x,y,z); }

  //: add a point to the bounding box and take the convex union
  // This is a "const" method since the bounding box is a "mutable" data member:
  // calculating the bounding box does not change the object.
  inline void add_to_bounding_box(double x, double y, double z) const
  { if (!bounding_box_) bounding_box_=new vsol_box_3d; bounding_box_->add_point(x,y,z); }

  // Operators

  virtual bool operator==(vsol_spatial_object_3d const& obj) const
  {
    return this==&obj;
  }

  //-------------------------------------------------------------------------

  //---------------------------------------------------------------------------
  //: The same behavior than dynamic_cast<>.
  // Needed because VXL is not always compiled with -frtti :-(
  //---------------------------------------------------------------------------
  vsol_spatial_object_3d* cast_to_spatial_object() { return this; }
  const vsol_spatial_object_3d* cast_to_spatial_object() const { return this; }
  virtual vtol_topology_object* cast_to_topology_object() {return 0;}
  virtual const vtol_topology_object* cast_to_topology_object()const{return 0;}
  virtual vsol_group_3d *cast_to_group(void) {return 0;}
  virtual const vsol_group_3d *cast_to_group(void) const {return 0;}
  virtual vsol_curve_3d *cast_to_curve(void) {return 0;}
  virtual const vsol_curve_3d *cast_to_curve(void) const {return 0;}
  virtual vsol_region_3d* cast_to_region(void) { return 0; }
  virtual const vsol_region_3d * cast_to_region(void) const { return 0; }
  virtual vsol_volume_3d* cast_to_volume(void) { return 0;}
  virtual const vsol_volume_3d * cast_to_volume() const { return 0;}
  virtual vsol_point_3d* cast_to_point(void) { return 0;}
  virtual const vsol_point_3d* cast_to_point(void) const { return 0;}

  inline virtual void print(vcl_ostream &strm=vcl_cout) const {describe(strm);}
  inline virtual void describe(vcl_ostream& =vcl_cout, int /*blanking*/=0) const { not_applicable("describe"); }

  friend inline vcl_ostream &operator<<(vcl_ostream &,const vsol_spatial_object_3d &);
  friend inline vcl_ostream &operator<<(vcl_ostream &,const vsol_spatial_object_3d *);
};

// inline member functions

inline void vsol_spatial_object_3d::set_tag_id(int id)
{
  //     ( set the new id bits)  or (save just the flag bits from the tag_)
  tag_ = ( (id & VSOL_DEXID_BITS)     |  ( tag_ & VSOL_FLAG_BITS ));
}

//: constructor initialize basic vsol_spatial_object_3d attributes.
//   bounding_box is set to NULL.
inline vsol_spatial_object_3d::vsol_spatial_object_3d(void)
  : vul_timestamp(), vbl_ref_count(), bounding_box_(0), tag_(0), id_(0)
{
  set_tag_id(++tagcount_);
}

inline vsol_spatial_object_3d::vsol_spatial_object_3d(vsol_spatial_object_3d const &s)
  : vul_timestamp(), vbl_ref_count(), bounding_box_(0), tag_(0), id_(s.get_id())
{
  set_tag_id(++tagcount_);
}

//: Bounds Accessors:  min_ and max_ are provided as methods on vsol_spatial_object_3d
//                    to be consistent with the previous interface
//                    Additional bounds accessors are available directly
//                    on vsol_box_3d.  - JLM

inline void vsol_spatial_object_3d::check_update_bounding_box(void) const
{
  if (!bounding_box_)
  {
    bounding_box_=new vsol_box_3d;
    this->compute_bounding_box();
    bounding_box_->touch();
    return;
  }
  if (bounding_box_->older(this))
  { // NOTE: first touch then compute, to avoid infinite loop!! - PVr
    bounding_box_->touch();
    this->compute_bounding_box();
  }
}

//: set a flag for a spatial object; flag can be VSOL_FLAG[1-6]
inline void vsol_spatial_object_3d::set_user_flag(unsigned int flag)
{
  tag_=(tag_|flag);
}

//: check if a flag is set for a spatial object; flag can be VSOL_FLAG[1-6]
inline bool vsol_spatial_object_3d::get_user_flag(unsigned int flag)
{
  return (tag_&flag) != 0;
}

//: un-set a flag for a spatial object; flag can be VSOL_FLAG[1-6]
inline void vsol_spatial_object_3d::unset_user_flag(unsigned int flag)
{
  tag_ = ( tag_ & (~flag) );
}

//: set the flag used by TAGGED_UNION.
inline void vsol_spatial_object_3d::set_tagged_union_flag()
{
  set_user_flag(VSOL_UNIONBIT);
}

//: check if the flag used by TAGGED_UNION is set.
inline bool vsol_spatial_object_3d::get_tagged_union_flag(void)
{
  return get_user_flag(VSOL_UNIONBIT);
}

//: un-set the flag used by TAGGED_UNION.
inline void vsol_spatial_object_3d::unset_tagged_union_flag(void)
{
  unset_user_flag(VSOL_UNIONBIT);
}

inline int vsol_spatial_object_3d::get_tag_id(void)
{
  return tag_ & VSOL_DEXID_BITS;
}

inline vcl_ostream &operator<<(vcl_ostream &strm,
                               const vsol_spatial_object_3d &so)
{
  ((vsol_spatial_object_3d const *)&so)->print(strm);
  return strm;
}

inline vcl_ostream &operator<<(vcl_ostream &strm,
                               const vsol_spatial_object_3d *so)
{
  if (so)
    ((vsol_spatial_object_3d const *)so)->print(strm);
  else
    strm << " NULL Spatial Object. ";
  return strm;
}

#endif // vsol_spatial_object_3d_h_
