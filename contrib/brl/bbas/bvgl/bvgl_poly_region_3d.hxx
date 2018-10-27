// This is brl/bbas/bvgl/bvgl_poly_region_3d.hxx
#ifndef bvgl_poly_region_3d_hxx_
#define bvgl_poly_region_3d_hxx_
#include <cstdlib>
#include <cmath>
#include <limits>
#include <iostream>
#include "bvgl_poly_region_3d.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
// plane fit to head cheek
//vgl_plane_3d -0.461582 x +0.0522239 y -0.885559 z +94.1097
//z(x,y) = (-0.461582*x + 0.0522239*y + 94.1)/0.885559


template <class Type>
bvgl_poly_region_3d<Type>::bvgl_poly_region_3d(vnl_vector<Type> const& u_coefs, vnl_vector<Type> const& v_coefs, vgl_vector_3d<Type> const&  normal,
                                               vgl_point_3d<Type> const& origin, Type tolerance):
  tolerance_(tolerance), u_coefs_(u_coefs), v_coefs_(v_coefs), origin_(origin){
  Type len = normal.length();
  unit_normal_ = normal/len;
  if(std::fabs(unit_normal_.z()) < 0.0001){
    std::cout << "FATAL! - plane parallel to the z axis a singular condition \n";
    return;
  }
  // initially assume that the normal should point into the positive coordinate half space
  // the user can flip the diretions later if desired.
  vgl_point_3d<Type> p_pos = origin_ + vgl_vector_3d<Type>(1.0e15,1.0e15, 1.0e15);
  this->set_point_positive(p_pos);
  plane_ = vgl_plane_3d<Type>(unit_normal_, origin);
  plane_.plane_coord_vectors(u_vec_, v_vec_);
  // normally the sense of the plane vectors doesn't matter
  // as they are used in both projecting to and from the plane
  if(u_vec_.x()< Type(0))
    u_vec_ = -u_vec_;
  if(v_vec_.y()< Type(0))
    v_vec_ = -v_vec_;
  // construct 2-d poly approximation
  poly_2d_.new_sheet();
  for(Type t = Type(0); t<Type(1); t+=0.025){
    vgl_point_2d<Type> p2d(u(t), v(t));
    poly_2d_.push_back(p2d);
  }
}
template <class Type>
vnl_vector<Type> bvgl_poly_region_3d<Type>::mono(Type t, unsigned size) const{
  vnl_vector<Type> m(size);
  m[0]=Type(1);
  for(unsigned i = 1; i<size; ++i)
    m[i]=t*m[i-1];
  return m;
}

template <class Type>
Type bvgl_poly_region_3d<Type>::u(Type t) const{
  unsigned size = static_cast<unsigned>(u_coefs_.size());
  Type ret = dot_product(mono(t,size), u_coefs_);
  return ret;
}


template <class Type>
Type bvgl_poly_region_3d<Type>::v(Type t) const{
  unsigned size = static_cast<unsigned>(v_coefs_.size());
  Type ret = dot_product(mono(t,size), v_coefs_);
  return ret;
}


template <class Type>
vgl_point_3d<Type> bvgl_poly_region_3d<Type>::p(Type t) const{
  Type uc = u(t), vc = v(t);
  Type x = u_vec_.x()*uc + origin_.x();
  Type y = v_vec_.y()*vc + origin_.y();
  Type z = (plane_.a()*x + plane_.b()*y + plane_.d())/plane_.c();
  vgl_point_3d<Type> ret(x, y, z);
  return ret;
}


template <class Type>
void bvgl_poly_region_3d<Type>::set_point_positive(vgl_point_3d<Type> const& p_pos){
  vgl_point_3d<Type> cp = vgl_closest_point(plane_, p_pos);
  vgl_vector_3d<Type> v = p_pos-cp;
  Type dp = dot_product(v, unit_normal_);
  if(dp<0){
    plane_.set(-plane_.a(), -plane_.b(), -plane_.c(), -plane_.d());
    unit_normal_ = -unit_normal_;
        u_vec_ = -u_vec_;
        v_vec_ = -v_vec_;
  }
}

template <class Type>
bool bvgl_poly_region_3d<Type>::in(vgl_point_3d<Type> const& p) const{
  vgl_point_3d<Type> cp = vgl_closest_point(plane_, p);
  Type len = (p-cp).length();
  if(len>tolerance_)
    return false;
  vgl_vector_3d<Type> del = p-origin_;
  Type pu = dot_product(del, u_vec_), pv = dot_product(del, v_vec_);
  vgl_point_2d<Type> p2d(pu, pv);
  return poly_2d_.contains(p2d);
}
template <class Type>
bool bvgl_poly_region_3d<Type>::signed_distance(vgl_point_3d<Type> const& p, Type& dist) const{
  vgl_point_3d<Type> cp = vgl_closest_point(plane_, p);
  if(!in(cp)){
    dist = std::numeric_limits<Type>::max();
    return false;
  }
  vgl_vector_3d<Type> v = p-cp;
  dist = dot_product(v, unit_normal_);
  return true;
}

template <class Type>
vgl_point_3d<Type> bvgl_poly_region_3d<Type>::centroid() const{
  Type cx=Type(0), cy=Type(0), cz=Type(0);
  const std::vector<vgl_point_2d<Type> >& pts = poly_2d_[0];
  if(pts.size()==0)
    return vgl_point_3d<Type>(cx, cy, cz);
  double n = Type(0);
  for(typename std::vector<vgl_point_2d<Type> >::const_iterator pit = pts.begin();
      pit != pts.end(); ++pit, n+=Type(1)){
                Type x = u_vec_.x()*pit->x() + origin_.x();
        Type y = v_vec_.y()*pit->y() + origin_.y();
        Type z = (plane_.a()*x + plane_.b()*y + plane_.d())/plane_.c();

    cx += x; cy += y; cz += z;
  }
  cx /= n; cy /= n; cz /= n;
  return vgl_point_3d<Type>(cx, cy, cz);
}
template <class Type>
vgl_box_2d<Type> bvgl_poly_region_3d<Type>::bounding_box_2d() const{
  // get a bounding box for the planar polygon
  vgl_box_2d<Type> bb;
  std::vector<vgl_point_2d<Type> > verts = poly_2d_[0];
  unsigned n = static_cast<unsigned>(verts.size());
  std::vector<vgl_point_2d<Type> > pts;
  for(unsigned i = 0; i<n; ++i){
    const vgl_point_2d<Type> &p_i = verts[i];
    pts.push_back(p_i); // poly pts are part of the set
    bb.add(p_i);
  }
  return bb;
}

template <class Type>
vgl_box_3d<Type> bvgl_poly_region_3d<Type>::bounding_box_3d() const{
  vgl_box_2d<Type> bb = this->bounding_box_2d();
  Type umin = bb.min_x(), umax = bb.max_x();
  Type vmin = bb.min_y(), vmax = bb.max_y();
  vgl_vector_3d<Type> del_min = umin*u_vec_ + vmin*v_vec_;
  vgl_point_3d<Type> pmin_3d = origin_ + del_min;
  vgl_vector_3d<Type> del_max = umax*u_vec_ + vmax*v_vec_;
  vgl_point_3d<Type> pmax_3d = origin_ + del_max;
  vgl_box_3d<Type> ret;
  ret.add(pmin_3d); ret.add(pmax_3d);
  return ret;
}

template <class Type>
vgl_pointset_3d<Type> bvgl_poly_region_3d<Type>::random_pointset(unsigned n_pts) const{
  vgl_box_2d<double> bb = this->bounding_box_2d();
  std::vector<vgl_point_2d<Type> > pts;
  unsigned n = static_cast<unsigned>(poly_2d_[0].size());
  for(unsigned i = 0; i<n; ++i)
   pts.push_back(poly_2d_[0][i]);
  unsigned n_req = n_pts - n, niter = 0;
  Type xmin = bb.min_x(), xmax = bb.max_x();
  Type ymin = bb.min_y(), ymax = bb.max_y();
  while(n_req>0 && niter < 100*n_pts){
    Type x = (xmax-xmin)*(static_cast<Type>(std::rand())/static_cast<Type>(RAND_MAX)) + xmin;
    Type y = (ymax-ymin)*(static_cast<Type>(std::rand())/static_cast<Type>(RAND_MAX)) + ymin;
    if(poly_2d_.contains(x, y)){
      pts.push_back(vgl_point_2d<Type>(x,y));
      n_req--;
    }else niter++;
  }
  if(n_req !=0)
    std::cout << "Warning! Insufficient number of points " << pts.size() << " instead of " << n_pts << '\n';
  //map the 2-d points back to the 3-d space
  vgl_pointset_3d<Type> ret;
  for(typename std::vector<vgl_point_2d<Type> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit){
    Type pu = pit->x(), pv = pit->y();
    vgl_vector_3d<Type> del = pu*u_vec_ + pv*v_vec_;
    vgl_point_3d<Type> p3d = origin_ + del;
    ret.add_point_with_normal(p3d, unit_normal_);
  }
  return ret;
}

#undef BVGL_POLY_REGION_3D_INSTANTIATE
#define BVGL_POLY_REGION_3D_INSTANTIATE(Type) \
template class bvgl_poly_region_3d<Type >

#endif // bvgl_poly_region_3d_hxx_
