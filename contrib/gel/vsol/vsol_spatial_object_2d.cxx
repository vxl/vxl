// This is gel/vsol/vsol_spatial_object_2d.cxx
#include "vsol_spatial_object_2d.h"
//:
// \file

#include <vcl_iostream.h>

const float vsol_spatial_object_2d::eps=1.0e-3f;
int vsol_spatial_object_2d::tagcount_=0;

char * vsol_spatial_object_2d::SpatialTypes[] =
{
  "NO_TYPE             ",
  "TOPOLOGYOBJECT      ",
  "POINT               ",
  "CURVE               ",
  "REGION              ",
  "SPATIALGROUP        ",
  "NUM_SPATIALOBJECT_TYPES"
};

const char * vsol_spatial_object_2d::get_name() const
{
  vsol_spatial_object_2d_type type =spatial_type();
  if (type > 0 && type < vsol_spatial_object_2d::NUM_SPATIALOBJECT_TYPES)
    return SpatialTypes[type];
  else
    return SpatialTypes[0];
}

void vsol_spatial_object_2d::protected_destroy()
{
  if (this->get_references() <= 0)
    {
      vcl_cout << "goodbye cruel world\n";
      delete this;
    }
}

//: constructor initialize basic vsol_spatial_object_2d attributes.
//   bounding_box is set to NULL.
vsol_spatial_object_2d::vsol_spatial_object_2d(void)
{
  bounding_box_=0;
  id_ = 0;
  tag_ = 0;
  ++vsol_spatial_object_2d::tagcount_;
  set_tag_id(tagcount_);
  touch();
}


vsol_spatial_object_2d::vsol_spatial_object_2d(const vsol_spatial_object_2d &other)
{
  bounding_box_=0;
  id_=other.get_id();
  tag_=0;
  ++vsol_spatial_object_2d::tagcount_;
  set_tag_id(tagcount_);
}

vsol_spatial_object_2d::~vsol_spatial_object_2d()
{
}

void vsol_spatial_object_2d::not_applicable(const vcl_string &message) const
{
  vcl_cerr<<message<<"() function call not applicable\tfor spatial object "
          <<get_name()<<" !\n";
}

void iu_delete(vsol_spatial_object_2d* so)
{
  if (so)
    so->protected_destroy();
}

//---------------------------------------------------------------------------
//: Compute bounding box, do nothing in this case except touching the box
//---------------------------------------------------------------------------
void vsol_spatial_object_2d::compute_bounding_box(void) const
{
  if (!bounding_box_)
    bounding_box_ = new vsol_box_2d;
  bounding_box_->touch();
}

void vsol_spatial_object_2d::grow_minmax_bounds(vsol_box_2d & comp_box) const
{
  if (!bounding_box_)
    bounding_box_=new vsol_box_2d;
  bounding_box_->grow_minmax_bounds(comp_box);
}
