// This is gel/vsol/vsol_spatial_object_3d.h
#ifndef vsol_spatial_object_3d_h_
#define vsol_spatial_object_3d_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief Base class of spatial entities Topology Geometry SpatialGroup
//
//   vsol_spatial_object_3d is the base class of all spatial entities: vtol, vsol.
//
// \author
//     Patricia A. Vrobel
// \verbatim
// Modifications
// 2000/05/12 François BERTEL  add cast_to_group() because VXL is not compiled
//                             with -frtti :-(
// 2000/05/03 Peter TU         ported
// 1996/12/16 Peter VANROOSE   made destructor protected
// 1995/12/xx JLM
//           Replaced the SpatialGroup pointer by an IUBox bounding volume.
//           See the discussion in SpatialGroup.h for the rationale.
//           The bounding volume is updated according to a timestamp
//           mechanism similar to that used in 3D display. That is,
//           if the vsol_spatial_object_3d is modified it is "touched" to
//           set the timestamp.  Then, any request to bounding information
//           automatically updates the bounding volume if necessary.
// \endverbatim
//-----------------------------------------------------------------------------

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_3d_sptr.h>

#include <vcl_iostream.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_3d.h>
#include <vcl_vector.h>
class vsol_curve_3d;
class vsol_group_3d;
class vsol_volume_3d;
class vtol_topology_object;
class vsol_spatial_object_3d;
extern void iu_delete(vsol_spatial_object_3d *);


#ifndef VSOL_SPATIAL_OBJECT_FLAGS_
#define VSOL_SPATIAL_OBJECT_FLAGS_

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

#endif

//: base class for spatial object

class vsol_spatial_object_3d
  : public vul_timestamp,
    public vbl_ref_count
{
// Data Members--------------------------------------------------------------
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
  static char *SpatialTypes[];
  static const float eps;

protected:

  vsol_box_3d *bounding_box_; // bounding volume
  unsigned int  tag_;         // for the COOL containers.
  int           id_;
  static int    tagcount_;    // global count of all spatial objects.

protected:

// Constructors/Destructors

  virtual ~vsol_spatial_object_3d();
  inline vsol_spatial_object_3d(void);
  inline vsol_spatial_object_3d(vsol_spatial_object_3d const& s);
  inline int not_applicable(const char *message) const
    { vcl_cerr << message << "() function call not applicable"
               << "\tfor spatial object " << get_name() << " !\n";
      return 0;
    }
public:

// Data Access---------------------------------------------------------------

  //: data description
  virtual vsol_spatial_object_3d::vsol_spatial_object_3d_type
  spatial_type(void) const=0;
  const char *get_name(void) const;

  //: bounding box stuff

  inline virtual void compute_bounding_box(void);

  vsol_box_3d *get_bounding_box(void);

  //: get set id of objects

  inline int get_id(void) const { return id_; }
  inline void set_id(int i) { id_ = i; }

  //: unprotect the object
  virtual void un_protect(void)
  {
    ref_count--;
    iu_delete(this);
  }

  //---------------------------------------------------------------------------
  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  //---------------------------------------------------------------------------
  virtual vsol_spatial_object_3d_sptr clone(void) const=0;
#if 0
  virtual vsol_spatial_object_3d *SpatialCopy(void)
  {
    return (vsol_spatial_object_3d*) NULL;
  }
#endif
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

  // inline void    SetFormat(char* f);

// Data Control--------------------------------------------------------------

public:

  virtual void protected_destroy(void);

#if 0 // these functions are deprecated, and are nowhere used
//: Geometry Accessors
  virtual vcl_vector<double> *GetLocation(void) { return 0; }
  virtual vcl_vector<double> *GetOrientation(void) { return 0; }
  virtual vcl_vector<double> *GetSize(void) { return 0; }
  virtual vcl_vector<double> *GetScalar(void) { return 0; }

  const vcl_vector<double> *GetLocation(void) const
  {
    return ((vsol_spatial_object_3d *)this)->GetLocation();
  }
  const vcl_vector<double> *GetOrientation(void) const
  {
    return ((vsol_spatial_object_3d *)this)->GetOrientation();
  }
  const vcl_vector<double> *GetSize(void) const
  {
    return ((vsol_spatial_object_3d *)this)->GetSize();
  }
  const vcl_vector<double> *GetScalar(void) const
  {
    return ((vsol_spatial_object_3d *)this)->GetScalar();
  }

  virtual void SetLocation(float,
                           float,
                           float)
  {
    not_applicable("SetLocation");
  }

  virtual void SetLocation(vcl_vector<double> &)
  {
    not_applicable("SetLocation");
  }

  virtual void SetOrientation(float,
                              float,
                              float)
  {
    not_applicable("SetOrientation");
  }

  virtual void SetOrientation(vcl_vector<double> &)
  {
    not_applicable("SetOrientation");
  }

  virtual void SetSize(float,
                       float,
                       float)
  {
    not_applicable("SetSize");
  }

  virtual void SetSize(vcl_vector<double> &)
  {
    not_applicable("SetSize");
  }

  virtual void SetScalar(float,
                         float,
                         float)
  {
    not_applicable("SetScalar");
  }

  virtual void SetScalar(vcl_vector<double> &)
  {
    not_applicable("SetScalar");
  }

  virtual void UpdateGeometry(void)
  {
    not_applicable("UpdateGeometry");
  }

  virtual  vcl_vector<double>  *GetCog()  { return NULL; }
  const vcl_vector<double> *GetCog() const  { return ((vsol_spatial_object_3d*)this)->GetCog(); }

  //---- transformations ----------------------------------------------
  virtual bool Transform(CoolTransform const& m);
  virtual CoolTransform GetTransformation() const; // to `standard' object
  virtual bool LinearScale(float sf);   // leaving GetLocation() fixed

  virtual bool Translate();     // moving GetLocation() to (0,0,0)
  virtual bool Rotate();        // moving GetOrientation() to (1,0,0)
  virtual bool Reflect();       // around GetOrientation() over 180 degrees
  virtual bool Shear(float);    // leaving GetOrientation() fixed
#endif

  //: bounding box accessors

  inline void check_update_bounding_box(void);  // Test consistency of bound
  // inline void get_min_location(vcl_vector<double>& min_loc);
  // inline void get_max_location(vcl_vector<double>& max_loc);
  void grow_minmax_bounds(vsol_box_3d & comp_box); //grow to the largest dimension of this and comp_box
  inline float get_min_x(void);
  inline float get_max_x(void);
  inline float get_min_y(void);
  inline float get_max_y(void);
  inline float get_min_z(void);
  inline float get_max_z(void);
  virtual void set_min_x(float xmin);
  virtual void set_max_x(float xmax);
  virtual void set_min_y(float ymin);
  virtual void set_max_y(float ymax);
  virtual void set_min_z(float zmin);
  virtual void set_max_z(float zmax);

  //: Operators

  virtual bool operator==(vsol_spatial_object_3d const& obj) const
  {
    return this==&obj;
  }

  //-------------------------------------------------------------------------

  //---------------------------------------------------------------------------
  //: The same behavior than dynamic_cast<>.
  // Needed because VXL is not compiled with -frtti :-(
  //---------------------------------------------------------------------------
  virtual vtol_topology_object* cast_to_topology_object() {return 0;}
  virtual const vtol_topology_object* cast_to_topology_object()const{return 0;}
  virtual vsol_group_3d *cast_to_group(void) {return 0;}
  virtual const vsol_group_3d *cast_to_group(void) const {return 0;}
  virtual vsol_curve_3d *cast_to_curve(void) {return 0;}
  virtual const vsol_curve_3d *cast_to_curve(void) const {return 0;}
  virtual vsol_volume_3d* cast_to_volume(void) { return 0;}
  virtual vsol_volume_3d const* cast_to_volume() const { return 0;}

  inline virtual void print(vcl_ostream &strm=vcl_cout) const;

  inline virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

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
{
  bounding_box_ = NULL;
  id_ = 0;
  tag_ = 0;
  vsol_spatial_object_3d::tagcount_++;
  set_tag_id(tagcount_);
  touch();
}


inline vsol_spatial_object_3d::vsol_spatial_object_3d(vsol_spatial_object_3d const &s)
{
  bounding_box_=0;
  id_ = s.get_id();
  tag_ = 0;
  vsol_spatial_object_3d::tagcount_++;
  set_tag_id(tagcount_);
}


inline void vsol_spatial_object_3d::compute_bounding_box(void)   //Does nothing in this case
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->touch();
}


//: Bounds Accessors:  min_ and max_ are provided as methods on vsol_spatial_object_3d
//                    to be consistent with the previous interface
//                    Additional bounds accessors are available directly
//                    on vsol_box_3d.  - JLM

inline void vsol_spatial_object_3d::check_update_bounding_box(void)  // Test consistency of bound
{
  if (bounding_box_==0)
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

inline vsol_box_3d *vsol_spatial_object_3d::get_bounding_box(void)
{
  this->check_update_bounding_box();
  return bounding_box_;
}

//inline void vsol_spatial_object_3d::get_min_location(vcl_vector<double>& min_loc)
//{
//  this->check_update_bounding_box();
//  bounding_box_->get_min_location(min_loc);
//}

//inline void vsol_spatial_object_3d::get_max_location(vcl_vector<double>& max_loc)
//{
//  this->check_update_bounding_box();
//  bounding_box_->get_max_location(max_loc);
//}

inline float vsol_spatial_object_3d::get_min_x(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_min_x();
}


inline float vsol_spatial_object_3d::get_max_x(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_max_x();
}


inline float vsol_spatial_object_3d::get_min_y(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_min_y();
}

inline float vsol_spatial_object_3d::get_max_y(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_max_y();
}

inline float vsol_spatial_object_3d::get_min_z(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_min_z();
}

inline float vsol_spatial_object_3d::get_max_z(void)
{
  this->check_update_bounding_box();
  return bounding_box_->get_max_z();
}

inline void vsol_spatial_object_3d::set_min_x(float xmin)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_min_x(xmin);
}

inline void vsol_spatial_object_3d::set_max_x(float xmax)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_max_x(xmax);
}

inline void vsol_spatial_object_3d::set_min_y(float ymin)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_min_y(ymin);
}

inline void vsol_spatial_object_3d::set_max_y(float ymax)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_max_y(ymax);
}

inline void vsol_spatial_object_3d::set_min_z(float zmin)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_min_z(zmin);
}


inline void vsol_spatial_object_3d::set_max_z(float zmax)
{
  if (bounding_box_==0)
    bounding_box_=new vsol_box_3d();
  bounding_box_->set_max_z(zmax);
}


//: set a flag for a spatial object; flag can be VSOL_FLAG[1-6]
inline void vsol_spatial_object_3d::set_user_flag(unsigned int flag)
{
  tag_=(tag_|flag);
}

//: check if a flag is set for a spatial object; flag can be VSOL_FLAG[1-6]
inline bool vsol_spatial_object_3d::get_user_flag(unsigned int flag)
{
  return (tag_&flag) ? true : false;
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

inline void vsol_spatial_object_3d::print(vcl_ostream &strm) const
{
  describe(strm);
}

inline void vsol_spatial_object_3d::describe(vcl_ostream &strm,
                                             int blanking) const
{
  not_applicable("Describe");
  if (blanking < 0) blanking = 0;
  while (blanking--) strm << ' ';
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
