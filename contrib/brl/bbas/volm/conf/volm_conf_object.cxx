#include <volm/conf/volm_conf_object.h>
//:
// \file
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_limits.h>

#define EPSILON 1E-5

  //The angular value is from 0 to 2*pi

volm_conf_object::volm_conf_object(float const& theta, float const& dist, unsigned char const& land)
{
  dist_ = dist;  land_ = land;
  theta_ = theta;
  while (theta_ > vnl_math::twopi)
    theta_ -= (float)vnl_math::twopi;
  while (theta_ < 0)
    theta_ += (float)vnl_math::twopi;
}

volm_conf_object::volm_conf_object(double const& theta, double const& dist, unsigned char const& land)
{
  dist_ = (float)dist; land_ = land;
  theta_ = (float)theta;
  while (theta_ > vnl_math::twopi)
    theta_ -= (float)vnl_math::twopi;
  while (theta_ < 0)
    theta_ += (float)vnl_math::twopi;
}

// construct by the location point represented as (x,y)
volm_conf_object::volm_conf_object(vgl_point_2d<float> const& pt, unsigned char const& land)
{
  land_ = land;
  dist_ = vcl_sqrt(pt.x()*pt.x()+pt.y()*pt.y());
  float theta = vcl_atan2(pt.y(),pt.x());
  theta_ = (theta < 0) ? theta + (float)vnl_math::twopi : theta;
}

volm_conf_object::volm_conf_object(vgl_point_2d<double> const& pt, unsigned char const& land)
{
  land_ = land;
  dist_ = (float)vcl_sqrt(pt.x()*pt.x()+pt.y()*pt.y());
  float theta = (float)vcl_atan2(pt.y(), pt.x());
  theta_ = (theta < 0) ? theta + (float)vnl_math::twopi : theta;
}

float volm_conf_object::theta_in_deg() const  {  return theta_ / (float)vnl_math::pi_over_180;  }
float volm_conf_object::x() const  {  return dist_ * vcl_cos(theta_);  }
float volm_conf_object::y() const  {  return dist_ * vcl_sin(theta_);  }
vgl_point_2d<float> volm_conf_object::loc() const { return vgl_point_2d<float>(this->x(), this->y()); }

bool volm_conf_object::is_same(volm_conf_object const& other)
{
  return (vcl_fabs(this->theta_ - other.theta() ) < EPSILON) && (vcl_fabs(this->dist_ - other.dist() ) < EPSILON) && this->land_ == other.land();
}
bool volm_conf_object::is_same(volm_conf_object_sptr other_sptr)
{
  return this->is_same(*other_sptr);
}
bool volm_conf_object::is_same(volm_conf_object const* other_ptr)
{
  return this->is_same(*other_ptr);
}

// binary IO
void volm_conf_object::b_write(vsl_b_ostream& os) const
{
  unsigned ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, dist_);
  vsl_b_write(os, theta_);
  vsl_b_write(os, land_);
}

void volm_conf_object::b_read(vsl_b_istream& is)
{
  unsigned ver;
  vsl_b_read(is, ver);
  if (ver == this->version())
  {
    vsl_b_read(is, dist_);
    vsl_b_read(is, theta_);
    vsl_b_read(is, land_);
  }
  else
  {
    vcl_cout << "volm_conf_object: binary read -- unknown binary io version: " << ver << '\n';
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_object const& obj)
{
  obj.b_write(os);
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_object const* obj_ptr)
{
  if (obj_ptr == 0)
    vsl_b_write(os, false);
  else {
    vsl_b_write(os, true);
    vsl_b_write(os, *obj_ptr);
  }
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_object_sptr const& obj_sptr)
{
  vsl_b_write(os, obj_sptr.ptr());
}

void vsl_b_read(vsl_b_istream& is, volm_conf_object& obj)
{
  obj.b_read(is);
}

void vsl_b_read(vsl_b_istream& is, volm_conf_object*& obj_ptr)
{
  delete obj_ptr;  obj_ptr = 0;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    obj_ptr = new volm_conf_object();
    obj_ptr->b_read(is);
  }
}

void vsl_b_read(vsl_b_istream& is, volm_conf_object_sptr& obj_sptr)
{
  volm_conf_object* obj_ptr = 0;
  vsl_b_read(is, obj_ptr);
  obj_sptr = obj_ptr;
}
