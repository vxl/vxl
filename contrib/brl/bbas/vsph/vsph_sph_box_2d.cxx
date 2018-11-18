// This is bbas/vsph/vsph_sph_box_2d.cxx
#include "vsph_sph_box_2d.h"
#include "vsph_utils.h"
#include "vsph_defs.h"
#include "vsph_unit_sphere.h"
#include <bvrml/bvrml_write.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>

double vsph_sph_box_2d::pye() const
{
  if (in_radians_) return vnl_math::pi;
  return 180.0;
}

double vsph_sph_box_2d::reduce_phi(double phi) const
{
  double ph = phi;
  if (ph>pye()) ph -= vnl_math::twopi;
  if (ph<-pye()) ph +=  vnl_math::twopi;
  return ph;
}

void vsph_sph_box_2d::b_ccw_a()
{
  if (!defined()) return;
  double dif = vsph_utils::azimuth_diff(a_phi_, b_phi_, in_radians_);
  if (dif<0) {
    double a_ph = a_phi_;
    a_phi_ = b_phi_;
    b_phi_ = a_ph;
  }
}

void vsph_sph_box_2d::set_comparisons()
{
  a_ge_zero_ = a_phi_ >= 0.0;
  b_le_zero_ = b_phi_ <= 0.0;
  b_lt_zero_ = b_phi_ < 0.0;
  c_gt_a_ = c_phi_> a_phi_;
  c_lt_b_ = c_phi_ < b_phi_;
  c_le_pi_ = c_phi_<=pye();
  c_ge_mpi_ = c_phi_>=-pye();
  c_ge_zero_ = c_phi_>=0.0;
}

void vsph_sph_box_2d::phi_bounds(double& phi_start, double& phi_end) const
{
  // assume that b_phi_ is ccw of a_phi_
  double ph_start = a_phi_, ph_end = b_phi_;
  phi_start = ph_start; phi_end = ph_end;
  // start and end the same sign
  if ((!a_ge_zero_ && b_lt_zero_)||(a_ge_zero_ && !b_lt_zero_)) {
    double mid = 0.5*(ph_start + ph_end);
    if (!(this->in_interval(mid, in_radians_))) {
      phi_start = ph_end;
      phi_end = ph_start;
    }
    return;
  }
  // signs are different
  if (!a_ge_zero_&& !b_lt_zero_) {
    if (this->in_interval(pye(), in_radians_)) {
      phi_start = ph_end;
      phi_end = ph_start;
    }
    return;
  }
  if (a_ge_zero_ && b_lt_zero_) {
    if (!(this->in_interval(pye(), in_radians_))) {
      phi_start = ph_end;
      phi_end = ph_start;
    }
    return;
  }
  assert(false);
  std::cout<< "IMPOSSIBLE CONDITION in phi_bounds(..)\n";
}

vsph_sph_box_2d::vsph_sph_box_2d(): in_radians_(true)
{
  min_th_ = max_th_= 1000.0; //empty
  a_phi_ = b_phi_ = c_phi_ = 1000.0;
}

vsph_sph_box_2d::vsph_sph_box_2d(bool in_radians): in_radians_(in_radians)
{
  min_th_ = max_th_= 1000.0; //empty
  a_phi_ = b_phi_ = c_phi_ = 1000.0;
  min_phi_ = max_phi_ = 1000.0;
}

vsph_sph_box_2d::vsph_sph_box_2d(const vsph_sph_box_2d& sbox)
{
  in_radians_ = sbox.in_radians();
  a_phi_ = sbox.a_phi(in_radians_);
  b_phi_ = sbox.b_phi(in_radians_);
  c_phi_ = sbox.c_phi(in_radians_);
  min_th_ = sbox.min_theta(in_radians_);
  max_th_ = sbox.max_theta(in_radians_);
  min_phi_ = sbox.min_phi(in_radians_);
  max_phi_ = sbox.max_phi(in_radians_);
  this->set_comparisons();
}

vsph_sph_box_2d::vsph_sph_box_2d(vsph_sph_point_2d const& pa,
                                 vsph_sph_point_2d const& pb,
                                 vsph_sph_point_2d const& pc)
{
  min_th_ = max_th_= 1000.0; //empty
  a_phi_ = b_phi_ = c_phi_ = 1000.0;
  //assume all the points are in the same units
  in_radians_ = pa.in_radians_;
  double a_th = pa.theta_, b_th = pb.theta_, c_th = pc.theta_;
  min_th_ = a_th;
  max_th_ = b_th;
  if (b_th < a_th) {
    min_th_ = b_th;
    max_th_ = a_th;
  }
  if (c_th<min_th_) min_th_ = c_th;
  if (c_th>max_th_) max_th_ = c_th;
  a_phi_ = pa.phi_;   b_phi_ = pb.phi_; c_phi_ = pc.phi_;
  this->b_ccw_a();
  this->set_comparisons();
  this->phi_bounds(min_phi_, max_phi_);
}

// want the phi value that is clockwise of phi_c in the box interval
double vsph_sph_box_2d::min_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return min_phi_;
  else if (in_radians&&!in_radians_)
    return min_phi_/vnl_math::deg_per_rad;
  else
    return min_phi_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::min_theta(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return min_th_;
  else if (in_radians&&!in_radians_)
    return min_th_/vnl_math::deg_per_rad;
  else
    return min_th_*vnl_math::deg_per_rad;
}

// want the phi value that is counter clockwise of phi_c in the box interval
double vsph_sph_box_2d::max_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return max_phi_;
  else if (in_radians&&!in_radians_)
    return max_phi_/vnl_math::deg_per_rad;
  else
    return max_phi_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::max_theta(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return max_th_;
  else if (in_radians&&!in_radians_)
    return max_th_/vnl_math::deg_per_rad;
  else
    return max_th_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::a_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return a_phi_;
  else if (in_radians&&!in_radians_)
    return a_phi_/vnl_math::deg_per_rad;
  else
    return a_phi_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::b_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return b_phi_;
  else if (in_radians&&!in_radians_)
    return b_phi_/vnl_math::deg_per_rad;
  else
    return b_phi_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::c_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return c_phi_;
  else if (in_radians&&!in_radians_)
    return c_phi_/vnl_math::deg_per_rad;
  else
    return c_phi_*vnl_math::deg_per_rad;
}

vsph_sph_point_2d vsph_sph_box_2d::min_point(bool in_radians) const
{
  double theta_min = min_th_;
  if (in_radians&&!in_radians_)
    theta_min /= vnl_math::deg_per_rad;
  if (!in_radians&&in_radians_)
    theta_min *= vnl_math::deg_per_rad;
  double ph_min = this->min_phi(in_radians);
  return vsph_sph_point_2d(theta_min, ph_min, in_radians);
}


vsph_sph_point_2d vsph_sph_box_2d::max_point(bool in_radians) const
{
  double theta_max = max_th_;
  if (in_radians&&!in_radians_)
    theta_max /= vnl_math::deg_per_rad;
  if (!in_radians&&in_radians_)
    theta_max *= vnl_math::deg_per_rad;
  double ph_max = this->max_phi(in_radians);
  return vsph_sph_point_2d(theta_max, ph_max, in_radians);
}

void vsph_sph_box_2d::set(double min_theta, double max_theta,
                          double a_phi, double b_phi,
                          double c_phi, bool in_radians)
{
  min_th_ = min_theta; max_th_ = max_theta;
  a_phi_ = a_phi;   b_phi_ = b_phi;   c_phi_ = c_phi;
  in_radians_ = in_radians;
  this->b_ccw_a();
  this->set_comparisons();
  this->phi_bounds(min_phi_, max_phi_);
}

vsph_sph_box_2d& vsph_sph_box_2d::operator= (const vsph_sph_box_2d & rhs)
{
  if (this != &rhs) {
    bool in_radians = rhs.in_radians();
    this->set(rhs.min_theta(in_radians), rhs.max_theta(in_radians),
              rhs.a_phi(in_radians), rhs.b_phi(in_radians),
              rhs.c_phi(in_radians), in_radians);
  }
  return *this;
}

bool vsph_sph_box_2d::is_empty() const
{
  return min_th_ == 1000.0;
}

bool vsph_sph_box_2d::defined() const
{
  return c_phi_ != 1000.0;
}

void  vsph_sph_box_2d::update_theta(double th)
{
  if (th< min_th_)  min_th_ = th;
  if (th> max_th_)   max_th_ = th;
}

bool vsph_sph_box_2d::extend_interval(double ph)
{
  if (!defined()) return false;//can't extend

  double dpa = vsph_utils::azimuth_diff(a_phi_, ph,  in_radians_);
  double dpb = vsph_utils::azimuth_diff(b_phi_, ph,  in_radians_);
  dpa = std::fabs(dpa); dpb = std::fabs(dpb);
  if (dpa <= dpb)
    a_phi_ = ph;
  else
    b_phi_ = ph;

  return true;
}

void vsph_sph_box_2d::add( double theta, double phi, bool in_radians)
{
  double ph = phi, th = theta;
  // convert input angles to box angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }   // the first point is being added
  if (this->is_empty()) {
    min_th_ = th;
    a_phi_ = phi;
    return;
  }
  if ( max_th_ == 1000.0) {//the 2nd point is being added
    if (th<min_th_) {
      max_th_ = min_th_;
      min_th_ = th;
    }
    else {
      max_th_ = th;
    }
    // case equal
    if (vsph_utils::a_eq_b(ph, a_phi_)) {
      b_phi_ = ph;
      return;
    }
    // case unequal
    if (vsph_utils::a_lt_b(ph, a_phi_)) {
      b_phi_ = a_phi_;
      a_phi_ = ph;
      return;
    }
    b_phi_ = ph;
    return;
  }//end 2nd point added
  if (!defined()) { // point c potentially added
    this->update_theta(th);
    // if point is equal to bounds then interval membership still undefined
    if (vsph_utils::a_eq_b(ph, a_phi_)||vsph_utils::a_eq_b(ph, b_phi_))
      return;
    // ok to update with point c
    c_phi_ = ph;
    // all three points are defined so can set up the compact interval
    // find the interpoint distances
    double dab = std::fabs(vsph_utils::azimuth_diff(a_phi_, b_phi_, in_radians_));
    double dac = std::fabs(vsph_utils::azimuth_diff(a_phi_, c_phi_, in_radians_));
    double dbc = std::fabs(vsph_utils::azimuth_diff(b_phi_, c_phi_, in_radians_));
    // assign the points so that a < c < b in the smaller interval
    double ang_a = a_phi_, ang_b = b_phi_, ang_c = c_phi_;
    if (dab>dac && dab>dbc) {
      if (vsph_utils::a_lt_b(ang_a, ang_b, in_radians_)) {
        a_phi_ = ang_a; b_phi_ = ang_b; c_phi_ = ang_c;
      }
      else {
        b_phi_ = ang_a; a_phi_ = ang_b; c_phi_ = ang_c;
      }
    }
    if (dac>dbc && dac>dab) {
      if (vsph_utils::a_lt_b(ang_a, ang_c, in_radians_)) {
        a_phi_ = ang_a; b_phi_ = ang_c; c_phi_ = ang_b;
      }
      else {
        b_phi_ = ang_a; a_phi_ = ang_c; c_phi_ = ang_b;
      }
    }
    if (dbc>dac && dbc>dab) {
      if (vsph_utils::a_lt_b(ang_b, ang_c, in_radians_)) {
        a_phi_ = ang_b; b_phi_ = ang_c; c_phi_ = ang_a;
      }
      else {
        b_phi_ = ang_b; a_phi_ = ang_c; c_phi_ = ang_a;
      }
    }
    this->b_ccw_a();
    this->set_comparisons();
    this->phi_bounds(min_phi_, max_phi_);
    return;
  }
  // the interval is established so carry out normal updates
  this->update_theta(th);
  if (this->in_interval(ph, in_radians_)) return;
  bool success = this->extend_interval(ph);
  assert(success);
  this->b_ccw_a();
  this->set_comparisons();
  this->phi_bounds(min_phi_, max_phi_);
}

bool vsph_sph_box_2d::contains(vsph_sph_point_2d const& p) const
{
  return contains(p.theta_, p.phi_, p.in_radians_);
}

bool vsph_sph_box_2d::in_interval(double phi, bool in_radians) const
{
  if (!defined()) return false;//inside an interval is undefined
  double ph = phi;
  // convert input angles to box angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad;
  }
  // small interval contains 180
  if (a_ge_zero_&&b_le_zero_) {
    // is the interval the small interval?
    if (( c_gt_a_&& c_le_pi_) || (c_ge_mpi_ && c_lt_b_)) {
      //yes
      bool in = (ph>= a_phi_ && ph<=pye()) ||
                (ph >= -pye() && ph <= b_phi_);
      return in;
    }
    // the interval is the long interval
      bool in = (ph<= a_phi_ && ph>=0.0) ||
                (ph <= 0 && ph >= b_phi_);
      return in;
  }
  // small interval doesn't contain 180
  if (c_gt_a_ && c_lt_b_)  // small interval
    return ph >= a_phi_ && ph <= b_phi_;
  else //long interval
    return (ph<=a_phi_ && ph>=-pye() ) || (ph<=pye()&& ph>=b_phi_);
}

#if 0
bool vsph_sph_box_2d::in_interval(double phi, bool in_radians) const
{
  if (!defined()) return false;//inside an interval is undefined
  double ph = phi;
  // convert input angles to box angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad;
  }
  // this difference is always less that 180.0
  double dif = vsph_utils::azimuth_diff(a_phi_, b_phi_, in_radians_);
  double start_ang = a_phi_, end_ang = b_phi_;
  if (dif<0) {
    start_ang = b_phi_;
    end_ang = a_phi_;
  }
  // small interval contains 180
  if (start_ang >0 && end_ang<0) {
    // is the interval the short interval?
    if ((c_phi_> start_ang && c_phi_<=pye()) ||
        (c_phi_ >= -pye() && c_phi_ < end_ang)) {
      //yes
      bool in = (ph>= start_ang && ph<=pye()) ||
                (ph >= -pye() && ph <= end_ang);
      return in;
    }
    // is the interval the long interval
    if ((c_phi_< start_ang && c_phi_>=0.0) ||
        (c_phi_ < 0 && c_phi_ > end_ang)) {
      //yes
      bool in = (ph<= start_ang && ph>=0.0) ||
                (ph <= 0 && ph >= end_ang);
      return in;
    }
    // can't happen
    assert(false);
  }
  // small interval doesn't contain 180
  if (c_phi_ > start_ang && c_phi_ < end_ang)  // small interval
    return ph >= start_ang && ph <= end_ang;
  else //long interval
    return (ph<=start_ang && ph>=-pye() ) ||
      (ph<=pye()     && ph>=end_ang);
}
#endif

bool vsph_sph_box_2d::operator==(const vsph_sph_box_2d& other) const
{
  if (this == &other) return true; //the same instance
  // can't convert units and still be exactly equal
  if (other.in_radians()!=in_radians_)
    return false;
  double min_th = other.min_theta(), max_th = other.max_theta();
  if ((min_th != min_th_) || (max_th != max_th_))
    return false;
  double a = other.a_phi(), b = other.b_phi(), c = other.c_phi();
  if (!(((a==a_phi_)&&(b==b_phi_))||((a==b_phi_)&&(b==a_phi_))))
    return false;
  if (c_phi_==c) return true;
  // the c location can differ without altering the actual spherical interval.
  if (!this->in_interval(c, in_radians_))
    return false;
  if (!other.in_interval(c_phi_, in_radians_))
    return false;
  return true;
}

bool vsph_sph_box_2d::contains(double const& theta, double const& phi,
                               bool in_radians) const
{
  double ph = phi, th = theta;
  // convert input angles to box angle units
  if (in_radians&&!in_radians_) {
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }
  else if (!in_radians&&in_radians_) {
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  // do the easy case first
  double min_th = min_theta(in_radians_);
  double max_th = max_theta(in_radians_);
  if (th < min_th || th > max_th) return false;
  // treatment of the +-180 cut and large vs. small arc defined by two points
  return in_interval(ph, in_radians_);
}

bool vsph_sph_box_2d::contains(vsph_sph_box_2d const& b) const
{
  double b_min_th = b.min_theta(in_radians_), b_max_th = b.max_theta(in_radians_);
  bool in_theta = b_min_th >= min_th_ && b_max_th<=max_th_;
  if (!in_theta) return false;
  bool in_phi = in_interval(b.min_phi(in_radians_), in_radians_) &&
    in_interval(b.max_phi(in_radians_), in_radians_) &&
    in_interval(b.c_phi(in_radians_), in_radians_);
  return in_phi;
}

double vsph_sph_box_2d::area() const
{
  //everything in radians
  double min_ph = min_phi(true), max_ph = max_phi(true);
  double min_th = min_theta(true), max_th = max_theta(true);
  double a = std::fabs(std::cos(min_th)-std::cos(max_th));
  double ang1, ang2;
  vsph_utils::half_angle(min_ph, max_ph, ang1, ang2, true);
  double ha = ang2;
  if (this->in_interval(ang1, true))
    ha = ang1;
  double dif = std::fabs(vsph_utils::azimuth_diff(min_ph, ha, true));
  dif += std::fabs(vsph_utils::azimuth_diff(ha, max_ph, true));
  return a*dif;
}

vsph_sph_point_2d vsph_sph_box_2d::center(bool in_radians) const
{
  double min_ph = min_phi(true), max_ph = max_phi(true);
  double min_th = min_theta(true), max_th = max_theta(true);
  double half_th = (min_th + max_th)/2.0;
  double ang1, ang2;
  vsph_utils::half_angle(min_ph, max_ph, ang1, ang2, true);
  double ha = ang2;
  if (this->in_interval(ang1, true))
    ha = ang1;

  if (!in_radians) {
    half_th *= vnl_math::deg_per_rad;
    ha *= vnl_math::deg_per_rad;
  }

  return vsph_sph_point_2d(half_th, ha, in_radians);
}

vsph_sph_box_2d vsph_sph_box_2d::transform(double t_theta,
                                           double t_phi, double scale,
                                           bool in_radians) const
{
  //work in units of *this box
  double d_th = t_theta, d_ph = t_phi;
  if (this->in_radians_ && !in_radians) {
    d_th /= vnl_math::deg_per_rad;
    d_ph /= vnl_math::deg_per_rad;
  }
  if (!this->in_radians_ && in_radians) {
    d_th *= vnl_math::deg_per_rad;
    d_ph *= vnl_math::deg_per_rad;
  }
  //first scale *this box and then rotate
  //scaling should occur around the box center
  //angular spread of both theta and phi is multiplied by scale
  vsph_sph_point_2d center = this->center(in_radians_);
  double min_th = min_theta(in_radians_), max_th = max_theta(in_radians_);
  double cth = center.theta_, scth = (cth-min_th)*scale;
  min_th = cth-scth, max_th = cth + scth;
  // now rotate theta
  min_th += d_th, max_th += d_th;
  // don't allow transformations on theta that move outside range
  if (min_th<0.0) min_th = 0.0;
  if (max_th>pye())max_th = pye();
  // for phi it is possible to scale beyond 360 degrees. Don't allow this.
  // stop at the full circle and put phi_c at half angle opposite the
  // circle cut.
  double c_ph = center.phi_;
  double ph_start=min_phi_, ph_end = max_phi_;
  double difs = std::fabs(vsph_utils::azimuth_diff(ph_start, c_ph, in_radians_));
  double dife = std::fabs(vsph_utils::azimuth_diff(c_ph, ph_end, in_radians_));
  double s = scale;
  if (s*(difs+dife) > 2.0*pye()) {
    ph_end = ph_start;
  }
  else {
    //extend the start and end positions
    difs *=s;
    ph_start = c_ph -difs + d_ph;
    ph_end = c_ph + difs + d_ph;
    c_ph += d_ph;
    ph_start = reduce_phi(ph_start);
    ph_end = reduce_phi(ph_end);
    c_ph = reduce_phi(c_ph);
  }
  vsph_sph_box_2d rbox;
  rbox.set(min_th, max_th, ph_start, ph_end, c_ph, in_radians_);
  return rbox;
}


vsph_sph_box_2d vsph_sph_box_2d::transform(double t_theta, double t_phi,
                                           double scale,  double theta_c,
                                           double phi_c,  bool in_radians) const
{
  //work in units of *this box
  double d_th = t_theta, d_ph = t_phi;
  if (this->in_radians_ && !in_radians) {
    d_th /= vnl_math::deg_per_rad;
    d_ph /= vnl_math::deg_per_rad;
    phi_c/=vnl_math::deg_per_rad;
    theta_c/=vnl_math::deg_per_rad;
  }
  if (!this->in_radians_ && in_radians) {
    d_th *= vnl_math::deg_per_rad;
    d_ph *= vnl_math::deg_per_rad;
    phi_c   *=vnl_math::deg_per_rad;
    theta_c *=vnl_math::deg_per_rad;
  }
  //first scale *this box and then rotate
  //scaling is about the point theta_c and phi_c
  //angular spread of both theta and phi is multiplied by scale
  vsph_sph_point_2d center = this->center(in_radians_);
  double min_th = min_theta(in_radians_), max_th = max_theta(in_radians_);
  min_th = (min_th-theta_c)*scale + theta_c +d_th;
  max_th = (max_th-theta_c)*scale + theta_c +d_th;


  // don't allow transformations on theta that move outside range
  if (min_th<0.0) min_th = 0.0;
  if (max_th>pye())max_th = pye();


  // for phi it is possible to scale beyond 360 degrees. Don't allow this.
  // stop at the full circle and put phi_c at half angle opposite the
  // circle cut.
  double c_ph = center.phi_;
  double ph_start=min_phi_, ph_end=max_phi_;
  double difs = (vsph_utils::azimuth_diff(phi_c, ph_start, in_radians_));
  double dife = (vsph_utils::azimuth_diff(phi_c, ph_end, in_radians_));
  double difc = (vsph_utils::azimuth_diff(phi_c, c_ph, in_radians_));

  double s = scale;

  if (s*(difs+dife) > 2.0*pye()) {
    ph_end = ph_start;
  }
  else {
    //extend the start and end positions
    difs *=s; dife*=s;difc*=s;
    ph_start = phi_c + difs + d_ph;
    ph_end =   phi_c + dife + d_ph;
    c_ph = phi_c + difc + d_ph;
    ph_start = reduce_phi(ph_start);
    ph_end = reduce_phi(ph_end);
    c_ph = reduce_phi(c_ph);
  }
  vsph_sph_box_2d rbox;
  rbox.set(min_th, max_th, ph_start, ph_end, c_ph, in_radians_);
  return rbox;
}

void vsph_sph_box_2d::planar_quads(std::vector<vgl_vector_3d<double> >& verts,
                                   std::vector<std::vector<int> >& quads,
                                   double tol) const{
  assert(tol >0.0 && tol < 1.0);
  verts.clear(); quads.clear();
  double temp = 1.0-tol;
  double max_ang = 2.0*std::acos(temp);
  if (!in_radians_)
    max_ang *= vnl_math::deg_per_rad;
  double min_th = min_theta(in_radians_), max_th = max_theta(in_radians_);
  double theta_range =  std::fabs(max_th - min_th);
  double ha1, ha2;
  double min_ph = min_phi(in_radians_), max_ph = max_phi(in_radians_);
  vsph_utils::half_angle(min_ph,max_ph, ha1, ha2, in_radians_);
  double ha = ha2;
  if (this->in_interval(ha1, in_radians_))
    ha = ha1;
  double ph_start, ph_end;
  this->phi_bounds(ph_start, ph_end);
  double phi_range = std::fabs(vsph_utils::azimuth_diff(ph_start, ha, in_radians_));
  phi_range += std::fabs(vsph_utils::azimuth_diff(ha, ph_end, in_radians_));
  double phi_last = ph_start + phi_range;
  double n_thd = std::ceil(theta_range/max_ang);
  double n_phd = std::ceil(phi_range/max_ang);
  double th_inc = theta_range/n_thd, ph_inc = phi_range/n_phd;
  int n_th = static_cast<int>(n_thd), n_ph = static_cast<int>(n_phd);
  if (n_th == 0) n_th = 1;
  if (n_ph == 0) n_ph = 1;
  if (n_th ==1 && n_ph ==1) {//a single quad
    vsph_sph_point_2d ul(min_th, ph_start, in_radians_);
    vsph_sph_point_2d ur(min_th, ph_end, in_radians_);
    vsph_sph_point_2d lr(max_th, ph_end, in_radians_);
    vsph_sph_point_2d ll(max_th, ph_start, in_radians_);
    vgl_vector_3d<double> vul = vsph_unit_sphere::cart_coord(ul);
    vgl_vector_3d<double> vur = vsph_unit_sphere::cart_coord(ur);
    vgl_vector_3d<double> vlr = vsph_unit_sphere::cart_coord(lr);
    vgl_vector_3d<double> vll = vsph_unit_sphere::cart_coord(ll);
    verts.resize(4);
    int ill = 0, ilr = 1, iur =2 , iul = 3;//ccw order
    verts[iul]=vul; verts[iur]=vur; verts[ilr]=vlr; verts[ill]=vll;
    std::vector<int> quad(4);
    quad[ill]=ill; quad[ilr]=ilr; quad[iur]=iur; quad[iul]=iul;
    quads.push_back(quad);
    return;
  }
  double th_end = max_th - DIST_TOL*th_inc;//take care of roundoff error
  double ph_fin = phi_last -DIST_TOL*ph_inc;
  double th_prev = min_th;
  while (th_prev < th_end)
  {
    double ph_prev = ph_start;
    double th_next = th_prev + th_inc;

    vsph_sph_point_2d ul(th_prev, reduce_phi(ph_prev), in_radians_);
    vgl_vector_3d<double> vul = vsph_unit_sphere::cart_coord(ul);
    int iul = verts.size();
    verts.push_back(vul);

    vsph_sph_point_2d ll(th_next, reduce_phi(ph_prev), in_radians_);
    vgl_vector_3d<double> vll = vsph_unit_sphere::cart_coord(ll);
    int ill = verts.size();
    verts.push_back(vll);

    while (ph_prev<ph_fin) {
      double ph_next = ph_prev + ph_inc;
      double ph_n=reduce_phi(ph_next);
      vsph_sph_point_2d ur(th_prev, ph_n, in_radians_);
      vgl_vector_3d<double> vur = vsph_unit_sphere::cart_coord(ur);
      int iur = verts.size();
      verts.push_back(vur);

      vsph_sph_point_2d lr(th_next, ph_n, in_radians_);
      vgl_vector_3d<double> vlr = vsph_unit_sphere::cart_coord(lr);
      int ilr = verts.size();
      verts.push_back(vlr);

      std::vector<int> quad(4);
      quad[0]=ill; quad[1]=ilr; quad[2]=iur; quad[3]=iul;
      quads.push_back(quad);

      ph_prev = ph_next;
      iul = iur; ill = ilr;
    }
    th_prev = th_next;
  }
}

bool vsph_sph_box_2d::sub_divide(std::vector<vsph_sph_box_2d>& sub_boxes,
                                 double min_ang) const{
  sub_boxes.clear();
  double min_th = this->min_theta(in_radians_);
  double max_th = this->max_theta(in_radians_);
  double c_th = (min_th + max_th)/2.0;
  double dth = std::fabs(max_th-c_th);
  bool div_th = dth>min_ang;
  double ph_start, ph_end;
  this->phi_bounds(ph_start, ph_end);
  double ha1, ha2;
  vsph_utils::half_angle(ph_start,ph_end, ha1, ha2, in_radians_);
  double ha = ha2;
  if (this->in_interval(ha1, in_radians_))
    ha = ha1;
  double dph = std::fabs(vsph_utils::azimuth_diff(ph_start, ha, in_radians_));
  bool div_ph = dph > min_ang;
  if (!div_th&&!div_ph)
    return false; // no division

  if (!div_th&div_ph) {
    vsph_sph_box_2d box_phi_i, box_phi_j;
    vsph_utils::half_angle(ph_start, ha, ha1, ha2, in_radians_);
    double hai = ha2;
    if (this->in_interval(ha1, in_radians_))
      hai = ha1;
    vsph_utils::half_angle(ha, ph_end, ha1, ha2, in_radians_);
    double haj = ha2;
    if (this->in_interval(ha1, in_radians_))
      haj = ha1;
    box_phi_i.set(min_th, max_th, ph_start, ha, hai, in_radians_);
    box_phi_j.set(min_th, max_th, ha, ph_end, haj, in_radians_);
    sub_boxes.resize(2);
    sub_boxes[0]=box_phi_i; sub_boxes[1]=box_phi_j;
    return true;
  }
  if (div_th&!div_ph) {
    vsph_sph_box_2d box_th_i, box_th_j;
    box_th_i.set(min_th, c_th, a_phi_, b_phi_, c_phi_, in_radians_);
    box_th_j.set(c_th, max_th, a_phi_, b_phi_, c_phi_, in_radians_);
    sub_boxes.resize(2);
    sub_boxes[0]=box_th_i; sub_boxes[1]=box_th_j;
    return true;
  }
  //final choice, sub-divide in both dimensions
  vsph_sph_box_2d box_th0_ph0, box_th0_ph1, box_th1_ph0, box_th1_ph1;
  vsph_utils::half_angle(ph_start, ha, ha1, ha2, in_radians_);
  double hai = ha2;
  if (this->in_interval(ha1, in_radians_))
    hai = ha1;
  vsph_utils::half_angle(ha, ph_end, ha1, ha2, in_radians_);
  double haj = ha2;
  if (this->in_interval(ha1, in_radians_))
    haj = ha1;
  box_th0_ph0.set(min_th, c_th, ph_start, ha, hai, in_radians_);
  box_th0_ph1.set(min_th, c_th, ha, ph_end, haj, in_radians_);
  box_th1_ph0.set(c_th, max_th, ph_start, ha, hai, in_radians_);
  box_th1_ph1.set(c_th, max_th, ha, ph_end, haj, in_radians_);
  sub_boxes.resize(4);
  sub_boxes[0]=box_th0_ph0;   sub_boxes[1]=box_th0_ph1;
  sub_boxes[2]=box_th1_ph0;   sub_boxes[3]=box_th1_ph1;
  return true;
}

void vsph_sph_box_2d::print(std::ostream& os, bool in_radians) const
{
  os << " vsph_sph_box_2d:[(" << min_theta(in_radians) << ' '
     << min_phi(in_radians) << ")->(" << max_theta(in_radians)
     << ' ' << max_phi(in_radians) << ")]\n";
}

void vsph_sph_box_2d::display_box(std::ostream& os,
                                  float r, float g, float b,
                                  double tol, double factor) const
{
  std::vector<vgl_vector_3d<double> > verts;
  std::vector<std::vector<int> > quads;
  this->planar_quads(verts, quads, tol);
  os << "Shape {\n"
     <<"  appearance Appearance{\n"
     <<"    material Material {\n"
     <<"     diffuseColor  " << r << ' ' << g << ' ' << b << '\n'
     <<"          }\n"
     <<"    }\n"
     << " geometry IndexedFaceSet\n"
     << "  {\n"
     << "   coord Coordinate{\n"
     << "     point [\n";
  unsigned n = verts.size();
  for (unsigned iv = 0; iv<n; ++iv) {
    vgl_vector_3d<double>& v = verts[iv];
    os << v.x() * factor<< ',' << v.y()* factor << ',' << v.z()* factor;
    if (iv < (n-1)) os << ",\n";
    else os << '\n';
  }
  os << "      ]\n"
     << "    }\n"
     << "  coordIndex [\n";
  unsigned nq = quads.size();
  for (unsigned iq = 0; iq<nq; ++iq) {
    std::vector<int> quad = quads[iq];
    os << quad[0] << ',' << quad[1] << ',' << quad[2] << ','
       << quad[3] << ", -1";
    if (iq < (nq-1)) os << ",\n";
    else os << '\n';
  }
  os << "   ]\n"
     << " }\n"
     << "}\n";
}

void vsph_sph_box_2d::display_boxes(std::string const& path,
                                    std::vector<vsph_sph_box_2d> const& boxes,
                                    std::vector<std::vector<float> > colors,
                                    double tol,double factor ) {
  std::ofstream os(path.c_str());
  if (!os.is_open())
    return;
  bvrml_write::write_vrml_header(os);
  unsigned nb = boxes.size();
  unsigned nc = colors.size();
  assert(nc == nb);
  for (unsigned i = 0; i<nb; ++i) {
    std::vector<float> c = colors[i];
    boxes[i].display_box(os, c[0], c[1],c[2],tol,factor);
  }
  os.close();
}

void vsph_sph_box_2d::b_read(vsl_b_istream& /*is*/)
{
#if 0
  short version;
  vsl_b_read(is, version);
  switch (version) {
  case 1:
    vsl_b_read(is,in_radians_);
    vsl_b_read(is, min_pos_[0]); // theta
    vsl_b_read(is, min_pos_[1]); // phi
    vsl_b_read(is, max_pos_[0]);
    vsl_b_read(is, max_pos_[1]);
  }
#endif
}

void vsph_sph_box_2d::b_write(vsl_b_ostream& /*os*/)
{
#if 0
  vsl_b_write(os, version());
  vsl_b_write(os, in_radians_);
  vsl_b_write(os, min_pos_[0]); // theta
  vsl_b_write(os, min_pos_[1]); // phi
  vsl_b_write(os, max_pos_[0]);
  vsl_b_write(os, max_pos_[1]);
#endif
}

#if 0
bool intersection(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2,
                  std::vector<vsph_sph_box_2d>& boxes)
{
  bool in_radians = b1.in_radians();
  vsph_sph_box_2d rbox(in_radians);
  double theta_min =
    b1.min_theta(in_radians) < b2.min_theta(in_radians) ?
    b2.min_theta(in_radians) : b1.min_theta(in_radians);

  double theta_max =
    b1.max_theta(in_radians) < b2.max_theta(in_radians) ?
    b1.max_theta(in_radians) : b2.max_theta(in_radians);
  // empty box.
  if (theta_max <= theta_min)
    return false;
  // takes 467 msecs for 10^7 intersections
  double b2_min_ph = b2.min_phi(in_radians), b1_min_ph = b1.min_phi(in_radians);
  double b2_max_ph = b2.max_phi(in_radians), b1_max_ph = b1.max_phi(in_radians);
  bool b2min_in_b1 = b1.in_interval(b2_min_ph, in_radians);
  bool b1min_in_b2 = b2.in_interval(b1_min_ph, in_radians);
  bool b2max_in_b1 = b1.in_interval(b2_max_ph, in_radians);
  bool b1max_in_b2 = b2.in_interval(b1_max_ph, in_radians);

  // no overlap return an empty box
  if (!b2min_in_b1&&!b1min_in_b2&&!b2max_in_b1&&!b1max_in_b2)
    return false;
  double ha1, ha2;

  if (!b1min_in_b2 && !b1max_in_b2 && b2min_in_b1 && b2max_in_b1) {
    vsph_utils::half_angle(b2.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1, in_radians)&& b2.in_interval(ha1, in_radians))
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    boxes.resize(1);
    boxes[0]=rbox;
    return true;
  }

  if (b1min_in_b2 && !b1max_in_b2 && !b2min_in_b1 && b2max_in_b1) {
    vsph_utils::half_angle(b1.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b1.in_interval(ha1, in_radians)&& b2.in_interval(ha1, in_radians))
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    boxes.resize(1);
    boxes[0]=rbox;
    return true;
  }

  if (!b1min_in_b2 && b1max_in_b2 && b2min_in_b1 && !b2max_in_b1) {
    vsph_utils::half_angle(b2.min_phi(in_radians), b1.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b1.in_interval(ha1,in_radians)&& b2.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b1.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b1.max_phi(in_radians),ha2,in_radians);
    boxes.resize(1);
    boxes[0]=rbox;
    return true;
  }

  if (b1min_in_b2 && b1max_in_b2&&!b2min_in_b1&&!b2max_in_b1) {
    vsph_utils::half_angle(b1.min_phi(in_radians), b1.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b1.in_interval(ha1,in_radians)&& b2.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b1.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b1.max_phi(in_radians),ha2,in_radians);
    boxes.resize(1);
    boxes[0]=rbox;
    return true;
  }

  if (!b1min_in_b2 && !b1max_in_b2 && b2min_in_b1 && b2max_in_b1) {
    vsph_utils::half_angle(b2.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    boxes.resize(1);
    boxes[0]=rbox;
    return true;
  }
  // This condition produces two boxes (b1min=>b2max  b2min=>b1max)
  if (b1min_in_b2 && b1max_in_b2 && b2min_in_b1 && b2max_in_b1) {
    vsph_sph_box_2d rbox2(in_radians);
    boxes.resize(2);
    vsph_utils::half_angle(b1.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    boxes[0]=rbox;
    vsph_utils::half_angle(b2.min_phi(in_radians), b1.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      rbox2.set(theta_min, theta_max, b2.min_phi(in_radians),
                b1.max_phi(in_radians),ha1,in_radians);
    else
      rbox2.set(theta_min, theta_max, b2.min_phi(in_radians),
                b1.max_phi(in_radians),ha2,in_radians);
    boxes[1]=rbox2;
    return true;
  }
  std::cout << "IMPOSSIBLE INTERSECTION CONDITION NOT HANDLED!!\n";
  assert(false); //shouldn't happen
  return false;
}
#endif

bool intersection(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2,
                  std::vector<vsph_sph_box_2d>& boxes)
{
  if (b1 == b2){
    boxes.resize(1);
    boxes[0]=b1;
    return true;
  }
  bool in_radians = b1.in_radians();
  double theta_min =
    b1.min_theta(in_radians) < b2.min_theta(in_radians) ?
    b2.min_theta(in_radians) : b1.min_theta(in_radians);

  double theta_max =
    b1.max_theta(in_radians) < b2.max_theta(in_radians) ?
    b1.max_theta(in_radians) : b2.max_theta(in_radians);
  // empty box.
  if (theta_max <= theta_min)
    return false;
  double b2_min_ph = b2.min_phi(in_radians), b1_min_ph = b1.min_phi(in_radians);
  double b2_max_ph = b2.max_phi(in_radians), b1_max_ph = b1.max_phi(in_radians);

  bool b2min_in_b1 = b1.in_interval(b2_min_ph, in_radians);
  bool b1min_in_b2 = b2.in_interval(b1_min_ph, in_radians);
  bool b2max_in_b1 = b1.in_interval(b2_max_ph, in_radians);
  bool b1max_in_b2 = b2.in_interval(b1_max_ph, in_radians);

  unsigned short flags = 0;
  if (b2max_in_b1) flags = flags | 1;
  if (b2min_in_b1) flags = flags | 2;
  if (b1max_in_b2) flags = flags | 4;
  if (b1min_in_b2) flags = flags | 8;
  double ha1, ha2;
  switch (flags)
  {
    // no intersection
   case 0:{
    return false;
   }
    // b2 contained in b1
   case 3:{
    boxes.resize(1);
    boxes[0].set(theta_min, theta_max, b2_min_ph,
                 b2_max_ph, b2.c_phi(in_radians),in_radians);
    return true;
   }
    //  b2_min => b1_max
   case 6:{
    boxes.resize(1);
    vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
    //either ha1 or  ha2 has to be in both box intervals
    if (b1.in_interval(ha1,in_radians)&& b2.in_interval(ha1,in_radians))
      boxes[0].set(theta_min, theta_max, b2_min_ph, b1_max_ph,ha1,in_radians);
    else
      boxes[0].set(theta_min, theta_max, b2_min_ph, b1_max_ph, ha2,in_radians);
    return true;
   }
    //  b1_min => b2_max
   case 9:{
    boxes.resize(1);
    vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
    //eithr ha1 or ha2 has to be in both intervals
    if (b1.in_interval(ha1, in_radians)&& b2.in_interval(ha1, in_radians))
      boxes[0].set(theta_min, theta_max, b1_min_ph, b2_max_ph, ha1,in_radians);
    else

      boxes[0].set(theta_min, theta_max, b1_min_ph, b2_max_ph, ha2,in_radians);
    return true;
   }
    // b1 contained in b2
  case 12:{
    boxes.resize(1);
    boxes[0].set(theta_min, theta_max, b1_min_ph, b1_max_ph,
                 b1.c_phi(in_radians),in_radians);
    return true;
   }

  // This condition produces two boxes (b1min=>b2max  b2min=>b1max)
   case 15:{
    boxes.resize(2);
    vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      boxes[0].set(theta_min, theta_max, b1_min_ph, b2_max_ph,ha1,in_radians);
    else
      boxes[0].set(theta_min, theta_max, b1_min_ph, b2_max_ph,ha2,in_radians);

    vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      boxes[1].set(theta_min, theta_max, b2_min_ph, b1_max_ph, ha1,in_radians);
    else
      boxes[1].set(theta_min, theta_max, b2_min_ph, b1_max_ph, ha2,in_radians);
    return true;
   }
   default:
    std::cout << "IMPOSSIBLE INTERSECTION CONDITION NOT HANDLED!!\n";
    assert(false); //shouldn't happen
    return false;
  }
}

double intersection_area(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2)
{
  if (b1 == b2)
    return b1.area();
  bool in_radians = true;
  double theta_min =
    b1.min_theta(in_radians) < b2.min_theta(in_radians) ?
    b2.min_theta(in_radians) : b1.min_theta(in_radians);

  double theta_max =
    b1.max_theta(in_radians) < b2.max_theta(in_radians) ?
    b1.max_theta(in_radians) : b2.max_theta(in_radians);
  // empty box.
  if (theta_max <= theta_min)
    return 0.0;
  double a = std::fabs(std::cos(theta_min)-std::cos(theta_max));

  double b2_min_ph = b2.min_phi(in_radians), b1_min_ph = b1.min_phi(in_radians);
  double b2_max_ph = b2.max_phi(in_radians), b1_max_ph = b1.max_phi(in_radians);

  bool b2min_in_b1 = b1.in_interval(b2_min_ph, in_radians);
  bool b1min_in_b2 = b2.in_interval(b1_min_ph, in_radians);
  bool b2max_in_b1 = b1.in_interval(b2_max_ph, in_radians);
  bool b1max_in_b2 = b2.in_interval(b1_max_ph, in_radians);

  unsigned short flags = 0;
  if (b2max_in_b1) flags = flags | 1;
  if (b2min_in_b1) flags = flags | 2;
  if (b1max_in_b2) flags = flags | 4;
  if (b1min_in_b2) flags = flags | 8;
  double ha1, ha2, dph =0.0;
  switch (flags)
  {
    // no intersection
    case 0:{
      return 0.0;
    }
      // b2 contained in b1
    case 3:{
      vsph_utils::half_angle(b2_min_ph, b2_max_ph, ha1, ha2, in_radians);
      //either ha1 or  ha2 has to be in the box interval
      if (b2.in_interval(ha1,in_radians))
        dph = vsph_utils::arc_len(b2_min_ph, b2_max_ph,ha1);
      else
        dph =  vsph_utils::arc_len(b2_min_ph, b2_max_ph, ha2);
      return dph*a;
    }
      //  b2_min => b1_max
    case 6:{
      vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
      //either ha1 or  ha2 has to be in both box intervals
      if (b1.in_interval(ha1,in_radians)&& b2.in_interval(ha1,in_radians))
        dph = vsph_utils::arc_len(b2_min_ph, b1_max_ph,ha1);
      else
        dph =  vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha2);
      return dph*a;
    }
      //  b1_min => b2_max
    case 9:{
      vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
      //eithr ha1 or ha2 has to be in both intervals
      if (b1.in_interval(ha1, in_radians)&& b2.in_interval(ha1, in_radians))
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph, ha1);
      else
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph, ha2);
      return dph*a;
    }
      // b1 contained in b2
    case 12:{
      vsph_utils::half_angle(b1_min_ph, b1_max_ph, ha1, ha2, in_radians);
      //either ha1 or  ha2 has to be in the box interval
      if (b1.in_interval(ha1,in_radians))
        dph = vsph_utils::arc_len(b1_min_ph, b1_max_ph,ha1);
      else
        dph =  vsph_utils::arc_len(b1_min_ph, b1_max_ph, ha2);
      return dph*a;
    }

    // This condition produces two boxes (b1min=>b2max  b2min=>b1max)
    case 15:{
      double dph2 =0.0;
      vsph_utils::half_angle(b1_min_ph, b2_max_ph, ha1, ha2, in_radians);
      if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph,ha1);
      else
        dph = vsph_utils::arc_len(b1_min_ph, b2_max_ph,ha2);

      vsph_utils::half_angle(b2_min_ph, b1_max_ph, ha1, ha2, in_radians);
      if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
        dph2 = vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha1);
      else
        dph2 = vsph_utils::arc_len(b2_min_ph, b1_max_ph, ha2);
      return a*(dph+dph2);
    }
    default:
      std::cout << "IMPOSSIBLE INTERSECTION CONDITION NOT HANDLED!!\n";
      assert(false); //shouldn't happen
      return 0.0;
  }
}

std::ostream& operator<<(std::ostream& os, vsph_sph_box_2d const& p)
{
  p.print(os);
  return os;
}
