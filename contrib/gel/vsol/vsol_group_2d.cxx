// This is ./gel/vsol/vsol_group_2d.cxx
#include <vsol/vsol_group_2d.h>

//:
// \file

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor: with no child
//---------------------------------------------------------------------------
vsol_group_2d::vsol_group_2d(void)
{
  storage_=new vcl_list<vsol_spatial_object_2d_sptr>();
}

//---------------------------------------------------------------------------
//: Copy constructor.
// Description: The objects of the group are not duplicated
//---------------------------------------------------------------------------
vsol_group_2d::vsol_group_2d(const vsol_group_2d &other)
{
  storage_=new vcl_list<vsol_spatial_object_2d_sptr>(*other.storage_);
}

//---------------------------------------------------------------------------
//: Destructor
// Description: The objects of the group are not deleted
//---------------------------------------------------------------------------
vsol_group_2d::~vsol_group_2d()
{
  delete storage_;
}

//---------------------------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_group_2d::clone(void) const
{
  return new vsol_group_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the object `i'
// Require: i>=0 and i<size()
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_group_2d::object(const int i) const
{
  // require
  assert((i>=0)&&(i<size()));

  vcl_list<vsol_spatial_object_2d_sptr>::iterator j;
  int k;

  j=storage_->begin();
  for (k=0;k<i;++k)
    ++j;
  return *j;
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
//: Return the real type of a group. It is a SPATIALGROUP
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_group_2d::spatial_type(void) const
{
  return vsol_spatial_object_2d::SPATIALGROUP;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
// Require: size()>0
//---------------------------------------------------------------------------
void vsol_group_2d::compute_bounding_box(void)
{
  // require
  assert(size()>0);

  double xmin;
  double ymin;
  double xmax;
  double ymax;
  vcl_list<vsol_spatial_object_2d_sptr>::iterator i;

  for (i=storage_->begin();i!=storage_->end();++i)
    {
      vsol_box_2d *b=(*i)->get_bounding_box();
      if (i==storage_->begin())
        {
          xmin=b->get_min_x();
          ymin=b->get_min_y();
          xmax=b->get_max_x();
          ymax=b->get_max_y();
        }
      else
        {
          if (b->get_min_x()<xmin) xmin=b->get_min_x();
          if (b->get_min_y()<ymin) ymin=b->get_min_y();
          if (b->get_max_x()>xmax) xmax=b->get_max_x();
          if (b->get_max_y()>ymax) ymax=b->get_max_y();
        }
      delete b;
    }
  if (bounding_box_==0)
    bounding_box_=new vsol_box_2d;
  bounding_box_->set_min_x(xmin);
  bounding_box_->set_max_x(xmax);
  bounding_box_->set_min_y(ymin);
  bounding_box_->set_max_y(ymax);
}

//---------------------------------------------------------------------------
//: Return the number of direct children of the group
//---------------------------------------------------------------------------
int vsol_group_2d::size(void) const
{
  return storage_->size();
}

//---------------------------------------------------------------------------
//: Return the number of objects of the group
//---------------------------------------------------------------------------
int vsol_group_2d::deep_size(void) const
{
  int result = 0;
  vcl_list<vsol_spatial_object_2d_sptr>::iterator i;
  for (i=storage_->begin();i!=storage_->end();++i)
    {
      vsol_group_2d const* g=(*i)->cast_to_group();
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
void vsol_group_2d::add_object(const vsol_spatial_object_2d_sptr &new_object)
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
void vsol_group_2d::remove_object(const int i)
{
  // require
  assert((i>=0)&&(i<size()));

  vcl_list<vsol_spatial_object_2d_sptr>::iterator j;
  int k;

  j=storage_->begin();
  for (k=0;k<i;++k)
    ++j;
  storage_->erase(j);
}

//---------------------------------------------------------------------------
//: Is `new_object' a child (direct or not) of `this' ?
//---------------------------------------------------------------------------
bool
vsol_group_2d::is_child(const vsol_spatial_object_2d_sptr &new_object) const
{
  bool result=false;
  vcl_list<vsol_spatial_object_2d_sptr>::iterator i;
  for (i=storage_->begin();(i!=storage_->end())&&!result;++i)
    {
      result=(*i).ptr()==new_object.ptr();
      if (!result)
        {
          // g=dynamic_cast<vsol_group_2d const *>((*i).ptr());   // GOOD VERSION
          vsol_group_2d const* g=(vsol_group_2d const*)((*i).ptr()->cast_to_group()); // BAD HACK
          if (g!=0)
            g->is_child(new_object);
        }
    }
  return result;
}
