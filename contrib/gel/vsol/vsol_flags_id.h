// This is gel/vsol/vsol_flags_id.h
#ifndef vsol_flags_id_h_
#define vsol_flags_id_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief
//  Base class for vsol_spatial_object
//  Includes members related to id, tag and flags of a vsol_spatial_object
//
// \author
//     H.Can Aras
//
// \verbatim
//  
// \endverbatim
//-----------------------------------------------------------------------------

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

class vsol_flags_id
{
protected:
  // Data Members--------------------------------------------------------------
	unsigned int tag_;
  int id_;
	static int tagcount_;// global count of all spatial objects.

public:
	// Constructors/Destructors--------------------------------------------------
	virtual ~vsol_flags_id();

protected:
	vsol_flags_id();

public:
  // Data Access---------------------------------------------------------------
	//: get id of object
  int get_id() const { return id_; }
  //: set id of object
  void set_id(int i) { id_ = i; }

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
};

// inline member functions

//: set a flag for a spatial object; flag can be VSOL_FLAG[1-6]
inline void vsol_flags_id::set_user_flag(unsigned int flag)
{
  tag_ =  (tag_ | flag);
}

//: check if a flag is set for a spatial object; flag can be VSOL_FLAG[1-6]
inline bool vsol_flags_id::get_user_flag(unsigned int flag)
{
  return (tag_ & flag) != 0;
}

//: un-set a flag for a spatial object; flag can be VSOL_FLAG[1-6]
inline void vsol_flags_id::unset_user_flag(unsigned int flag)
{
  tag_ = ( tag_ & (~flag) );
}

//: set the flag used by TAGGED_UNION.
inline void vsol_flags_id::set_tagged_union_flag()
{
  set_user_flag(VSOL_UNIONBIT);
}

//: check if the flag used by TAGGED_UNION is set.
inline bool vsol_flags_id::get_tagged_union_flag()
{
  return get_user_flag(VSOL_UNIONBIT);
}

//: un-set the flag used by TAGGED_UNION.
inline void vsol_flags_id::unset_tagged_union_flag()
{
  unset_user_flag(VSOL_UNIONBIT);
}

inline int vsol_flags_id::get_tag_id()
{
  return tag_ & VSOL_DEXID_BITS;
}

inline void vsol_flags_id::set_tag_id(int id)
{
  //     ( set the new id bits)  or (save just the flag bits from the tag_)
  tag_ = ( (id & VSOL_DEXID_BITS)     |  ( tag_ & VSOL_FLAG_BITS ));
}

#endif // vsol_flags_id_h_
