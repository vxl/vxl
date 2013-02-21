// This is bbas/vsph/vsph_sph_box_2d.cxx
#include "vsph_sph_box_2d.h"
#include "vsph_utils.h"
#include "vsph_defs.h"
#include "vsph_unit_sphere.h"
#include <bvrml/bvrml_write.h>
#include <vcl_cassert.h>
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

void vsph_sph_box_2d::phi_bounds(double& phi_start, double& phi_end) const
{
  double ph_min = min_phi(in_radians_), ph_max = max_phi(in_radians_);
  // does interval span cut?
  if(ph_min>0 && ph_max<0)
    if(in_interval(pye(), in_radians_)){
      phi_start = ph_min;
      phi_end = ph_max;
      return;
    }else{
      phi_start = ph_max;
      phi_end = ph_min;
      return;
    }
  double mid = 0.5*(ph_min+ph_max);
  if (in_interval(mid, in_radians_)) {//small interval
    phi_start = ph_min;
    phi_end = ph_max;
  }
  else {//large interval
    phi_start = ph_max;
    phi_end = ph_min;
  }
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
// want the phi value that is clockwise of phi_c in the box interval
double vsph_sph_box_2d::min_phi(bool in_radians) const
{
  double ph_min = a_phi_;
  if(vsph_utils::azimuth_diff(ph_min, c_phi_,  in_radians_) < 0)
    ph_min = b_phi_;
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return ph_min;
  else if (in_radians&&!in_radians_)
    return ph_min/vnl_math::deg_per_rad;
  else
    return ph_min*vnl_math::deg_per_rad;
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
  double ph_max = b_phi_;
  if(vsph_utils::azimuth_diff(c_phi_, ph_max,  in_radians_) < 0)
    ph_max = a_phi_;
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return ph_max;
  else if (in_radians&&!in_radians_)
    return ph_max/vnl_math::deg_per_rad;
  else
    return ph_max*vnl_math::deg_per_rad;
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
double vsph_sph_box_2d::a_phi(bool in_radians) const{
  if ((in_radians&&in_radians_)||(!in_radians&&!in_radians_))
    return a_phi_;
  else if (in_radians&&!in_radians_)
    return a_phi_/vnl_math::deg_per_rad;
  else
    return a_phi_*vnl_math::deg_per_rad;
}

double vsph_sph_box_2d::b_phi(bool in_radians) const{
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
  double ph_start, ph_end;
  this->phi_bounds(ph_start, ph_end);
  if (!this->in_radians_) {
    ph_start /= vnl_math::deg_per_rad;
    ph_end /= vnl_math::deg_per_rad;
  }
  double dif = vcl_fabs(vsph_utils::azimuth_diff(ph_start, ha, true));
  dif += vcl_fabs(vsph_utils::azimuth_diff(ha, ph_end, true));
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

  if(!in_radians){
    half_th *= vnl_math::deg_per_rad;
    ha *= vnl_math::deg_per_rad;
  }

  return vsph_sph_point_2d(half_th, ha, in_radians);
}

vsph_sph_box_2d vsph_sph_box_2d::transform(double t_theta, 
					   double t_phi, double scale,
					   bool in_radians) const{
  
  //work in units of *this box
  double d_th = t_theta, d_ph = t_phi;
  if(this->in_radians_ && !in_radians){
    d_th /= vnl_math::deg_per_rad;
    d_ph /= vnl_math::deg_per_rad;
  }
  if(!this->in_radians_ && in_radians){
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
  if(min_th<0.0) min_th = 0.0;
  if(max_th>pye())max_th = pye();
  // for phi it is possible to scale beyond 360 degrees. Don't allow this.
  // stop at the full circle and put phi_c at half angle opposite the 
  // circle cut.
  double c_ph = center.phi_;
  double ph_start, ph_end;
  this->phi_bounds(ph_start, ph_end);
  double difs = vcl_fabs(vsph_utils::azimuth_diff(ph_start, c_ph, in_radians_));
  double dife = vcl_fabs(vsph_utils::azimuth_diff(c_ph, ph_end, in_radians_));  
  double s = scale;
  if(s*(difs+dife) > 2.0*pye()){
    ph_end = ph_start;
  }else{
    //extend the start and end positions
    difs *=s; dife*=s;
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

void vsph_sph_box_2d::planar_quads(vcl_vector<vgl_vector_3d<double> >& verts,
                                   vcl_vector<vcl_vector<int> >& quads,
                                   double tol) const{
  assert(tol >0.0 && tol < 1.0);
  verts.clear(); quads.clear();
  double temp = 1.0-tol;
  double max_ang = 2.0*vcl_acos(temp);
  if (!in_radians_)
    max_ang *= vnl_math::deg_per_rad;
  double min_th = min_theta(in_radians_), max_th = max_theta(in_radians_);
  double theta_range =  vcl_fabs(max_th - min_th);
  double ha1, ha2;
  double min_ph = min_phi(in_radians_), max_ph = max_phi(in_radians_);
  vsph_utils::half_angle(min_ph,max_ph, ha1, ha2, in_radians_);
  double ha = ha2;
  if (this->in_interval(ha1, in_radians_))
    ha = ha1;
  double ph_start, ph_end;
  this->phi_bounds(ph_start, ph_end);
  double phi_range = vcl_fabs(vsph_utils::azimuth_diff(ph_start, ha, in_radians_));
  phi_range += vcl_fabs(vsph_utils::azimuth_diff(ha, ph_end, in_radians_));
  double phi_last = ph_start + phi_range;
  double n_thd = vcl_ceil(theta_range/max_ang);
  double n_phd = vcl_ceil(phi_range/max_ang);
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
    vcl_vector<int> quad(4);
    quad[ill]=ill; quad[ilr]=ilr; quad[iur]=iur; quad[iul]=iul;
    quads.push_back(quad);
    return;
  }
  double th_end = max_th - DIST_TOL*th_inc;//take care of roundoff error
  double ph_fin = phi_last -DIST_TOL*ph_inc;
  double th_prev = min_th;
  while (th_prev < th_end) {
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

      vcl_vector<int> quad(4);
      quad[0]=ill; quad[1]=ilr; quad[2]=iur; quad[3]=iul;
      quads.push_back(quad);

      ph_prev = ph_next;
      iul = iur; ill = ilr;
    }
    th_prev = th_next;
  }
}

void vsph_sph_box_2d::print(vcl_ostream& os, bool in_radians) const
{
  os << " vsph_sph_box_2d:[(" << min_theta(in_radians) << ' '
     << min_phi(in_radians) << ")->(" << max_theta(in_radians)
     << ' ' << max_phi(in_radians) << ")]\n";
}

void vsph_sph_box_2d::display_box(vcl_ostream& os,
                                  float r, float g, float b, double tol) const
{
  vcl_vector<vgl_vector_3d<double> > verts;
  vcl_vector<vcl_vector<int> > quads;
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
    os << v.x() << ',' << v.y() << ',' << v.z();
    if (iv < (n-1)) os << ",\n";
    else os << '\n';
  }
  os << "      ]\n"
     << "    }\n"
     << "  coordIndex [\n";
  unsigned nq = quads.size();
  for (unsigned iq = 0; iq<nq; ++iq) {
    vcl_vector<int> quad = quads[iq];
    os << quad[0] << ',' << quad[1] << ',' << quad[2] << ','
       << quad[3] << ", -1";
    if (iq < (nq-1)) os << ",\n";
    else os << '\n';
  }
  os << "   ]\n"
     << " }\n"
     << "}\n";
}

void vsph_sph_box_2d::display_boxes(vcl_string const& path,
				    vcl_vector<vsph_sph_box_2d> const& boxes,
				    vcl_vector<vcl_vector<float> > colors,
				    double tol){
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return;
  bvrml_write::write_vrml_header(os);
  unsigned nb = boxes.size();
  unsigned nc = colors.size();
  assert(nc == nb);
  for (unsigned i = 0; i<nb; ++i) {
    vcl_vector<float> c = colors[i];
    boxes[i].display_box(os, c[0], c[1],c[2],tol);
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

bool intersection(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2,
                  vcl_vector<vsph_sph_box_2d>& boxes)
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
  bool b2min_in_b1 = b1.in_interval(b2.min_phi(in_radians), in_radians);
  bool b1min_in_b2 = b2.in_interval(b1.min_phi(in_radians), in_radians);
  bool b2max_in_b1 = b1.in_interval(b2.max_phi(in_radians), in_radians);
  bool b1max_in_b2 = b2.in_interval(b1.max_phi(in_radians), in_radians);

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
    boxes.push_back(rbox);
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
    boxes.push_back(rbox);
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
    boxes.push_back(rbox);
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
    boxes.push_back(rbox);
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
    boxes.push_back(rbox);
    return true;
  }
  // This condition produces two boxes (b1min=>b2max  b2min=>b1max)
  if (b1min_in_b2 && b1max_in_b2 && b2min_in_b1 && b2max_in_b1) {
    vsph_sph_box_2d rbox2(in_radians);
    vsph_utils::half_angle(b1.min_phi(in_radians), b2.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha1,in_radians);
    else
      rbox.set(theta_min, theta_max, b1.min_phi(in_radians),
               b2.max_phi(in_radians),ha2,in_radians);
    boxes.push_back(rbox);
    vsph_utils::half_angle(b2.min_phi(in_radians), b1.max_phi(in_radians),
                           ha1, ha2, in_radians);
    if (b2.in_interval(ha1,in_radians)&& b1.in_interval(ha1,in_radians))
      rbox2.set(theta_min, theta_max, b2.min_phi(in_radians),
		b1.max_phi(in_radians),ha1,in_radians);
    else
      rbox2.set(theta_min, theta_max, b2.min_phi(in_radians),
		b1.max_phi(in_radians),ha2,in_radians);
    boxes.push_back(rbox2);
    return true;
  }
  vcl_cout << "IMPOSSIBLE INTERSECTION CONDITION NOT HANDLED!!\n";
  assert(false); //shouldn't happen
  return false;
}

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_box_2d const& p)
{
  p.print(os);
  return os;
}

