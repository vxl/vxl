#include <vsol/vsol_line_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from the direction and the middle point
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vnl_vector_fixed<double,2> &new_direction,
                           const vsol_point_2d_sptr &new_middle)
{
  p0_=new vsol_point_2d(*(new_middle->plus_vector(-(new_direction)/2)));
  p1_=new vsol_point_2d(*(new_middle->plus_vector((new_direction)/2)));
}

//---------------------------------------------------------------------------
// -- Constructor
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vsol_point_2d_sptr &new_p0,
                           const vsol_point_2d_sptr &new_p1)
{
  p0_=new_p0;
  p1_=new_p1;
}

//---------------------------------------------------------------------------
// -- Copy constructor
// Description: no duplication of the points
//---------------------------------------------------------------------------
vsol_line_2d::vsol_line_2d(const vsol_line_2d &other)
{
  p0_=other.p0_;
  p1_=other.p1_;
}

//---------------------------------------------------------------------------
// -- Destructor
//---------------------------------------------------------------------------
vsol_line_2d::~vsol_line_2d()
{
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
//       See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_sptr vsol_line_2d::clone(void) const
{
  return new vsol_line_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Middle point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::middle(void) const
{
  return p0_->middle(*p1_);
}

//---------------------------------------------------------------------------
// -- direction of the straight line segment. Has to be deleted manually
//---------------------------------------------------------------------------
vnl_vector_fixed<double,2> *vsol_line_2d::direction(void) const
{
  vnl_vector_fixed<double,2> *result;

  result=p0_->to_vector(*p1_);

  return result;
}

//---------------------------------------------------------------------------
// -- First point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
// -- Last point of the straight line segment
//---------------------------------------------------------------------------
vsol_point_2d_sptr vsol_line_2d::p1(void) const
{
  return p1_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same points than `other' ?
//---------------------------------------------------------------------------
bool vsol_line_2d::operator==(const vsol_line_2d &other) const
{
  bool result;

  result=this==&other;
  if(!result)
    result=(((*p0_)==(*(other.p0_)))&&((*p1_)==(*(other.p1_))))
      ||(((*p0_)==(*(other.p1_)))&&((*p1_)==(*(other.p0_))));
  return result;
}

// -- spatial object equality

bool vsol_line_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::LINE
  ? *this == (vsol_line_2d const&) (vsol_curve_2d const&) obj
  : false;
}

//---------------------------------------------------------------------------
// -- Has `this' not the same points than `other' ?
//---------------------------------------------------------------------------
bool vsol_line_2d::operator!=(const vsol_line_2d &other) const
{
  return !operator==(other);
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the real type of a line. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_line_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
// -- Compute the bounding box of `this'
//---------------------------------------------------------------------------
void vsol_line_2d::compute_bounding_box(void)
{
  double xmin;
  double xmax;
  double ymin;
  double ymax;

  xmin=p0_->x();
  ymin=p0_->y();
  xmax=xmin;
  ymax=ymin;

  if(p1_->x()<xmin)
    xmin=p1_->x();
  else if(p1_->x()>xmax)
    xmax=p1_->x();
  if(p1_->y()<ymin)
    ymin=p1_->y();
  else if(p1_->y()>ymax)
    ymax=p1_->y();

  if(_bounding_box==0)
    _bounding_box=new vsol_box_2d();
  _bounding_box->set_min_x(xmin);
  _bounding_box->set_max_x(xmax);
  _bounding_box->set_min_y(ymin);
  _bounding_box->set_max_y(ymax);
}

//---------------------------------------------------------------------------
// -- Return the length of `this'
//---------------------------------------------------------------------------
double vsol_line_2d::length(void) const
{
  return p0_->distance(p1_);
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// -- Set the first point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_2d::set_p0(const vsol_point_2d_sptr &new_p0)
{
  p0_=new_p0;
}

//---------------------------------------------------------------------------
// -- Set the last point of the straight line segment
//---------------------------------------------------------------------------
void vsol_line_2d::set_p1(const vsol_point_2d_sptr &new_p1)
{
  p1_=new_p1;
}

//---------------------------------------------------------------------------
// -- Set the length of `this'. Doesn't change middle point and orientation.
//    If p0 and p1 are equal then the direction is set to (1,0)
// Require: new_length>=0
//---------------------------------------------------------------------------
void vsol_line_2d::set_length(const double new_length)
{
  // require
  assert(new_length>=0);

  vsol_point_2d_sptr m=middle();
  vnl_vector_fixed<double,2> *d=direction();

  if((*p0_)==(*p1_)) // ie. d=0 then d is set to (1,0)
    {
      (*d)[0]=1;
      (*d)[1]=0;
    }
  else
    d->normalize();

  (*d)*=new_length;

  p0_=new vsol_point_2d(*(m->plus_vector(-(*d)/2)));
  p1_=new vsol_point_2d(*(m->plus_vector((*d)/2)));

  delete d;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `p' in `this' ?
//---------------------------------------------------------------------------
bool vsol_line_2d::in(const vsol_point_2d_sptr &p) const
{
  bool result;
  double dot_product;

  // `p' belongs to the straight line
  result=(p0_->y()-p1_->y())*p->x()+(p1_->x()-p0_->x())*p->y()
    +p0_->x()*p1_->y()-p0_->y()*p1_->x()==0;

  if(result) // `p' belongs to the segment
    {
      dot_product=(p->x()-p0_->x())*(p1_->x()-p0_->x())
        +(p->y()-p0_->y())*(p1_->y()-p0_->y());
      result=(dot_product>=0)&&
        (dot_product<(vnl_math_sqr(p1_->x()-p0_->x())
                      +vnl_math_sqr(p1_->y()-p0_->y())));
    }
  return result;
}

//---------------------------------------------------------------------------
// -- Return the tangent to `this' at `p'.  Has to be deleted manually
// Require: in(p)
//---------------------------------------------------------------------------
vgl_homg_line_2d<double> *
vsol_line_2d::tangent_at_point(const vsol_point_2d_sptr &p) const
{
  // require
  // assert(in(p));

  vgl_homg_line_2d<double> *result;

  result=new vgl_homg_line_2d<double>(p0_->y()-p1_->y(),p1_->x()-p0_->x(),
                                      p0_->x()*p1_->y()-p0_->y()*p1_->x());

  return result;
}
