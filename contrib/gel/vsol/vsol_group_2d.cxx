// This is gel/vsol/vsol_group_2d.cxx
#include "vsol_group_2d.h"
//:
// \file
#include <vcl_cassert.h>
#include <vsl/vsl_string_io.h>
#include <vsl/vsl_vector_io.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
//: Default Constructor: with no child
//---------------------------------------------------------------------------
vsol_group_2d::vsol_group_2d(void)
  : vsol_spatial_object_2d()
{
  storage_=new vcl_vector<vsol_spatial_object_2d_sptr>();
}

//---------------------------------------------------------------------------
//: Copy constructor.
// Description: The objects of the group are not duplicated
//---------------------------------------------------------------------------
vsol_group_2d::vsol_group_2d(const vsol_group_2d &other)
  : vsol_spatial_object_2d(other)
{
  storage_=new vcl_vector<vsol_spatial_object_2d_sptr>(*other.storage_);
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
vsol_spatial_object_2d* vsol_group_2d::clone(void) const
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
vsol_spatial_object_2d_sptr vsol_group_2d::object(unsigned int i) const
{
  // require
  assert(i<size());

  vcl_vector<vsol_spatial_object_2d_sptr>::iterator j = storage_->begin();
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
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_group_2d::spatial_type(void) const
{
  return vsol_spatial_object_2d::SPATIALGROUP;
}

//---------------------------------------------------------------------------
//: Compute the bounding box of `this'
// Require: size()>0
//---------------------------------------------------------------------------
void vsol_group_2d::compute_bounding_box(void) const
{
  // require
  assert(size()>0);

  vcl_vector<vsol_spatial_object_2d_sptr>::iterator i = storage_->begin();
  set_bounding_box(   (*i)->get_min_x(), (*i)->get_min_y());
  add_to_bounding_box((*i)->get_max_x(), (*i)->get_max_y());
  for (++i; i!=storage_->end(); ++i)
  {
    add_to_bounding_box((*i)->get_min_x(), (*i)->get_min_y());
    add_to_bounding_box((*i)->get_max_x(), (*i)->get_max_y());
  }
}

//---------------------------------------------------------------------------
//: Return the number of objects of the group
//---------------------------------------------------------------------------
unsigned int vsol_group_2d::deep_size(void) const
{
  int result = 0;
  vcl_vector<vsol_spatial_object_2d_sptr>::iterator i;
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
void vsol_group_2d::remove_object(unsigned int i)
{
  // require
  assert(i<size());

  vcl_vector<vsol_spatial_object_2d_sptr>::iterator j = storage_->begin();
  for (unsigned int k=0;k<i;++k)
    ++j;
  storage_->erase(j);
}

//---------------------------------------------------------------------------
//: Is `new_object' a child (direct or not) of `this' ?
//---------------------------------------------------------------------------
bool
vsol_group_2d::is_child(const vsol_spatial_object_2d_sptr &new_object) const
{
  vcl_vector<vsol_spatial_object_2d_sptr>::iterator i;
  for (i=storage_->begin(); i!=storage_->end(); ++i)
  {
    if ((*i).ptr()==new_object.ptr())
      return true;
    vsol_group_2d const* g=(*i)->cast_to_group();
    if (g!=0 && g->is_child(new_object))
      return true;
  }
  return false;
}

bool vsol_group_2d::operator==(const vsol_group_2d &other) const
{
  //groups must have the same number of shallow elements
  if (this->size()!= other.size())
    return false;
  //groups must have the same number of deep (flattened) elements
  if (this->deep_size()!= other.deep_size())
    return false;
  //groups must have the same elements and in the same order
  for (unsigned int i = 0; i<this->size(); i++)
    if (*(this->object(i))!=*(other.object(i)))
      return false;
  return true;
}

bool vsol_group_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return obj.cast_to_group() && *this == *obj.cast_to_group();
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
void vsol_group_2d::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, *storage_);
#if 0
  for (unsigned int i = 0; i<this->size(); i++)
  {
    vsol_spatial_object_2d_sptr so = this->object(i);
    vsol_point_2d_sptr p = so->cast_to_point();
    if (p)
    {
      vsl_b_write(os, p->is_a());
      vsl_b_write(os, p);
      continue;
    }
    vsol_curve_2d* c = so->cast_to_curve();
    if (c)
    {
      vsol_line_2d_sptr l = c->cast_to_line();
      if (l)
      {
        vsl_b_write(os, l->is_a());
        vsl_b_write(os, l);
        continue;
      }
      vsol_conic_2d_sptr cn = c->cast_to_conic();
      if (cn)
      {
        vsl_b_write(os, cn->is_a());
        vsl_b_write(os, cn);
        continue;
      }
      vsol_polyline_2d_sptr pl = c->cast_to_polyline();
      if (pl)
      {
        vsl_b_write(os, pl->is_a());
        vsl_b_write(os, pl);
        continue;
      }
    }
    vsol_region_2d* r = so->cast_to_region();
    if (r)
    {
      vsol_polygon_2d_sptr pg = r->cast_to_polygon();
      if (pg)
      {
        vsol_triangle_2d_sptr tr = pg->cast_to_triangle();
        if (tr)
        {
          vsl_b_write(os, tr->is_a());
          vsl_b_write(os, tr);
          continue;
        }
        vsol_rectangle_2d_sptr rc = pg->cast_to_rectangle_2d();
        if (rc)
        {
          vsl_b_write(os, rc->is_a());
          vsl_b_write(os, rc);
          continue;
        }
        vsl_b_write(os, pg->is_a());
        vsl_b_write(os, pg);
        continue;
      }
    }
    vsol_group_2d* g = so->cast_to_group();
    if (g)
    {
      vsl_b_write(os, g->is_a());
      g->b_write(os);
    }
  }
  vsl_b_write(os, vcl_string("vsol_group_2d_end"));
#endif // 0
}

//: Binary load self from stream (not typically used)
void vsol_group_2d::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   default:
    assert(!"vsol_group_2d I/O version should be 1");
   case 1:
    vsl_b_read(is, *storage_);
#if 0
    vcl_string type;
    while (true)
    {
      vsl_b_read(is, type);
      if (type=="vsol_point_2d")
      {
        vsol_point_2d_sptr p;
        vsl_b_read(is, p);
        if (p)
          storage_->push_back((vsol_point_2d*)p.ptr());
      }
      if (type=="vsol_line_2d")
      {
        vsol_line_2d_sptr l;
        vsl_b_read(is, l);
        if (l)
          storage_->push_back((vsol_line_2d*)l.ptr());
      }
      if (type=="vsol_conic_2d")
      {
        vsol_conic_2d_sptr cn;
        vsl_b_read(is, cn);
        if (cn)
          storage_->push_back((vsol_conic_2d*)cn.ptr());
      }
      if (type=="vsol_polyline_2d")
      {
        vsol_polyline_2d_sptr pl;
        vsl_b_read(is, pl);
        if (pl)
          storage_->push_back((vsol_polyline_2d*)pl.ptr());
      }
      if (type=="vsol_polygon_2d")
      {
        vsol_polygon_2d_sptr pg;
        vsl_b_read(is, pg);
        if (pg)
          storage_->push_back((vsol_polygon_2d*)pg.ptr());
      }
      if (type=="vsol_triangle_2d")
      {
        vsol_triangle_2d_sptr t;
        vsl_b_read(is, t);
        if (t)
          storage_->push_back((vsol_triangle_2d*)t.ptr());
      }
      if (type=="vsol_rectangle_2d")
      {
        vsol_rectangle_2d_sptr r;
        vsl_b_read(is, r);
        if (r)
          storage_->push_back((vsol_rectangle_2d*)r.ptr());
      }
      if (type=="vsol_group_2d")
      {
        vsol_group_2d_sptr g;
        vsl_b_read(is, g);
        if (g)
          storage_->push_back((vsol_group_2d*)g.ptr());
      }
      if (type=="vsol_group_2d_end")
        return;
    }
#endif // 0
  }
}

//: Return IO version number;
short vsol_group_2d::version() const
{
  return 1;
}

//: Print an ascii summary to the stream
void vsol_group_2d::print_summary(vcl_ostream &os) const
{
  os << *this;
}

//external functions

//: Binary save vsol_group_2d* to stream.
void
vsl_b_write(vsl_b_ostream &os, const vsol_group_2d* g)
{
  if (!g)
    vsl_b_write(os,false); // Indicate null pointer stored
  else {
    vsl_b_write(os,true);  // Indicate non-null pointer stored
    g->b_write(os);
  }
}

//: Binary load vsol_group_2d* from stream.
void
vsl_b_read(vsl_b_istream &is, vsol_group_2d* &g)
{
  delete g;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
    g = new vsol_group_2d();
    g->b_read(is);
  }
  else
    g = 0;
}

