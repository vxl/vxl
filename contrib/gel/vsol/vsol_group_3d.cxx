#include <vsol/vsol_group_3d.h>

//:
//  \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_3d.h>

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
vsol_spatial_object_3d_sptr vsol_group_3d::object(const int i) const
{
  // require
  assert((i>=0)&&(i<size()));

  vcl_list<vsol_spatial_object_3d_sptr>::iterator j;
  int k;

  j=storage_->begin();
  for(k=0;k<i;++k)
    ++j;
  return *j;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a group. It is a SPATIALGROUP
//---------------------------------------------------------------------------
enum  vsol_spatial_object_3d::vsol_spatial_object_3d_type
vsol_group_3d::spatial_type(void) const
{
  return vsol_spatial_object_3d::SPATIALGROUP;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
// Require: size()>0
//---------------------------------------------------------------------------
void vsol_group_3d::compute_bounding_box(void)
{
  // require
  assert(size()>0);

  double xmin;
  double ymin;
  double zmin;
  double xmax;
  double ymax;
  double zmax;

  vsol_box_3d *b;
  vcl_list<vsol_spatial_object_3d_sptr>::iterator i;

  for(i=storage_->begin();i!=storage_->end();++i)
    {
      b=(*i)->get_bounding_box();
      if(i==storage_->begin())
        {
          xmin=b->get_min_x();
          ymin=b->get_min_y();
          zmin=b->get_min_z();
          xmax=b->get_max_x();
          ymax=b->get_max_y();
          zmax=b->get_max_z();
        }
      else
        {
          if(b->get_min_x()<xmin)
            xmin=b->get_min_x();
          if(b->get_min_y()<ymin)
            ymin=b->get_min_y();
          if(b->get_min_z()<zmin)
            zmin=b->get_min_z();
          if(b->get_max_x()>xmax)
            xmax=b->get_max_x();
          if(b->get_max_y()>ymax)
            ymax=b->get_max_y();
          if(b->get_max_z()>zmax)
            zmax=b->get_max_z();
        }
      delete b;
    }
  if(_bounding_box==0)
    _bounding_box=new vsol_box_3d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
  _bounding_box->set_min_z(zmin);
  _bounding_box->set_max_z(zmax);
}

//---------------------------------------------------------------------------
//: Return the number of direct children of the group
//---------------------------------------------------------------------------
int vsol_group_3d::size(void) const
{
  return storage_->size();
}

//---------------------------------------------------------------------------
//: Return the number of objects of the group
//---------------------------------------------------------------------------
int vsol_group_3d::deep_size(void) const
{
  int result=0;
  vcl_list<vsol_spatial_object_3d_sptr>::iterator i;
  for(i=storage_->begin();i!=storage_->end();++i)
    {
      // g=dynamic_cast<vsol_group_3d const *>((*i).ptr());   // GOOD VERSION
      vsol_group_3d const* g=(vsol_group_3d const*)((*i).ptr()->cast_to_group()); // BAD HACK
      if(g!=0)
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
void vsol_group_3d::remove_object(const int i)
{
  // require
  assert((i>=0)&&(i<size()));

  vcl_list<vsol_spatial_object_3d_sptr>::iterator j;
  int k;

  j=storage_->begin();
  for(k=0;k<i;++k)
    ++j;
  storage_->erase(j);
}

//---------------------------------------------------------------------------
//: Is `new_object' a child (direct or not) of `this' ?
//---------------------------------------------------------------------------
bool
vsol_group_3d::is_child(const vsol_spatial_object_3d_sptr &new_object) const
{
  bool result;
  vsol_group_3d *g;

  vcl_list<vsol_spatial_object_3d_sptr>::iterator i;

  result=false;
  for(i=storage_->begin();(i!=storage_->end())&&!result;++i)
    {
      result=(*i).ptr()==new_object.ptr();
      if(!result)
        {
          // g=dynamic_cast<vsol_group_3d const *>((*i).ptr());   // GOOD VERSION
          g=(vsol_group_3d *)((*i).ptr()->cast_to_group()); // BAD HACK
          if(g!=0)
            g->is_child(new_object);
        }
    }
  return result;
}

//---------------------------------------------------------------------------
//: The same behavior than dynamic_cast<>. Needed because VXL is not compiled with -frtti :-(
//---------------------------------------------------------------------------
const vsol_group_3d *vsol_group_3d::cast_to_group(void) const
{
  return this;
}
