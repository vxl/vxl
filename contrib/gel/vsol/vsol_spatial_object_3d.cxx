#include <vsol/vsol_spatial_object_3d.h>

#include <vcl_iostream.h>

const float vsol_spatial_object_3d::eps = 1.0e-3f;
int vsol_spatial_object_3d::_tagcount = 0;

char * vsol_spatial_object_3d::SpatialTypes[] = {"NO_TYPE             ",
                        "TOPOLOGYOBJECT      ",
                        "POINT               ",
                        "CURVE               ",
                        "REGION              ",
                        "VOLUME              ",
                        "SPATIALGROUP        ",
                        "NUM_SPATIALOBJECT_TYPES"};

const char*    vsol_spatial_object_3d::get_name() const
{
  vsol_spatial_object_3d_type type =spatial_type();
  if (type > 0 && type < vsol_spatial_object_3d::NUM_SPATIALOBJECT_TYPES)
    return SpatialTypes[type];
  else
    return SpatialTypes[0];
}

void vsol_spatial_object_3d::protected_destroy()
{
  if(this->get_references() <= 0)
    {
      vcl_cout << "goodbye crule world " << vcl_endl;
      delete this;
    }
}


vsol_spatial_object_3d::~vsol_spatial_object_3d()
{
  if(_bounding_box)
    _bounding_box->unref();
}


void iu_delete(vsol_spatial_object_3d* so)
{
  if(so)
    so->protected_destroy();
}

//---------------------------------------------------------------------------
//: The same behavior than dynamic_cast<>. Needed because VXL is not compiled with -frtti :-(
//---------------------------------------------------------------------------
const vsol_group_3d *vsol_spatial_object_3d::cast_to_group(void) const
{
  return 0;
}

#if 0 // the following functions are not (yet) supported

//------------------------------------------------------------
//: Perform an arbitrary projective transformation to all points of the obj.
// Note that this will in general not be supported by objects which are not
// projectively invariant, like circles or ellipses, so observing the return
// value is important.  (E.g., circles should implement this method, but only
// perform it if the resulting curve is again a circle.  Maybe some tolerance
// could be taken into account.)
bool
vsol_spatial_object_3d::Transform(CoolTransform const& )
{
  vcl_cerr << "Transform() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//: Return the projective transformation needed to move GetLocation() to (0,0,0),
// GetOrientation() to (1,0,0), GetNormal() to (0,0,1) and GetSize() to (1,1,1).
CoolTransform vsol_spatial_object_3d::GetTransformation() const
{
  vcl_cerr << "GetTransformation() not implemented for this " << GetName() << vcl_endl;
  return CoolTransform::identity();
}

//------------------------------------------------------------
//: leave GetLocation() fixed, move all other points towards or away from it
// by the given scale factor.  Smaller than 1 means towards.  Negative moves the
// points to the other side of GetLocation().
bool
vsol_spatial_object_3d::LinearScale(float )
{
  vcl_cerr << "LinearScale() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//: leave GetOrientation() fixed, translate all points so that GetLocation()
// moves to (0,0,0).
bool
vsol_spatial_object_3d::Translate()
{
  vcl_cerr << "Translate() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//: leave GetLocation() fixed, rotate all other points around it so that
// GetOrientation() moves to (1,0,0).
bool
vsol_spatial_object_3d::Rotate()
{
  vcl_cerr << "Rotate() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//: leave GetOrientation() fixed, rotate all other points around it over 180
// degrees.
bool
vsol_spatial_object_3d::Reflect()
{
  vcl_cerr << "Reflect() not implemented for this " << GetName() << vcl_endl;
  return false;
}

//------------------------------------------------------------
//: leave GetOrientation() fixed, move all other points parallel to it,
// over a distance which is proportional to its distance to GetOrientation().
// A right angle becomes an angle of a (in degrees).
bool
vsol_spatial_object_3d::Shear(float )
{
  vcl_cerr << "Shear() not implemented for this " << GetName() << vcl_endl;
  return false;
}

#endif

void vsol_spatial_object_3d::grow_minmax_bounds(vsol_box_3d & comp_box)
{
  if(_bounding_box==0)
    {
    _bounding_box=new vsol_box_3d;
    }
  _bounding_box->grow_minmax_bounds(comp_box);
}

//#include <vcl_rel_ops.h> // gcc 2.7
//VCL_INSTANTIATE_INLINE(bool operator!=(vsol_spatial_object_3d const &, vsol_spatial_object_3d const &));
