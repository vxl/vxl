// This is gel/vsol/vsol_spatial_object_3d.cxx
#include "vsol_spatial_object_3d.h"
//:
// \file

#include <vcl_iostream.h>

const float vsol_spatial_object_3d::eps = 1.0e-3f;
int vsol_spatial_object_3d::tagcount_ = 0;

const char * vsol_spatial_object_3d::SpatialTypes[] =
{
  "NO_TYPE             ",
  "TOPOLOGYOBJECT      ",
  "POINT               ",
  "CURVE               ",
  "REGION              ",
  "VOLUME              ",
  "SPATIALGROUP        ",
  "NUM_SPATIALOBJECT_TYPES"
};

const char * vsol_spatial_object_3d::get_name() const
{
  vsol_spatial_object_3d_type type =spatial_type();
  if (type > 0 && type < vsol_spatial_object_3d::NUM_SPATIALOBJECT_TYPES)
    return SpatialTypes[type];
  else
    return SpatialTypes[0];
}

void vsol_spatial_object_3d::protected_destroy()
{
  if (this->get_references() <= 0)
  {
    vcl_cout << "goodbye cruel world\n";
    delete this;
  }
}

vsol_spatial_object_3d::~vsol_spatial_object_3d()
{
}

void iu_delete(vsol_spatial_object_3d* so)
{
  if (so)
    so->protected_destroy();
}

void vsol_spatial_object_3d::grow_minmax_bounds(vsol_box_3d & comp_box) const
{
  if (!bounding_box_)
    bounding_box_=new vsol_box_3d;
  bounding_box_->grow_minmax_bounds(comp_box);
}
