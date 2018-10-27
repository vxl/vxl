// This is brl/bbas/bvgl/bvgl_spline_region_3d.hxx
#ifndef bvgl_spline_region_3d_hxx_
#define bvgl_spline_region_3d_hxx_
#include <cstdlib>
#include <cmath>
#include <limits>
#include "bvgl_spline_region_3d.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
template <class Type>
void bvgl_spline_region_3d<Type>::plane_to_world(Type u, Type v, vgl_point_3d<Type>& p3d) const{
  vgl_vector_3d<Type> plane_vec = u*u_vec_ + v*v_vec_;
  p3d =  origin_+ plane_vec;
}
template <class Type>
bool bvgl_spline_region_3d<Type>::world_to_plane(vgl_point_3d<Type> p3d, Type& u, Type& v, Type tolerance) const{
  u = Type(0); v = Type(0); Type roundoff = Type(1)/Type(1000);
  vgl_point_3d<Type> cp = vgl_closest_point(plane_, p3d);
  Type len = (p3d-cp).length();
  if(len>(tolerance+roundoff))
    return false;
  vgl_vector_3d<Type> plane_vec = cp-origin_;
  u = dot_product(plane_vec, u_vec_);
  v = dot_product(plane_vec, v_vec_);
  return true;
}

template <class Type>
bvgl_spline_region_3d<Type>::bvgl_spline_region_3d(std::vector<vgl_point_3d<Type> > const& knots, Type tolerance):
  tolerance_(tolerance),cang_(Type(1)), sang_(Type(0)), su_(Type(1)), sv_(Type(1)), tv_(vgl_vector_3d<Type>(Type(0), Type(0), Type(0)))
{
  if(knots.size()<3){
          std::cout << "FATAL - two few points to construct spline region\n";
          return;
  }
  vgl_fit_plane_3d<Type> fitter;
  fitter.clear();
  Type limit = static_cast<Type>(1);
  for(typename std::vector<vgl_point_3d<Type> >::const_iterator pit = knots.begin();
      pit != knots.end(); ++pit)
    fitter.add_point(pit->x(), pit->y(), pit->z());
  double temp = fitter.fit(&std::cout);
  bool success =  std::sqrt(temp)< limit;
  if(!success){
    std::cout << "Fitting error is " << std::sqrt(temp) << " exceeds the limit " << limit << '\n';
    std::cout << " Could not fit knots to a plane -- FATAL! \n";
    return;
  }
  plane_ = vgl_plane_3d<Type>(fitter.get_plane());

  unit_normal_ = plane_.normal();

  vgl_point_3d<Type> cp = vgl_closest_point(plane_, knots[0]);
  origin_.set(cp.x(), cp.y(), cp.z());
  plane_.plane_coord_vectors(u_vec_, v_vec_);

  std::vector<vgl_point_3d<Type> > planar_knots;
  for(typename std::vector<vgl_point_3d<Type> >::const_iterator pit = knots.begin();
      pit != knots.end(); ++pit){
    vgl_point_3d<Type> cp = vgl_closest_point(plane_, *pit);
    planar_knots.push_back(cp);
  }
  spline_3d_ = vgl_cubic_spline_3d<Type>(planar_knots, Type(0.5), true);
  // normally the sense of the plane vectors doesn't matter
  // as they are used in both projecting to and from the plane
  // it is convenient to have them in the positive half space
  if(u_vec_.x()< Type(0))
    u_vec_ = -u_vec_;
  if(v_vec_.y()< Type(0))
    v_vec_ = -v_vec_;
  vgl_vector_3d<Type> newn = cross_product(u_vec_,v_vec_);
  Type len = newn.length();
  newn/=len;
  unit_normal_ = newn;
  unit_normal_/=length(newn);
  plane_ = vgl_plane_3d<Type>(unit_normal_,origin_);

  // create the spline on the plane
  bool closed = true;
  std::vector<vgl_point_2d<Type> > knots_2d;
  for(typename std::vector<vgl_point_3d<Type> >::iterator kit=planar_knots.begin();
      kit != planar_knots.end(); ++kit){
    Type u, v;
    if(!this->world_to_plane(*kit, u, v, tolerance_))
      continue;
    vgl_point_2d<Type> p2d(u, v);
    knots_2d.push_back(p2d);
  }
  spline_2d_.set_knots(knots_2d, closed);
  spline_3d_.set_s(Type(0.5));

  // planar polygon approximation to the spline and 2-d spline spec.
      poly_2d_.new_sheet();
  for(Type t = Type(0); t<=spline_3d_.max_t(); t+=tolerance_){
    vgl_point_3d<Type> p3d = spline_3d_(t);
    Type u, v;
    if(!this->world_to_plane(p3d, u, v, tolerance_))
      continue;
    vgl_point_2d<Type> p2d(u, v);
    poly_2d_.push_back(p2d);
  }
  centroid_2d_ = this->compute_centroid_2d();
  centroid_3d_ = this->compute_centroid();
}
template <class Type>
bvgl_spline_region_3d<Type>::bvgl_spline_region_3d(vgl_pointset_3d<Type> const& ptset, Type tolerance):
  cang_(Type(1)), sang_(Type(0)), su_(Type(1)), sv_(Type(1)), tv_(vgl_vector_3d<Type>(Type(0), Type(0), Type(0)))
{
  std::vector<vgl_point_3d<Type> > knots = ptset.points();
  *this = bvgl_spline_region_3d<Type>(knots, tolerance);
}

template <class Type>
bvgl_spline_region_3d<Type>::bvgl_spline_region_3d(std::vector<vgl_point_2d<Type> > const& knots_2d, vgl_vector_3d<Type> const& normal,
                                                   vgl_point_3d<Type> const& origin, Type tolerance):tolerance_(tolerance), origin_(origin),cang_(Type(1)), sang_(Type(0)), su_(Type(1)), sv_(Type(1)), tv_(vgl_vector_3d<Type>(Type(0), Type(0), Type(0))){
  bool closed = true;
  spline_2d_.set_knots(knots_2d, closed);
  spline_2d_.set_s(Type(0.5));
  Type len = normal.length();
  unit_normal_ = normal/len;
  plane_ = vgl_plane_3d<Type>(unit_normal_, origin_);
  // initially assume that the normal should point into the positive coordinate half space
  // the user can flip the diretions later if desired.
  vgl_point_3d<Type> p_pos = origin_ + vgl_vector_3d<Type>(1.0e15,1.0e15, 1.0e15);
  this->set_point_positive(p_pos);
  plane_.plane_coord_vectors(u_vec_, v_vec_);
  // normally the sense of the plane vectors doesn't matter
  // as they are used in both projecting to and from the plane
  if(u_vec_.x()< Type(0))
    u_vec_ = -u_vec_;
  if(v_vec_.y()< Type(0))
    v_vec_ = -v_vec_;
  // construct 2-d poly approximation
  poly_2d_.new_sheet();
  for(Type t = Type(0); t<=spline_2d_.max_t(); t+=tolerance_)
    poly_2d_.push_back(vgl_point_2d<Type>(spline_2d_(t)));

  std::vector<vgl_point_3d<double> > knots_3d;
  for(typename std::vector<vgl_point_2d<Type> >::const_iterator kit = knots_2d.begin();
      kit != knots_2d.end(); ++kit){
    vgl_point_3d<Type> p3d;
    this->plane_to_world(kit->x(), kit->y(), p3d);
    knots_3d.push_back(p3d);
  }
  spline_3d_.set_knots(knots_3d, closed);
  spline_3d_.set_s(Type(0.5));
  centroid_2d_ = this->compute_centroid_2d();
  centroid_3d_ = this->compute_centroid();
}
template <class Type>
void bvgl_spline_region_3d<Type>::set_point_positive(vgl_point_3d<Type> const& p_pos){
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
bool bvgl_spline_region_3d<Type>::in(vgl_point_3d<Type> const& p3d) const{
  Type u, v;
  if(!world_to_plane(p3d, u, v, tolerance_))
    return false;

  vgl_point_2d<Type> p2d(u, v);
  return poly_2d_.contains(p2d);
}

template <class Type>
vgl_point_3d<Type>  bvgl_spline_region_3d<Type>::closest_point(vgl_point_3d<Type> const& p) const{
  vgl_point_3d<Type> cp = vgl_closest_point(plane_, p);
  if(this->in(cp))
    return cp;
  return vgl_closest_point(spline_3d_,p);
}

template <class Type>
bool bvgl_spline_region_3d<Type>::signed_distance(vgl_point_3d<Type> const& p, Type& dist) const{
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
vgl_point_3d<Type> bvgl_spline_region_3d<Type>::operator () (Type t) const{
  vgl_point_2d<Type> p2d = spline_2d_(t);
  vgl_point_3d<Type> ret;
  this->plane_to_world(p2d.x(), p2d.y(), ret);
  return ret;
}
template <class Type>
vgl_point_2d<Type> bvgl_spline_region_3d<Type>::compute_centroid_2d() const{
  Type cx=Type(0), cy=Type(0);
  std::vector<vgl_point_2d<Type> > pts = spline_2d_.knots();
  if(pts.size()==0)
    return vgl_point_2d<Type>(cx, cy);
  double n = Type(0);
  for(typename std::vector<vgl_point_2d<Type> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit, n+=Type(1)){
    cx += pit->x(); cy += pit->y();
  }
  cx /= n; cy /= n;
  return vgl_point_2d<Type>(cx, cy);
}
template <class Type>
vgl_point_3d<Type> bvgl_spline_region_3d<Type>::compute_centroid() const{

  Type cx=Type(0), cy=Type(0), cz=Type(0);
  std::vector<vgl_point_3d<Type> > pts = spline_3d_.knots();
  if(pts.size()==0)
    return vgl_point_3d<Type>(cx, cy, cz);
  double n = Type(0);
  for(typename std::vector<vgl_point_3d<Type> >::iterator pit = pts.begin();
      pit != pts.end(); ++pit, n+=Type(1)){
    cx += pit->x(); cy += pit->y(); cz += pit->z();
  }
  cx /= n; cy /= n; cz /= n;
  return vgl_point_3d<Type>(cx, cy, cz);
}
// approximate using the polgon
template <class Type>
Type bvgl_spline_region_3d<Type>::area() const{
  unsigned n = static_cast<unsigned>(poly_2d_[0].size());
  vgl_point_2d<Type> pm = poly_2d_[0][0];
  Type a = Type(0);
  for(unsigned i = 1; i<n; ++i){
    vgl_point_2d<Type> pi = poly_2d_[0][i];
    a += pm.x()*pi.y()-pi.x()*pm.y();
    pm = pi;
  }
  vgl_point_2d<Type> pn = poly_2d_[0][n-1];
  a += pn.x()*pm.y()-pm.x()*pn.y();
  return std::fabs(a/Type(2));
}
template <class Type>
void bvgl_spline_region_3d<Type>::set_principal_eigenvector(vgl_vector_3d<Type> const& L1){
  if(L1.length()==Type(0)){
    sang_ = Type(0);
    cang_ = Type(1);
    return;
  }
  //find the angle needed to rotate u_vec_ to L1
  // insure unit vectors
  vgl_vector_3d<Type> u_unit = u_vec_/u_vec_.length();
  vgl_vector_3d<Type> L1_unit = L1/L1.length();
  // compute sine of angle
  vgl_vector_3d<Type> cp = cross_product(u_unit,L1_unit);
  sang_ = dot_product(unit_normal_,cp);
  // compute cosine of angle
  cang_ = dot_product(u_unit,L1_unit);
  // normalize to produce a valid rotation
  Type den = std::sqrt(sang_*sang_ + cang_*cang_);
  sang_/=den; cang_/=den;
}
template <class Type>
bool bvgl_spline_region_3d<Type>::inverse_vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& inv, Type tolerance) const{
  // map target point to plane coordinates
  Type u, v, tol=tolerance;
  if(tolerance<Type(0))
    tol = tolerance_;
  vgl_point_3d<Type> pmt = p-tv_;
  if(!this->world_to_plane(pmt, u, v, tol))
    return false;
  Type su_inv = Type(1)/su_, sv_inv = Type(1)/sv_;
  vgl_point_2d<double> c = this->centroid_2d();
  Type dvx = u-c.x(), dvy = v-c.y();
  // rotate around the centroid by the negative rotation angle
  Type sang = -sang_;
  Type rdvx = cang_*dvx - sang*dvy;
  // principal offset
  rdvx -= principal_offset_;
  Type rdvy = sang*dvx + cang_*dvy;
  // undo anisotropic scaling
  Type srdvx = su_inv*rdvx, srdvy = sv_inv*rdvy;
  // restore principal offset
  rdvx += principal_offset_;
  // rotate back to original plane coordinate frame
  Type rinv_srdvx =  cang_*srdvx + sang*srdvy;
  Type rinv_srdvy = -sang*srdvx + cang_*srdvy;
  Type uinv = rinv_srdvx + c.x(), vinv = rinv_srdvy + c.y();
  // convert back to 3d coordinates, i.e. the source point corresponding to the target
  vgl_point_3d<Type> p3d;
  this->plane_to_world(uinv, vinv, p3d);
  // the difference is the inverse vector field
  inv = p3d - p;
  return true;
}
template <class Type>
bool bvgl_spline_region_3d<Type>::vector_field(vgl_point_3d<Type> const& p, vgl_vector_3d<Type>& vf, vgl_vector_3d<Type> const& tv) const{
// map target point to plane coordinates
  Type u, v;
  if(!this->world_to_plane(p, u, v, tolerance_))
    return false;
  vgl_point_2d<double> c = this->centroid_2d();
  Type dvx = u-c.x(), dvy = v-c.y();
  // rotate around the centroid by the negative rotation angle
  Type sang = -sang_;
  Type rdvx = cang_*dvx - sang*dvy;
  Type rdvy = sang*dvx + cang_*dvy;
  // principal offset
  rdvx -= principal_offset_;
  // anisotropic scaling
  Type srdvx = su_*rdvx, srdvy = sv_*rdvy;
  //restore principal offset
  srdvx += principal_offset_;
  // rotate back to original plane coordinate frame
  Type rinv_srdvx =  cang_*srdvx + sang*srdvy;
  Type rinv_srdvy = -sang*srdvx + cang_*srdvy;
  Type uinv = rinv_srdvx + c.x(), vinv = rinv_srdvy + c.y();
  // convert back to 3d coordinates, i.e. the source point corresponding to the target
  vgl_point_3d<Type> p3d;
  this->plane_to_world(uinv, vinv, p3d);
  // the difference is the inverse vector field
  vf = (p3d-p) + tv;
  return true;
}

template <class Type>
bvgl_spline_region_3d<Type> bvgl_spline_region_3d<Type>::scale(Type su, Type sv, vgl_vector_3d<Type> const& tv,
                                                               vgl_vector_3d<Type> const& L1, bool verbose) const{
  Type old_sang = sang_, old_cang = cang_;
  // bad practice, but harmless in this case. saves repeating code for the eigenvector rotation
  // cang and sang will be reset to their original values
  bvgl_spline_region_3d<Type>* nconst = const_cast<bvgl_spline_region_3d<Type>* >(this);
  if(L1.length()==Type(0))
    nconst->set_principal_eigenvector(u_vec_);
  else
    nconst->set_principal_eigenvector(L1);
  // negate the angle since the cross section is to be rotated in the opposite sense
  Type sang = -sang_;
  vgl_point_2d<double> c = this->centroid_2d();
  std::vector<vgl_point_2d<Type> > pts = spline_2d_.knots();
  std::vector<vgl_point_3d<Type> > scaled_pts;
  unsigned k = 0;
  for(typename std::vector<vgl_point_2d<Type> >::iterator kit = pts.begin();
      kit != pts.end(); ++kit, k++){
    // subtract off the centroid, i.e. rotate about the centroid
    vgl_vector_2d<Type> dv = (*kit)-c;
    Type dvx = dv.x(), dvy = dv.y();
    // rotate around the centroid by negative rotation angle
    Type rdvx = cang_*dvx - sang*dvy;
    Type rdvy = sang*dvx + cang_*dvy;
    // anisotropic scaling along the principal direction
    Type srdvx = su*rdvx, srdvy = sv*rdvy;
    // rotate back to original plane coordinate frame
    Type rinv_srdvx =  cang_*srdvx + sang*srdvy;
    Type rinv_srdvy = -sang*srdvx + cang_*srdvy;
    Type uinv = rinv_srdvx+c.x(), vinv = rinv_srdvy+c.y();
    // convert back to 3d
    vgl_point_3d<Type> p3d;
    this->plane_to_world(uinv, vinv, p3d);
    //translate the point 3-d to the position defined by tv
    scaled_pts.push_back(p3d + tv);
  }
  // set state back to const correctness
  nconst->sang_ = old_sang; nconst->cang_ = old_cang;
  return bvgl_spline_region_3d<Type>(scaled_pts, tolerance_);
}

template <class Type>
bvgl_spline_region_3d<Type> bvgl_spline_region_3d<Type>::scale(Type s, vgl_vector_3d<Type>const& tv) const{
  return this->scale(s, s, tv, vgl_vector_3d<Type>(Type(0), Type(0), Type(0)));
}

template <class Type>
vgl_box_2d<Type> bvgl_spline_region_3d<Type>::bounding_box_2d() const{
  vgl_box_2d<Type> bb;
  std::vector<vgl_point_2d<Type> > verts = poly_2d_[0];
  unsigned n = static_cast<unsigned>(verts.size());
  std::vector<vgl_point_2d<Type> > pts;
  for(unsigned i = 0; i<n; ++i){
    const vgl_point_2d<Type> &p_i = verts[i];
    bb.add(p_i);
  }
  return bb;
}

template <class Type>
vgl_box_3d<Type> bvgl_spline_region_3d<Type>::bounding_box() const{
  vgl_box_2d<Type> bb = this->bounding_box_2d();
  vgl_point_2d<Type> pmin = bb.min_point(), pmax = bb.max_point();
  vgl_point_3d<Type> pmin_3d, pmax_3d;
  this->plane_to_world(pmin.x(), pmin.y(), pmin_3d);
  this->plane_to_world(pmax.x(), pmax.y(), pmax_3d);
  vgl_box_3d<Type> ret;
  ret.add(pmin_3d); ret.add(pmax_3d);
  return ret;
}

template <class Type>
vgl_pointset_3d<Type> bvgl_spline_region_3d<Type>::random_pointset(unsigned n_pts) const{
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
    vgl_point_3d<Type> p3d;
    this->plane_to_world(pit->x(), pit->y(), p3d);
    ret.add_point_with_normal(p3d, unit_normal_);
  }
  return ret;
}
#undef BVGL_SPLINE_REGION_3D_INSTANTIATE
#define BVGL_SPLINE_REGION_3D_INSTANTIATE(Type) \
template class bvgl_spline_region_3d<Type >

#endif // bvgl_spline_region_3d_hxx_
