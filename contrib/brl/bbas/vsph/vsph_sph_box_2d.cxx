// This is bbas/vsph/vsph_sph_box_2d.cxx
#include "vsph_sph_box_2d.h"
#include "vsph_utils.h"
#include <vcl_cassert.h>

double vsph_sph_box_2d::pye() const
{
  if (in_radians_) return vnl_math::pi;
  return 180.0;
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
}

double vsph_sph_box_2d::min_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return a_phi_;
  else if (in_radians&&!in_radians_)
    return a_phi_/vnl_math::deg_per_rad;
  else
    return a_phi_*vnl_math::deg_per_rad;
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

double vsph_sph_box_2d::max_phi(bool in_radians) const
{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return b_phi_;
  else if (in_radians&&!in_radians_)
    return b_phi_/vnl_math::deg_per_rad;
  else
    return b_phi_*vnl_math::deg_per_rad;
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
  double phi_min = a_phi_;
  if (!in_radians) phi_min*=vnl_math::deg_per_rad;
  double theta_min = min_th_;
  if (!in_radians) theta_min*=vnl_math::deg_per_rad;
  return vsph_sph_point_2d(phi_min, theta_min);
}

vsph_sph_point_2d vsph_sph_box_2d::max_point(bool in_radians) const
{
  double phi_max = b_phi_;
  if (!in_radians) phi_max*=vnl_math::deg_per_rad;
  double theta_max = max_th_;
 // if (!in_radians) theta_max*=vnl_math::deg_per_rad;
  return vsph_sph_point_2d(phi_max, theta_max);
}

void vsph_sph_box_2d::set(double min_theta, double max_theta,
                          double a_phi, double b_phi,
                          double c_phi, bool in_radians)
{
  min_th_ = min_theta; max_th_ = max_theta;
  a_phi_ = a_phi;   b_phi_ = b_phi;   c_phi_ = c_phi;
  in_radians_ = in_radians;
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
  dpa = vcl_fabs(dpa); dpb = vcl_fabs(dpb);
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
    double dab = vcl_fabs(vsph_utils::azimuth_diff(a_phi_, b_phi_, in_radians_));
    double dac = vcl_fabs(vsph_utils::azimuth_diff(a_phi_, c_phi_, in_radians_));
    double dbc = vcl_fabs(vsph_utils::azimuth_diff(b_phi_, c_phi_, in_radians_));
    // assign the points so that a < c < b in the smaller interval
    double ang_a = a_phi_, ang_b = b_phi_, ang_c = c_phi_;
    if (dab>dac && dab>dbc) {
      if (vsph_utils::a_lt_b(ang_a, ang_b, in_radians_)) {
        a_phi_ = ang_a; b_phi_ = ang_b; c_phi_ = ang_c;
      }
      else {
        b_phi_ = ang_a; a_phi_ = ang_b; c_phi_ = ang_c;}
      return;
    }
    if (dac>dbc && dac>dab) {
      if (vsph_utils::a_lt_b(ang_a, ang_c, in_radians_)) {
        a_phi_ = ang_a; b_phi_ = ang_c; c_phi_ = ang_b;
      }
      else {
        b_phi_ = ang_a; a_phi_ = ang_c; c_phi_ = ang_b;}
      return;
    }
    if (dbc>dac && dbc>dab) {
      if (vsph_utils::a_lt_b(ang_b, ang_c, in_radians_)) {
        a_phi_ = ang_b; b_phi_ = ang_c; c_phi_ = ang_a;
      }
      else {
        b_phi_ = ang_b; a_phi_ = ang_c; c_phi_ = ang_a;}
      return;
    }
  }
  // the interval is established so carry out normal updates
  this->update_theta(th);
  if (this->in_interval(ph, in_radians_)) return;
  bool success = this->extend_interval(ph);
  assert(success);
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
  // treatment of the +-180 cut
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
  double a = vcl_fabs(vcl_cos(min_th)-vcl_cos(max_th));
  double ang1, ang2;
  vsph_utils::half_angle(min_ph, max_ph, ang1, ang2, true);
  double ha = ang2;
  if (this->in_interval(ang1, true))
    ha = ang1;
  double dif = vcl_fabs(vsph_utils::azimuth_diff(min_ph, ha, true));
  dif += vcl_fabs(vsph_utils::azimuth_diff(ha, max_ph, true));
  return a*dif;
}

void vsph_sph_box_2d::print(vcl_ostream& os, bool in_radians) const
{
  os << " vsph_sph_box_2d:[(" << min_theta(in_radians) << ' '
     << min_phi(in_radians) << ")->(" << max_theta(in_radians)
     << ' ' << max_phi(in_radians) << ")]\n";
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

vsph_sph_box_2d intersection(vsph_sph_box_2d const& b1,
                             vsph_sph_box_2d const& b2)
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
    return rbox;

  bool b2min_in_b1 = b1.in_interval(b2.min_phi(in_radians), in_radians);
  bool b1min_in_b2 = b2.in_interval(b1.min_phi(in_radians), in_radians);
  bool b2max_in_b1 = b1.in_interval(b2.max_phi(in_radians), in_radians);
  bool b1max_in_b2 = b2.in_interval(b1.max_phi(in_radians), in_radians);
  // no overlap return an empty box
  if (!b2min_in_b1&&!b1min_in_b2&&!b2max_in_b1&&!b1max_in_b2)
    return rbox;
  // two possible conditions of overlap
  double ha1, ha2;

  if (b1min_in_b2 && b2max_in_b1) {
    vsph_utils::half_angle(b1.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b1.in_interval(ha1, in_radians)&& b2.in_interval(ha1, in_radians))
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    return rbox;
  }

  if (b2min_in_b1 && b1max_in_b2) {
    vsph_utils::half_angle(b2.min_phi(in_radians), b1.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b1.in_interval(ha1,in_radians)&& b2.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b1.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b2.min_phi(in_radians),
               b1.max_phi(in_radians),ha2,in_radians);
    return rbox;
  }
  assert(false); //shouldn't happen
  return rbox;
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_box_2d const& p)
{
  p.print(os);
  return os;
}

