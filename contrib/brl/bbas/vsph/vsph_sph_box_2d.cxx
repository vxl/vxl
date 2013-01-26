// This is bbas/vsph/vsph_sph_box_2d.cxx
#include "vsph_sph_box_2d.h"
#include "vsph_utils.h"
#include <vgl/vgl_point_2d.h>

vsph_sph_box_2d::vsph_sph_box_2d(): in_radians_(true){
    min_pos_[0]=min_pos_[1]= 1.0;
    max_pos_[0]=max_pos_[1]= 0.0; // empty box
}
vsph_sph_box_2d::vsph_sph_box_2d(bool in_radians): in_radians_(in_radians){
    min_pos_[0]=min_pos_[1]= 1.0;
    max_pos_[0]=max_pos_[1]= 0.0; // empty box
}


double vsph_sph_box_2d::min_phi(bool in_radians) const{
  if((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return min_pos_[1]; 
  if(in_radians&&!in_radians_)
    return (min_pos_[1]/vnl_math::deg_per_rad);
 // if(!in_radians&&in_radians_)
    return (min_pos_[1]*vnl_math::deg_per_rad);
}
double vsph_sph_box_2d::min_theta(bool in_radians) const{
  if((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return min_pos_[0]; 
  if(in_radians&&!in_radians_)
    return (min_pos_[0]/vnl_math::deg_per_rad);
 // if(!in_radians&&in_radians_)
    return (min_pos_[0]*vnl_math::deg_per_rad);
}
double vsph_sph_box_2d::max_phi(bool in_radians) const{
  if((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return max_pos_[1]; 
  if(in_radians&&!in_radians_)
    return (max_pos_[1]/vnl_math::deg_per_rad);
 // if(!in_radians&&in_radians_)
    return (max_pos_[1]*vnl_math::deg_per_rad);
}

double vsph_sph_box_2d::max_theta(bool in_radians) const{
  if((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return max_pos_[0];
  if(in_radians&&!in_radians_)
    return (max_pos_[0]/vnl_math::deg_per_rad);
 // if(!in_radians&&in_radians_)
    return (max_pos_[0]*vnl_math::deg_per_rad);
}
vsph_sph_point_2d vsph_sph_box_2d::min_point(bool in_radians) const{
  double phi_min = min_pos_[1];
  if(!in_radians) phi_min*=vnl_math::deg_per_rad;
  double theta_min = min_pos_[0];
  if(!in_radians) theta_min*=vnl_math::deg_per_rad;
  return vsph_sph_point_2d(phi_min, theta_min);
}
vsph_sph_point_2d vsph_sph_box_2d::max_point(bool in_radians) const{
  double phi_max = max_pos_[1];
  if(!in_radians) phi_max*=vnl_math::deg_per_rad;
  double theta_max = max_pos_[0];
 // if(!in_radians) theta_max*=vnl_math::deg_per_rad;
  return vsph_sph_point_2d(phi_max, theta_max);
}

bool vsph_sph_box_2d::is_empty() const{
  return (min_pos_[0]==1.0&&min_pos_[1]==1.0&&
	  max_pos_[1]==0.0&&max_pos_[1]==0.0);
}

void vsph_sph_box_2d::add( double theta, double phi, bool in_radians){
  double ph = phi, th = theta;
  // convert input angles to box angle units
  if(in_radians&&!in_radians_){
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }else if(!in_radians&&in_radians_){
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  if(this->is_empty()){
    min_pos_[0]= max_pos_[0] = th;
    min_pos_[1]= max_pos_[1] = ph;
  }else{
    // do the easy case first
    double min_th = min_theta(in_radians_);
    double max_th = max_theta(in_radians_);
    if(th < min_th) min_pos_[0] = th;
    if(th > max_th) max_pos_[0] = th;
    //do the angle with the +-180 cut
    double min_ph = min_phi(in_radians_);
    double max_ph = max_phi(in_radians_);
    if(vsph_utils::a_eq_b(ph, min_ph) ||vsph_utils::a_eq_b(ph, max_ph))
      return;
    if(vsph_utils::a_lt_b(ph, min_ph))  min_pos_[1] = ph; 
    if(!vsph_utils::a_lt_b(ph, max_ph)) max_pos_[1] = ph; 
  }
}

bool vsph_sph_box_2d::contains(vsph_sph_point_2d const& p) const{
  return contains(p.phi_, p.theta_, p.in_radians_);
}

bool vsph_sph_box_2d::contains(double const& theta, double const& phi,
			       bool in_radians) const{
  double ph = phi, th = theta; 
  // convert input angles to box angle units
  if(in_radians&&!in_radians_){
    ph*=vnl_math::deg_per_rad; th*=vnl_math::deg_per_rad;
  }else if(!in_radians&&in_radians_){
    ph/=vnl_math::deg_per_rad; th/=vnl_math::deg_per_rad;
  }
  // do the easy case first
  double min_th = min_theta(in_radians_);
  double max_th = max_theta(in_radians_);
  bool temp = th >= min_th && th <= max_th;
  if(!temp) return false;
  // treatment of the +-180 cut
  double min_ph = min_phi(in_radians_);
  double max_ph = max_phi(in_radians_);
  if(vsph_utils::a_eq_b(ph, min_ph)) return true;
  if(vsph_utils::a_eq_b(ph, max_ph)) return true;
  if(vsph_utils::a_lt_b(min_ph, ph)) return true;
  if(vsph_utils::a_lt_b(ph, max_ph)) return true;
  return false;
}
bool vsph_sph_box_2d::contains(vsph_sph_box_2d const& b) const{
  // comparisons are in units of *this box
   return contains(b.min_phi(), b.min_theta(), b.in_radians()) &&
    contains(b.max_phi(), b.max_theta(), b.in_radians());
}

double vsph_sph_box_2d::area() const{
  double min_ph = min_phi(), max_ph = max_phi();
  double min_th = min_theta(), max_th = max_theta();
  double a = vcl_fabs(vcl_cos(min_th)-vcl_cos(max_th));
  a *= vcl_fabs(vsph_utils::azimuth_diff(min_ph, max_ph));
  return a;
}
void vsph_sph_box_2d::print(vcl_ostream& os, bool in_radians) const
{
  os << " vsph_sph_box_2d:[(" << min_theta(in_radians) << ' ' 
     << min_phi(in_radians) << ")->(" << max_theta(in_radians) 
     << ' ' << max_phi(in_radians) << ")]\n";
}

void vsph_sph_box_2d::b_read(vsl_b_istream& is)
{
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
}

void vsph_sph_box_2d::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, version());
  vsl_b_write(os, in_radians_);
  vsl_b_write(os, min_pos_[0]); // theta
  vsl_b_write(os, min_pos_[1]); // phi
  vsl_b_write(os, max_pos_[0]); 
  vsl_b_write(os, max_pos_[1]);
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_box_2d const& p)
{
  p.print(os);
  return os;
}

