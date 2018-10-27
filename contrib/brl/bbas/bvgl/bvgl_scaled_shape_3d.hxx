// This is brl/bbas/bvgl/bvgl_scaled_shape_3d.hxx
#ifndef bvgl_scaled_shape_3d_hxx_
#define bvgl_scaled_shape_3d_hxx_
#include <cstdlib>
#include <cmath>
#include <limits>
#include "bvgl_scaled_shape_3d.h"
#include <vgl/vgl_distance.h>
template <class Type>
Type bvgl_scaled_shape_3d<Type>::linear_scale(Type w) const{
  // how does the scale of the base vary with the normal parameter
  // for now assume linear range of 0.1-1.0
  Type s = Type(1)+ w*(scale_at_max_-Type(1));
  if(s > Type(1))
    return Type(1);
  if(s<Type(0.1))
    return Type(0.1);
  return s;
}
template <class Type>
Type bvgl_scaled_shape_3d<Type>::quadratic_scale(Type w) const{
  // how does the scale of the base vary with the normal parameter
  // for now assume quadratic range of 0.1-1.0
  Type s1 = Type(4)*scale_at_midpt_-scale_at_max_-Type(3);
  Type s2 = Type(2)*(Type(1)+scale_at_max_)-Type(4)*scale_at_midpt_;
  Type s = Type(1)+ w*s1 + w*w*s2;
  if(s > Type(1))
    return Type(1);
  if(s<Type(0.1))
    return Type(0.1);
  return s;
}
template <class Type>
void bvgl_scaled_shape_3d<Type>::compute_cross_sections(){
  vgl_vector_3d<Type> n = base_.normal();
  for(Type nd = Type(0); nd<=max_norm_distance_; nd += tolerance_){
    vgl_vector_3d<Type> v = nd*n;
    Type w = nd/max_norm_distance_;
    Type s = Type(1);
    if(stype_ == LINEAR)
      s = this->linear_scale(w);
    else
      s = this->quadratic_scale(w);
    cross_sections_.push_back(base_.scale(s,v));
  }
}
template <class Type>
bool bvgl_scaled_shape_3d<Type>::nearest_cross_section_index(vgl_point_3d<Type> const& p3d, unsigned& index) const{
  // assume for the time being that the base is the maximum cross section
  index = 0;
  unsigned n =static_cast<unsigned>(cross_sections_.size());
  Type dist;
  if(!base_.signed_distance(p3d, dist))
    return false;
  if(dist<-tolerance_)//tolerance below the bottom of the base
    return false;
  if(dist>max_norm_distance_){
    index = n-1;
    return false;
  }
  Type dindex = std::floor(dist/tolerance_);
  index = static_cast<unsigned>(dindex);

  if(index >= n)
    index = n-1;
  return true;
}
template <class Type>
void bvgl_scaled_shape_3d<Type>::nearest_cross_section_index_unbounded(vgl_point_3d<Type> const& p3d, unsigned& index) const{
  unsigned n =static_cast<unsigned>(cross_sections_.size());
  vgl_plane_3d<Type> pl = base_.plane();
  Type alg_dist = pl.a()*p3d.x() + pl.b()*p3d.y() + pl.c()*p3d.z() + pl.d();
  if(alg_dist <= Type(0)){
    index = unsigned(0);
    return;
  }
  Type dindex = std::floor(alg_dist/tolerance_);
  index = static_cast<unsigned>(dindex);
  if(index >= n)
    index = n-1;
}

template <class Type>
vgl_point_3d<Type>  bvgl_scaled_shape_3d<Type>::closest_point(vgl_point_3d<Type> const& p) const{
  unsigned index;
  this->nearest_cross_section_index_unbounded(p, index);
  return cross_sections_[index].closest_point(p);
}

template <class Type>
bool bvgl_scaled_shape_3d<Type>::in(vgl_point_3d<Type> const& p3d) const{
  unsigned indx;
  if(!nearest_cross_section_index(p3d, indx))
    return false;
  return cross_sections_[indx].in(p3d);
}
template <class Type>
Type bvgl_scaled_shape_3d<Type>::distance(vgl_point_3d<Type> const& p) const{
  vgl_point_3d<Type> cp = this->closest_point(p);
  return (p-cp).length();
}

template <class Type>
void bvgl_scaled_shape_3d<Type>::uv_bounds(Type& u_min, Type& u_max, Type& v_min, Type& v_max) const{
  vgl_box_2d<Type> bb_2d = base_.bounding_box_2d();
  u_min = bb_2d.min_x(); u_max = bb_2d.max_x();
  v_min = bb_2d.min_y(); v_max = bb_2d.max_y();
}

template <class Type>
bool bvgl_scaled_shape_3d<Type>::operator () (Type u, Type v, Type w, vgl_point_3d<Type>& p) const{
  p.set(Type(0), Type(0), Type(0));
   unsigned n = static_cast<unsigned>(cross_sections_.size());
  Type dindex = std::floor(w*Type(n-1));
  unsigned index = 0;
  if(dindex>=Type(0))
    index = static_cast<unsigned>(dindex);
  else
    return false;
  if(index>=n)
    return false;

  const bvgl_spline_region_3d<Type>& csect = cross_sections_[index];
  csect.plane_to_world(u,v,p);
  return csect.in(p);
}

template <class Type>
vgl_point_3d<Type> bvgl_scaled_shape_3d<Type>::centroid() const{
  Type cx=Type(0), cy=Type(0), cz=Type(0);
  return vgl_point_3d<Type>(cx, cy, cz);
}

template <class Type>
Type bvgl_scaled_shape_3d<Type>::volume() const{
  unsigned n =static_cast<unsigned>(cross_sections_.size());
  // determine the spacing between the cross sections
  Type dw = max_norm_distance_/(static_cast<Type>(n-1)); // n-1 spaces for n cross sections
  Type vol = Type(0);
  Type am = cross_sections_[0].area();
  for(unsigned i = 1; i<n; ++i){
    Type ai = cross_sections_[i].area();
    vol += (am+ai)*dw/Type(2);
    am = ai;
  }
 return vol;
}
template <class Type>
bvgl_scaled_shape_3d<Type> bvgl_scaled_shape_3d<Type>::anisotropic_scale(Type su, Type sv, Type sw, vgl_vector_3d<Type> const& L1) const{
  unsigned n = static_cast<unsigned>(cross_sections_.size());
  Type dn = max_norm_distance_/static_cast<Type>(n-1);
  Type max_nd = max_norm_distance_*sw;
  vgl_vector_3d<Type> dv = (sw-Type(1))*dn*base_.normal();
  bvgl_scaled_shape_3d<Type> ret = (*this);
  const std::vector<bvgl_spline_region_3d<Type> >& csections = ret.cross_sections();
  std::vector<bvgl_spline_region_3d<Type> > scaled_csects;
  for(unsigned i =0; i<n; ++i){
    Type mul = static_cast<Type>(i);
    scaled_csects.push_back(csections[i].scale(su, sv, mul*dv, L1));
  }
  ret.set_cross_sections(scaled_csects);
  ret.set_max_norm_distance(max_nd);
  return ret;
}
template <class Type>
bvgl_scaled_shape_3d<Type> bvgl_scaled_shape_3d<Type>::deform(Type lambda, Type gamma, vgl_vector_3d<Type> const& L1)const{
  Type su = lambda, sv = std::pow(lambda,-gamma), sw = Type(1)/(su*sv);
  return anisotropic_scale(su, sv, sw, L1);
}
template <class Type>
bvgl_scaled_shape_3d<Type> bvgl_scaled_shape_3d<Type>::scale(Type s){
  Type su = s, sv = s, sw = s;
  return anisotropic_scale(su, sv, sw);
}
template <class Type>
void bvgl_scaled_shape_3d<Type>::apply_parameters_to_cross_sections(){
  Type su=su_, sv = sv_, sw = sw_;
  if((su*sv*sw)==Type(0)){
    su = lambda_;
    sv = std::pow(lambda_,-gamma_);
    sw = Type(1)/(su*sv);
  }
  max_nd_ = max_norm_distance_*sw;
  unsigned n = static_cast<unsigned>(cross_sections_.size());
  Type dn = max_norm_distance_/static_cast<Type>(n-1);
  vgl_vector_3d<Type> dv = (sw-Type(1))*dn*base_.normal();
  for(unsigned i = 0; i<n; ++i){
    Type mul = static_cast<Type>(i);
    cross_sections_[i].set_principal_eigenvector(L1_);
    cross_sections_[i].set_deformation_eigenvalues(su, sv);
    cross_sections_[i].set_offset_vector(mul*dv);
    cross_sections_[i].set_principal_offset(principal_offset_);
  }
}
template <class Type>
bool bvgl_scaled_shape_3d<Type>::inverse_vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& inv) const{
  Type dist;
  if(!cross_sections_[0].signed_distance(p, dist))
    return false;
  unsigned n = static_cast<unsigned>(cross_sections_.size()), index = 0;
  Type csect_separation_dist = max_nd_/static_cast<Type>(n-1);

  if(dist<-tolerance_){//tolerance below the bottom of the base
    return false;
  }
  if(dist>(max_nd_+tolerance_)){//tolerance above the apex
    return false;
  }
  Type dindex = std::floor(dist/csect_separation_dist);
  if(dindex<Type(0))
    index =0;
  else
    index = static_cast<unsigned>(dindex);
  if(index >= n)
    index = n-1;
  return cross_sections_[index].inverse_vector_field(p, inv, csect_separation_dist);
}

template <class Type>
bool bvgl_scaled_shape_3d<Type>::vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& vf) const{
  Type dist;
  if(!cross_sections_[0].signed_distance(p, dist))
    return false;
  unsigned n = static_cast<unsigned>(cross_sections_.size()), index = 0;

  Type csect_separation_dist = tolerance_;//undeformed shape

  if(dist<-tolerance_){//tolerance below the bottom of the base
    return false;
  }
  if(dist>(max_norm_distance_+tolerance_)){//tolerance above the apex
    return false;
  }
  Type dindex = std::ceil(dist/csect_separation_dist);//changed to ceil (2/28)
  if(dindex<Type(0))
    index =0;
  else
    index = static_cast<unsigned>(dindex);
  if(index >= n)
    index = n-1;
  Type dn = (max_nd_-max_norm_distance_)/static_cast<Type>(n-1);
  if(dn<Type(0)) dn = -dn;
  vgl_vector_3d<Type> dv = Type(index)*dn*base_.normal();
  return cross_sections_[index].vector_field(p, vf, dv);
}

template <class Type>
vgl_box_3d<Type> bvgl_scaled_shape_3d<Type>::bounding_box() const{
  vgl_box_3d<Type> bb = base_.bounding_box();
  // assume the scale monotonically decreases so bounding box is defined by the base
  vgl_vector_3d<Type> nv = max_norm_distance_*base_.normal();
  std::vector<vgl_point_3d<Type> > knots = base_.knots();
  for(typename std::vector<vgl_point_3d<Type> >::iterator kit = knots.begin();
      kit != knots.end(); ++kit){
    vgl_point_3d<Type> p = (*kit) + nv;
    bb.add(p);
  }
    return bb;
}

template <class Type>
vgl_pointset_3d<Type> bvgl_scaled_shape_3d<Type>::random_pointset(unsigned n_pts) const{
  vgl_vector_3d<Type> norm = base_.normal();
  vgl_pointset_3d<Type> ret;
  std::vector<vgl_point_3d<Type> > pts;
  unsigned n_req = n_pts, niter = 0;
  Type umin=Type(0), umax=Type(0), vmin=Type(0), vmax=Type(0), wmin = Type(0), wmax = Type(1);
  this->uv_bounds(umin, umax, vmin, vmax);
  while(n_req>0 && niter < 100*n_pts){
    Type u = (umax-umin)*(static_cast<Type>(std::rand())/static_cast<Type>(RAND_MAX)) + umin;
    Type v = (vmax-vmin)*(static_cast<Type>(std::rand())/static_cast<Type>(RAND_MAX)) + vmin;
    Type w = (wmax-wmin)*(static_cast<Type>(std::rand())/static_cast<Type>(RAND_MAX)) + wmin;
    vgl_point_3d<Type> p;
    if((*this)(u, v, w, p)){
      pts.push_back(p);
      n_req--;
    }else niter++;
  }
  if(n_req !=0)
    std::cout << "Warning! Insufficient number of points " << pts.size() << " instead of " << n_pts << '\n';
  //map the 2-d points back to the 3-d space


  for(typename std::vector<vgl_point_3d<Type> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    ret.add_point_with_normal(*pit,norm);
  }
  return ret;
}
#undef BVGL_SCALED_SHAPE_3D_INSTANTIATE
#define BVGL_SCALED_SHAPE_3D_INSTANTIATE(Type) \
template class bvgl_scaled_shape_3d<Type >

#endif // bvgl_scaled_shape_3d_hxx_
