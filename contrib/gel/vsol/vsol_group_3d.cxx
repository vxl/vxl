// This is gel/vsol/vsol_group_3d.cxx
#include "vsol_group_3d.h"
//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor: with no child
//---------------------------------------------------------------------------
vsol_group_3d::vsol_group_3d(void)
{
  storage_=new vcl_list<vsol_spatial_object_3d_sptr>();
}

//---------------------------------------------------------------------------
//: Copy constructor.
// Description: The objects of the group are not duplicated
//---------------------------------------------------------------------------
vsol_group_3d::vsol_group_3d(const vsol_group_3d &other)
  : vsol_spatial_object_3d(other)
{
  storage_=new vcl_list<vsol_spatial_object_3d_sptr>(*other.storage_);
}

//---------------------------------------------------------------------------
//: Destructor
// Description: The objects of the group are not deleted
//---------------------------------------------------------------------------
vsol_group_3d::~vsol_group_3d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_group_3d::clone(void) const
{
  return new vsol_group_3d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the object `i'
// Require: i>=0 and i<size()
//---------------------------------------------------------------------------
vsol_spatial_object_3d_sptr vsol_group_3d::object(unsigned int i) const
{
  // require
  assert(i<size());

  vcl_list<vsol_spatial_object_3d_sptr>::iterator j = storage_->begin();
  for (unsigned int k=0;k<i;++k)
    ++j;
  return *j;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a group. It is a SPATIALGROUP
//---------------------------------------------------------------------------
vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_group_3d::spatial_type(void) const
{
  return vsol_spatial_object_3d::SPATIALGROUP;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
// Require: size()>0
//---------------------------------------------------------------------------
void vsol_group_3d::compute_bounding_box(void) const
{
  // require
  assert(size()>0);

  vcl_list<vsol_spatial_object_3d_sptr>::iterator i = storage_->begin();
  set_bounding_box(   (*i)->get_min_x(), (*i)->get_min_y(), (*i)->get_min_z());
  add_to_bounding_box((*i)->get_max_x(), (*i)->get_max_y(), (*i)->get_max_z());
  for (++i; i!=storage_->end(); ++i)
  {
    add_to_bounding_box((*i)->get_min_x(), (*i)->get_min_y(), (*i)->get_min_z());
    add_to_bounding_box((*i)->get_max_x(), (*i)->get_max_y(), (*i)->get_max_z());
  }
}

//---------------------------------------------------------------------------
//: Return the number of objects of the group
//---------------------------------------------------------------------------
unsigned int vsol_group_3d::deep_size(void) const
{
  int result=0;
  vcl_list<vsol_spatial_object_3d_sptr>::iterator i;
  for (i=storage_->begin(); i!=storage_->end(); ++i)
  {
    vsol_group_3d const* g=(*i)->cast_to_group();
    if (g!=0)
      result+=g->deep_size();
    else
      ++result;
  }
  return result;
}

//***************************************************************************
// Element change
//***************************************************************************

//---------------------------------------------------------------------------
//: Add an object `new_object'to `this'
// Require: !is_child(new_object)
//---------------------------------------------------------------------------
void vsol_group_3d::add_object(const vsol_spatial_object_3d_sptr &new_object)
{
  // require
  assert(!is_child(new_object));

  storage_->push_back(new_object);
}

//***************************************************************************
// Removal
//***************************************************************************

//---------------------------------------------------------------------------
//: Remove object `i' of `this' (not delete it)
// Require: i>=0 and i<size()
//---------------------------------------------------------------------------
void vsol_group_3d::remove_object(unsigned int i)
{
  // require
  assert(i<size());

  vcl_list<vsol_spatial_object_3d_sptr>::iterator j = storage_->begin();
  for (unsigned int k=0;k<i;++k)
    ++j;
  storage_->erase(j);
}

//---------------------------------------------------------------------------
//: Is `new_object' a child (direct or not) of `this' ?
//---------------------------------------------------------------------------
bool
vsol_group_3d::is_child(const vsol_spatial_object_3d_sptr &new_object) const
{
  vcl_list<vsol_spatial_object_3d_sptr>::iterator i;
  for (i=storage_->begin(); i!=storage_->end(); ++i)
  {
    if ((*i).ptr()==new_object.ptr())
      return true;
    vsol_group_3d const* g=(*i)->cast_to_group();
    if (g!=0 && g->is_child(new_object))
      return true;
  }
  return false;
}
