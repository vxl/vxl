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
      vcl_cout << "goodbye crule world " << vcl_endl;
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

int vsol_spatial_object_2d::not_applicable(const vcl_string &message) const
{
  vcl_cerr<<message<<"() function call not applicable\tfor spatial object "
          <<get_name()<<" !\n";
  return 0;
}

void iu_delete(vsol_spatial_object_2d* so)
{
  if (so)
    so->protected_destroy();
}

#if 0 // following functions are not (yet) supported

//------------------------------------------------------------
//: Perform an arbitrary projective transformation to all points of the obj.
// Note that this will in general not be supported by objects which are not
// projectively invariant, like circles or ellipses, so observing the return
// value is important.  (E.g., circles should implement this method, but only
// perform it if the resulting curve is again a circle.  Maybe some tolerance
// could be taken into account.)
bool
vsol_spatial_object_2d::Transform(CoolTransform const& )
{
  vcl_cerr << "Transform() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//:
// Return the projective transformation needed to move GetLocation() to (0,0,0),
// GetOrientation() to (1,0,0), GetNormal() to (0,0,1) and GetSize() to (1,1,1).
CoolTransform vsol_spatial_object_2d::GetTransformation() const
{
  vcl_cerr << "GetTransformation() not implemented for this " << GetName() << vcl_endl;
  return CoolTransform::identity();
}

//------------------------------------------------------------
//:
// Leave GetLocation() fixed, move all other points towards or away from it
// by the given scale factor.  Smaller than 1 means towards.  Negative moves the
// points to the other side of GetLocation().
bool
vsol_spatial_object_2d::LinearScale(float )
{
  vcl_cerr << "LinearScale() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//:
// Leave GetOrientation() fixed, translate all points so that GetLocation()
// moves to (0,0,0).
bool
vsol_spatial_object_2d::Translate()
{
  vcl_cerr << "Translate() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//:
// Leave GetLocation() fixed, rotate all other points around it so that
// GetOrientation() moves to (1,0,0).
bool
vsol_spatial_object_2d::Rotate()
{
  vcl_cerr << "Rotate() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//:
// Leave GetOrientation() fixed, rotate all other points around it over 180
// degrees.
bool
vsol_spatial_object_2d::Reflect()
{
  vcl_cerr << "Reflect() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//:
// Leave GetOrientation() fixed, move all other points parallel to it,
// over a distance which is proportional to its distance to GetOrientation().
// A right angle becomes an angle of a (in degrees).
bool
vsol_spatial_object_2d::Shear(float )
{
  vcl_cerr << "Shear() not implemented for this " << GetName() << vcl_endl;
  return false;
}
#endif

//---------------------------------------------------------------------------
//: Compute bounding box, do nothing in this case
//---------------------------------------------------------------------------
void vsol_spatial_object_2d::compute_bounding_box(void)
{
  if (!bounding_box_)
    bounding_box_ = new vsol_box_2d;
  bounding_box_->touch();
}

void vsol_spatial_object_2d::grow_minmax_bounds(vsol_box_2d & comp_box)
{
  if (!bounding_box_)
    bounding_box_=new vsol_box_2d;
  bounding_box_->grow_minmax_bounds(comp_box);
}
