#include <vsol/vsol_conic_2d.h>

//*****************************************************************************
// External declarations for implementation
//*****************************************************************************
#include <vcl_cassert.h>
#include <vsol/vsol_point_2d.h>

//***************************************************************************
// Initialization
//***************************************************************************

//---------------------------------------------------------------------------
// -- Constructor from coefficient of the cartesian equation
// Description: `new_a'x^2+`new_b'xy+`new_c'y^2+`new_d'x+`new_e'y+`new_f'
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(double new_a,
                             double new_b,
                             double new_c,
                             double new_d,
                             double new_e,
                             double new_f) :
  a_(new_a), b_(new_b), c_(new_c), d_(new_d), e_(new_e), f_(new_f)
{
}

//---------------------------------------------------------------------------
// Copy constructor
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_2d(const vsol_conic_2d &other) :
  a_(other.a_),b_(other.b_),c_(other.c_),d_(other.d_),e_(other.e_),f_(other.f_)
{
}

//---------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------
vsol_conic_2d::~vsol_conic_2d()
{
}

//---------------------------------------------------------------------------
// -- Clone `this': creation of a new object and initialization
// See Prototype pattern
//---------------------------------------------------------------------------
vsol_spatial_object_2d_ref vsol_conic_2d::clone(void) const
{
  return new vsol_conic_2d(*this);
}

//***************************************************************************
// Access
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the first point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_ref vsol_conic_2d::p0(void) const
{
  return p0_;
}

//---------------------------------------------------------------------------
// -- Return the last point of `this'
//---------------------------------------------------------------------------
vsol_point_2d_ref vsol_conic_2d::p1(void) const
{
  return p1_;
}

//***************************************************************************
// Comparison
//***************************************************************************

//---------------------------------------------------------------------------
// -- Has `this' the same coefficients than `other' ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::operator==(const vsol_conic_2d &other) const
{
  double lambda;

  if (a_!=0)
    lambda=other.a_/a_;
  else if (b_!=0)
    lambda=other.b_/b_;
  else if (c_!=0)
    lambda=other.c_/c_;
  else if (d_!=0)
    lambda=other.d_/d_;
  else if (e_!=0)
    lambda=other.e_/e_;
  else if (f_!=0)
    lambda=other.f_/f_;
  else return false;

  return are_equal(a_*lambda,other.a_)&&are_equal(b_*lambda,other.b_)
    &&are_equal(c_*lambda,other.c_)&&are_equal(d_*lambda,other.d_)
    &&are_equal(e_*lambda,other.e_)&&are_equal(f_*lambda,other.f_);
}

// -- spatial object equality

bool vsol_conic_2d::operator==(const vsol_spatial_object_2d& obj) const
{
  return
   obj.spatial_type() == vsol_spatial_object_2d::CURVE &&
   ((vsol_curve_2d const&)obj).curve_type() == vsol_curve_2d::CONIC
  ? *this == (vsol_conic_2d const&) (vsol_curve_2d const&) obj
  : false;
}

//---------------------------------------------------------------------------
// -- Has `this' not the same coefficients than `other' ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::operator!=(const vsol_conic_2d &other) const
{
  return !operator==(other);
}

//***************************************************************************
// Status report
//***************************************************************************

//---------------------------------------------------------------------------
// -- Return the real type of a conic. It is a CURVE
//---------------------------------------------------------------------------
vsol_spatial_object_2d::vsol_spatial_object_2d_type
vsol_conic_2d::spatial_type(void) const
{
  return CURVE;
}

//---------------------------------------------------------------------------
// -- Find the real type of the conic from its coefficients
//---------------------------------------------------------------------------
vsol_conic_2d::vsol_conic_type vsol_conic_2d::real_type(void) const
{
  vsol_conic_type result;
  double det1; // determinant: det
  double det2;   // upper 2x2 determinant: j
  double sum;   // sum of two other 2x2 determinants: k
  const double tr = a_+c_; // trace of upper 2x2: i
  const double b2 = b_/2;
  const double d2 = d_/2;
  const double e2 = e_/2;

  det1=a_*(c_*f_-e2*e2)-b2*(b2*f_-d2*e2)+d2*(b2*e2-c_*d2);
  det2=a_*c_-b2*b2;
  sum=(c_*f_-e2*e2)+(a_*f_-d2*d2);

  if (!is_zero(det1))
  {
    if (det2>0)
    {
      if (det1*tr<0)
      {
        if (are_equal(a_,c_)&&is_zero(b2))
          result=real_circle;
        else
          result=real_ellipse;
      }
      else
      {
        if (are_equal(a_,c_)&&is_zero(b2))
          result=complex_circle;
        else
          result=complex_ellipse;
      }
    }
    else if (det2<0)
      result=hyperbola;
    else
      result=parabola;
  }
  else
  {    // limiting cases
    if (det2<0)
      result=real_intersecting_lines;
    else if (det2>0)
      result=complex_intersecting_lines;
    else
    {
      if (is_zero(a_)&&is_zero(b2)&&is_zero(c_))
      {
        if (is_zero(d2)&&is_zero(e2))
          result=degenerate;
        else
          result=coincident_lines;
      }
      else if (sum<0)
        result=real_parallel_lines;
      else if (sum>0)
        result=complex_parallel_lines;
      else
        result=coincident_lines;
    }
  }
  return result;
}

//---------------------------------------------------------------------------
// -- Is `this' an real ellipse ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_ellipse(void) const
{
 vsol_conic_type tmp;

  tmp=real_type();

  return (tmp==real_ellipse)||(tmp==real_circle);
}

//---------------------------------------------------------------------------
// -- Is `this' a real circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_circle(void) const
{
  return real_type()==real_circle;
}

//---------------------------------------------------------------------------
// -- Is `this' a complex ellipse ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_ellipse(void) const
{
  vsol_conic_type tmp;

  tmp=real_type();

  return (tmp==complex_ellipse)||(tmp==complex_circle);
}

//---------------------------------------------------------------------------
// -- Is `this' a complex circle ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_circle(void) const
{
  return real_type()==complex_circle;
}

//---------------------------------------------------------------------------
// -- Is `this' a parabola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_parabola(void) const
{
  return real_type()==parabola;
}

//---------------------------------------------------------------------------
// -- Is `this' a hyperbola ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_hyperbola(void) const
{
  return real_type()==hyperbola;
}

//---------------------------------------------------------------------------
// -- Is `this' an real intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_real_intersecting_lines(void) const
{
  return real_type()==real_intersecting_lines;
}

//---------------------------------------------------------------------------
// -- Is `this' an complex intersecting lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_complex_intersecting_lines(void) const
{
  return real_type()==complex_intersecting_lines;
}

//---------------------------------------------------------------------------
// -- Is `this' an coincident lines ?
//---------------------------------------------------------------------------
bool vsol_conic_2d::is_coincident_lines(void) const
{
  return real_type()==coincident_lines;
}

//---------------------------------------------------------------------------
// -- Return 3 ellipse parameters: centre (`cx',`cy'),
//                                 orientation `phi',
//                                 size (`width',`height')
// Require: is_real_ellipse()
//---------------------------------------------------------------------------
void vsol_conic_2d::ellipse_parameters(double &cx,
                                       double &cy,
                                       double &phi,
                                       double &width,
                                       double &height) const
{
  // require
  assert(is_real_ellipse());

  const double b2=b_/2;
  const double d2=d_/2;
  const double e2=e_/2;
  const double det=a_*c_-b2*b2;

  if (is_zero(b2)) // only for accuracy
  {
    cx=-d2/a_;
    cy=-e2/c_;
  }
  else
  {
    cx=(b2*e2-c_*d2)/det;
    cy=(b2*d2-a_*e2)/det;
  }

  double f0=a_*cx*cx+b_*cx*cy+c_*vnl_math_sqr(cy)+d_*cx+e_*cy+f_;

  if (is_zero(f0)) // avoid dividing by zero
    f0=1;
  const double a0=-a_/f0;
  const double b0=-b2/f0;
  const double c0=-c_/f0;

  if (are_equal(a0,c0)&&is_zero(b0))
    phi=0; // circle
  else
    phi=atan2(-2*b0,c0-a0)/2; //ellipse
  
  const double cosphi=cos(phi);
  const double sinphi=sin(phi);
  width=vnl_math_sqrt(1/(a0*cosphi*cosphi+2*b0*cosphi*sinphi
                         +c0*sinphi*sinphi));
  height=vnl_math_sqrt(1/(a0*sinphi*sinphi-2*b0*cosphi*sinphi
                          +c0*cosphi*cosphi));
}

//---------------------------------------------------------------------------
// -- Return 2 parabola parameters: top (`cx',`cy'),
//                                  orientation (`cosphi',`sinphi')
// Require: is_parabola()
//---------------------------------------------------------------------------
void vsol_conic_2d::parabola_parameters(double &cx,
                                        double &cy,
                                        double &cosphi,
                                        double &sinphi) const
{
  // require
  assert(is_parabola());

  // Note that for a parabola B*B == 4*A*C, hence the quadratic part
  // of the equation is a square: (nX+mY)^2, with n=sqrt(A), m=sqrt(C)
  // Hence norm cannot be zero since the parabola is not degererate:
  const double norm=a_+c_;
  // The parabola direction is then (-m,n):
  cosphi=-vnl_math_sqrt(c_/norm);
  sinphi=vnl_math_sqrt(a_/norm);
  // Finally, the top can be found as the point with tangent direction
  // orthogonal to the direction of the axis:
  // TODO
}

//---------------------------------------------------------------------------
// -- Return the length of `this'
//---------------------------------------------------------------------------
double vsol_conic_2d::length(void) const
{
  assert(false); // TO DO
  return -1;
}

//---------------------------------------------------------------------------
// -- Return the matrix associated to the coefficients. Has to be deleted manually
//---------------------------------------------------------------------------
vnl_double_3x3 *vsol_conic_2d::matrix(void) const
{
  vnl_double_3x3 *result;

  result=new vnl_double_3x3();

  // row 0
  result->put(0,0,a_);
  result->put(0,1,b_/2);
  result->put(0,2,d_/2);
  // row 1
  result->put(1,0,b_/2);
  result->put(1,1,c_);
  result->put(1,2,e_/2);
  // row 2
  result->put(2,0,d_/2);
  result->put(2,1,e_/2);
  result->put(2,2,f_);
 
  return result;
}

//***************************************************************************
// Status setting
//***************************************************************************

//---------------------------------------------------------------------------
// -- Set the first point of the curve
// Require: in(new_p0)
//---------------------------------------------------------------------------
void vsol_conic_2d::set_p0(const vsol_point_2d_ref &new_p0)
{
  // require
  assert(in(new_p0));

  p0_=new_p0;
}

//---------------------------------------------------------------------------
// -- Set the last point of the curve
// Require: in(new_p1)
//---------------------------------------------------------------------------
void vsol_conic_2d::set_p1(const vsol_point_2d_ref &new_p1)
{
  // require
  assert(in(new_p1));

  p1_=new_p1;
}

//***************************************************************************
// Basic operations
//***************************************************************************

//---------------------------------------------------------------------------
// -- Is `p' in `this' ? (ie `p' verifies the equation)
//---------------------------------------------------------------------------
bool vsol_conic_2d::in(const vsol_point_2d_ref &p) const
{
  const double x=p->x();
  const double y=p->y();
  return is_zero(a_*vnl_math_sqr(x)+b_*x*y+c_*vnl_math_sqr(y)+d_*x+e_*y+f_);
}
