#include "volm_conf_score.h"
//:
// \file
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_limits.h>

// note the angular value is from 0 to 2*pi and 0 refers to the east direction

volm_conf_score::volm_conf_score(float const& score, float const& theta) : score_(score), theta_(theta)
{
  while (theta_ > vnl_math::twopi)
    theta_ -= (float)vnl_math::twopi;
  while (theta_ < 0)
    theta_ += (float)vnl_math::twopi;
}

float volm_conf_score::theta_in_deg() const
{
  return theta_ / (float)vnl_math::pi_over_180;
}

// return the angular value (in degree) relative to north
float volm_conf_score::theta_to_north() const
{
  return (theta_ - (float)vnl_math::pi_over_2) / (float)vnl_math::pi_over_180;
}

// binary IO
void volm_conf_score::b_write(vsl_b_ostream& os) const
{
  unsigned char ver = this->version();
  vsl_b_write(os, ver);
  vsl_b_write(os, score_);
  vsl_b_write(os, theta_);
}

void volm_conf_score::b_read(vsl_b_istream& is)
{
  unsigned char ver;
  vsl_b_read(is, ver);
  if (ver == this->version()) {
    vsl_b_read(is, score_);
    vsl_b_read(is, theta_);
  }
  else {
    vcl_cout << "volm_conf_score: binary read -- unknown binary io version: " << (int)ver << ", most updated version is " << this->version() << '\n';
    return;
  }
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_score const& score)
{
  score.b_write(os);
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_score const* score_ptr)
{
  if (score_ptr == 0)
    vsl_b_write(os,false);
  else {
    vsl_b_write(os, true);
    vsl_b_write(os, *score_ptr);
  }
}

void vsl_b_write(vsl_b_ostream& os, volm_conf_score_sptr const& score_sptr)
{
  vsl_b_write(os, score_sptr.ptr());
}

void vsl_b_read(vsl_b_istream& is, volm_conf_score& score)
{
  score.b_read(is);
}

void vsl_b_read(vsl_b_istream& is, volm_conf_score*& score_ptr)
{
  delete score_ptr;  score_ptr = 0;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    score_ptr = new volm_conf_score();
    score_ptr->b_read(is);
  }
}

void vsl_b_read(vsl_b_istream& is, volm_conf_score_sptr& score_sptr)
{
  volm_conf_score* score_ptr = 0;
  vsl_b_read(is, score_ptr);
  score_sptr = score_ptr;
}