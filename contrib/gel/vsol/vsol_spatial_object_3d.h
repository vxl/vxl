#ifndef _vsol_spatial_object_3d_h_
#define _vsol_spatial_object_3d_h_
//-----------------------------------------------------------------------------
//
// .NAME    vsol_spatial_object_3d - Base class of spatial entities Topology Geometry SpatialGroup
// .LIBRARY vsol
// .HEADER  vxl package
// .INCLUDE vsol/vsol_spatial_object_3d.h
// .FILE    vsol_spatial_object_3d.cxx
//
// .SECTION Description
//   vsol_spatial_object_3d is the base class of all spatial entities: vtol, vsol.
//
// .SECTION Author
//     Patricia A. Vrobel
// .SECTION Modifications
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
//-----------------------------------------------------------------------------

class vsol_spatial_object_3d;

//*****************************************************************************
// External declarations for values
//*****************************************************************************
#include <vsol/vsol_spatial_object_3d_sptr.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vul/vul_timestamp.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_box_3d.h>
#include <vcl_vector.h>
#include <vsol/vsol_group_3d_sptr.h>

class vtol_topology_object_3d;
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

  vsol_box_3d *_bounding_box; // bounding volume
  unsigned int  _tag;         // for the COOL containers.
  int           _id;
  static int    _tagcount;    // global count of all spatial objects.

protected:

// Constructors/Destructors

  virtual ~vsol_spatial_object_3d();
  inline vsol_spatial_object_3d(void);
  inline vsol_spatial_object_3d(vsol_spatial_object_3d const& s);
  inline int not_applicable(const char *message) const
    { vcl_cerr << message << " Function call not applicable "
           << "\tfor spatial object " << get_name() << " ! "<< vcl_endl;
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

  int get_id(void) const
  {
    return _id;
  }
  void set_id(int i)
  {
    _id = i;
  }

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
  inline unsigned int  get_user_flag(unsigned int flag);
  inline void unset_user_flag(unsigned int flag);
  inline void set_tagged_union_flag();
  inline unsigned int get_tagged_union_flag();
  inline void unset_tagged_union_flag();
  inline int get_tag_id();
  inline void set_tag_id(int id);

  // inline void    SetFormat(char* f);

  // virtual SpatialGroup *  CastToSpatialGroup()   { return NULL;}
  // virtual SpatialGroup const* CastToSpatialGroup() const { return NULL;}
  virtual vtol_topology_object_3d* cast_to_topology_object_3d() { return NULL;}

  // for the moment topology object inherits off of spatial_object_3d

  virtual vtol_topology_object* cast_to_topology_object() { return NULL;}

  //virtual GeometryObject* CastToGeometryObject() { return NULL;}
// Data Control--------------------------------------------------------------

public:

  virtual void protected_destroy(void);

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

#if 0 // commented out
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
  virtual const vsol_group_3d *cast_to_group(void) const;

  inline virtual void print(vcl_ostream &strm=vcl_cout) const;

  inline virtual void describe(vcl_ostream &strm=vcl_cout, int blanking=0) const;

  friend inline vcl_ostream &operator<<(vcl_ostream &,const vsol_spatial_object_3d &);
  friend inline vcl_ostream &operator<<(vcl_ostream &,const vsol_spatial_object_3d *);
};

// inline member functions

inline void vsol_spatial_object_3d::set_tag_id(int id)
{
  //     ( set the new id bits)  or (save just the flag bits from the _tag)
  _tag = ( (id & VSOL_DEXID_BITS)     |  ( _tag & VSOL_FLAG_BITS ));
}

//: constructor initialize basic vsol_spatial_object_3d attributes.
//   bounding_box is set to NULL.
inline vsol_spatial_object_3d::vsol_spatial_object_3d(void)
{
  _bounding_box = NULL;
  _id = 0;
  _tag = 0;
  vsol_spatial_object_3d::_tagcount++;
  set_tag_id(_tagcount);
  touch();
}


inline vsol_spatial_object_3d::vsol_spatial_object_3d(vsol_spatial_object_3d const &s)
{
  _bounding_box=0;
  _id = s.get_id();
  _tag = 0;
  vsol_spatial_object_3d::_tagcount++;
  set_tag_id(_tagcount);
}


inline void vsol_spatial_object_3d::compute_bounding_box(void)   //Does nothing in this case
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->touch();
}


//: Bounds Accessors:  min_ and max_ are provided as methods on vsol_spatial_object_3d
//                    to be consistent with the previous interface
//                    Additional bounds accessors are available directly
//                    on vsol_box_3d.  - JLM

inline void vsol_spatial_object_3d::check_update_bounding_box(void)  // Test consistency of bound
{
  if(_bounding_box==0)
    {
      _bounding_box=new vsol_box_3d;
      this->compute_bounding_box();
      _bounding_box->touch();
      return;
    }
  if(_bounding_box->older(this))
    { // NOTE: first touch then compute, to avoid infinite loop!! - PVr
      _bounding_box->touch();
      this->compute_bounding_box();
    }
}

inline vsol_box_3d *vsol_spatial_object_3d::get_bounding_box(void)
{
  this->check_update_bounding_box();
  return _bounding_box;
}

//inline void vsol_spatial_object_3d::get_min_location(vcl_vector<double>& min_loc)
//{
//  this->check_update_bounding_box();
//  _bounding_box->get_min_location(min_loc);
//}

//inline void vsol_spatial_object_3d::get_max_location(vcl_vector<double>& max_loc)
//{
//  this->check_update_bounding_box();
//  _bounding_box->get_max_location(max_loc);
//}


inline float vsol_spatial_object_3d::get_min_x(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_min_x();
}


inline float vsol_spatial_object_3d::get_max_x(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_max_x();
}


inline float vsol_spatial_object_3d::get_min_y(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_min_y();
}

inline float vsol_spatial_object_3d::get_max_y(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_max_y();
}

inline float vsol_spatial_object_3d::get_min_z(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_min_z();
}

inline float vsol_spatial_object_3d::get_max_z(void)
{
  this->check_update_bounding_box();
  return _bounding_box->get_max_z();
}

inline void vsol_spatial_object_3d::set_min_x(float xmin)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_x(xmin);
}

inline void vsol_spatial_object_3d::set_max_x(float xmax)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_max_x(xmax);
}

inline void vsol_spatial_object_3d::set_min_y(float ymin)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_y(ymin);
}

inline void vsol_spatial_object_3d::set_max_y(float ymax)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_max_y(ymax);
}

inline void vsol_spatial_object_3d::set_min_z(float zmin)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_z(zmin);
}


inline void vsol_spatial_object_3d::set_max_z(float zmax)
{
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_max_z(zmax);
}


//: set_ a flag for a spatialObject, flag can be VSOL_FLAG[1-6]
inline void vsol_spatial_object_3d::set_user_flag(unsigned int flag)
{
  _tag=(_tag|flag);
}

//: get_ a flag for a spatialObject, flag can be VSOL_FLAG[1-6] return value is
//    one or zero.
inline unsigned int  vsol_spatial_object_3d::get_user_flag(unsigned int flag)
{
  return (_tag&flag) ? 1 : 0;
}

//: set_ a flag for a spatialObject, flag can be VSOL_FLAG[1-6] value is
//    set to zero.
inline void vsol_spatial_object_3d::unset_user_flag(unsigned int flag)
{
  _tag = ( _tag & (~flag) );
}

//: set_ the flag used by TAGGED_UNION.
inline void vsol_spatial_object_3d::set_tagged_union_flag()
{
  set_user_flag(VSOL_UNIONBIT);
}

//: get_ the flag used by TAGGED_UNION.
inline unsigned int vsol_spatial_object_3d::get_tagged_union_flag(void)
{
  return get_user_flag(VSOL_UNIONBIT);
}

inline void vsol_spatial_object_3d::unset_tagged_union_flag(void)
{
  unset_user_flag(VSOL_UNIONBIT);
}

inline int vsol_spatial_object_3d::get_tag_id(void)
{
  return _tag & VSOL_DEXID_BITS;
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

#endif // _vsol_spatial_object_3d_h_
