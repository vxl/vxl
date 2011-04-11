// This is gel/mrc/vpgl/vpgl_generic_camera.txx
#ifndef vpgl_generic_camera_txx_
#define vpgl_generic_camera_txx_
//:
// \file

#include "vpgl_generic_camera.h"
#include <vnl/vnl_numeric_traits.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vcl_iostream.h>
//-------------------------------------------
template <class T>
vpgl_generic_camera<T>::vpgl_generic_camera()
{
  // rays_ is empty and min ray and max ray origins are (0 0 0)
}

//------------------------------------------
template <class T>
vpgl_generic_camera<T>::
vpgl_generic_camera( vbl_array_2d<vgl_ray_3d<T> > const& rays)
{
  int nc = rays.cols(), nr = rays.rows();
  //compute bounds on ray origins
  double min_dist = vnl_numeric_traits<double>::maxval;
  double max_dist = 0.0;
  vgl_point_3d<T> datum(T(0), T(0), T(0));
  for(int v = 0; v<nr; ++v)
    for(int u = 0; u<nc; ++u){
      vgl_point_3d<T> org = rays[v][u].origin();
      double d = vgl_distance(datum, org);
      if(d>max_dist){
        max_dist = d;
        max_ray_origin_ = org;
      }
      if(d<min_dist){
        min_dist = d;
        min_ray_origin_ = org;
      }
    }
  // form the pyramid for efficient projection
  // find the number of levels
  double dim = nc;
  if(nr<nc)
    dim = nr;
  double lv = vcl_log(dim)/vcl_log(2.0);
  n_levels_ = static_cast<int>(lv);// round down
  rays_.resize(n_levels_);
  nr_.resize(n_levels_);
  nc_.resize(n_levels_);
  rays_[0]=rays;
  nr_[0]=nr; nc_[0]=nc;
  int nrlv = nr/2, nclv = nc/2;
  for(int lev = 1; lev<n_levels_; ++lev){
    rays_[lev].resize(nrlv, nclv);
    nr_[lev]=nrlv; nc_[lev]=nclv;
    for(int r = 0; r<nrlv; ++r)
      for(int c = 0; c<nclv; ++c)// nearest neighbor downsampling
        rays_[lev][r][c] = rays_[lev-1][2*r][2*c];
    //next level
    nrlv /= 2; nclv /= 2;
  }  
}
// the ray closest to the given 3-d point is selected
// note that the ray is taken to be an infinite 3-d line
// and so the bound of the ray origin is not taken into account
//
template <class T>
void vpgl_generic_camera<T>::nearest_ray(int level, 
                                         vgl_point_3d<T> const& p,
                                         int start_r, int end_r, 
                                         int start_c, int end_c,
                                         int& nearest_r, int& nearest_c) const
{
  assert(level>=0 && level<n_levels_);
  assert(start_r>=0 && end_r < nr_[level]);
  assert(start_c>=0 && end_c < nc_[level]);
  nearest_r = 0, nearest_c = 0;
  double min_d = vnl_numeric_traits<double>::maxval;
  for(int r = start_r; r<=end_r; ++r)
    for(int c = start_c; c<=end_c; ++c){
      double d = vgl_distance(rays_[level][r][c], p);
      if(d<min_d){
        min_d=d;
        nearest_r = r; 
        nearest_c = c;
      }
    }
}
// refine the projection to sub-pixel accuracy
// using a Taylor series approximation in the ray neigborhood
template <class T>
void vpgl_generic_camera<T>::
refine_projection(int nearest_c, int nearest_r, vgl_point_3d<T> const& p,
                  T& u, T& v) const
{
  // the ray closest to the projected 3-d point 
  vgl_ray_3d<T> r = rays_[0][nearest_r][nearest_c];

  //extract the neigboring ray origins and corresponding pixel differences
  vcl_vector<vgl_point_3d<T> > origins;
  vcl_vector<double> du, dv;
  if(nearest_r>0){
    origins.push_back(rays_[0][nearest_r-1][nearest_c].origin());
    du.push_back(0.0);
    dv.push_back(-1.0);
  }
  if(nearest_c>0){
    origins.push_back(rays_[0][nearest_r][nearest_c-1].origin());
    du.push_back(-1.0);
    dv.push_back(0.0);
  }
  int nbl = static_cast<int>(rows())-1;
  if(nearest_r<nbl){
    origins.push_back(rays_[0][nearest_r+1][nearest_c].origin());
    du.push_back(0.0);
    dv.push_back(1.0);
  }
  int nrght = static_cast<int>(cols())-1;
  if(nearest_c<nrght){
    origins.push_back(rays_[0][nearest_r][nearest_c+1].origin());
    du.push_back(1.0);
    dv.push_back(0.0);
  }
  //no neighbors, shouldn't happen!
  if(!origins.size()){
    u = static_cast<T>(nearest_c);
    v = static_cast<T>(nearest_r);
    return;
  }

  // do a local Taylor series expansion in the closest ray neigborhood
  double tol = 1.0e-8;
  // partial derivatives
  double dudx = 0.0, dudy = 0.0, dudz = 0.0;
  double dvdx = 0.0, dvdy = 0.0, dvdz = 0.0;
  double sx = 0.0, sy = 0.0, sz = 0.0;
  for(unsigned i = 0; i< origins.size(); ++i){
    vgl_point_3d<T> org_i = origins[i];
    vgl_point_3d<T> cp = vgl_closest_point(org_i, r);
    vgl_vector_3d<T> q = org_i-cp;
    double du_i = du[i], dv_i = dv[i];
    //accumulate partial derivatives based on neighbors
    if(vcl_fabs(q.x())>tol){
      dudx += du_i/q.x();
      dvdx += dv_i/q.x();
      sx +=1.0;
    }
    if(vcl_fabs(q.y())>tol){
      dudy += du_i/q.y();
      dvdy += dv_i/q.y();
      sy +=1.0;
    }
    if(vcl_fabs(q.z())>tol){
      dudz += du_i/q.z();
      dvdz += dv_i/q.z();
      sz +=1.0;
    }
  }
  //normalize partial derivatives
  if(sx>0.0){
    dudx /= sx;
    dvdx /= sx;
  }
  if(sy>0.0){
    dudy /= sy;
    dvdy /= sy;
  }
  if(sz>0.0){
    dudz /= sz;
    dvdz /= sz;
  }
  // the closest point on the nearest ray to the projected point
  vgl_point_3d<T> np = vgl_closest_point(p, r);

  // the corresponding perpendicular vector
  vgl_vector_3d<T> vp = p - np;

  // expand using the  Taylor series
  double eu = nearest_c, ev = nearest_r;
  eu = eu + dudx*vp.x() + dudy*vp.y() + dudz*vp.z();
  ev = ev + dvdx*vp.x() + dvdy*vp.y() + dvdz*vp.z();
  u = static_cast<T>(eu);
  v = static_cast<T>(ev);
}
// projects by exhaustive search in a pyramid. 
template <class T>
void vpgl_generic_camera<T>::project(const T x, const T y, const T z,
                                     T& u, T& v) const
{
  vgl_point_3d<T> p(x, y, z);
  int lev = n_levels_-1;
  int start_r = 0, end_r = nr_[lev];
  int start_c = 0, end_c = nc_[lev];
  int nearest_r = 0, nearest_c = 0;
  for(; lev >= 0; --lev){
    if(start_r<0) start_r = 0;
    if(start_c<0) start_c = 0;
    if(end_r>=nr_[lev]) end_r = nr_[lev]-1;
    if(end_c>=nc_[lev]) end_c = nc_[lev]-1;
    nearest_ray(lev, p, start_r, end_r, start_c, end_c,
                nearest_r, nearest_c);
    // compute new bounds
    start_r = 2*nearest_r-1; start_c = 2*nearest_c-1;
    end_r = start_r + 2; end_c = start_c +2;
    // check if the image sizes are odd, so search range is extended
    if(lev ==1&&nr_[0]%2!=0) end_r++;
    if(lev ==1&&nc_[0]%2!=0) end_c++;
  }
  // refine to sub-pixel accuracy using a Taylor series approximation
  this->refine_projection(nearest_c, nearest_r, p, u, v);
}
// a ray specified by an image location (can be sub-pixel)
template <class T>
vgl_ray_3d<T> vpgl_generic_camera<T>::ray(const T u, const T v)
{
  double du = static_cast<double>(u);
  double dv = static_cast<double>(v);
  assert(du>=0.0&&dv>=0.0);
  int iu = static_cast<int>(du);
  int iv = static_cast<int>(dv);
  assert(iu<static_cast<int>(cols()) && iv<static_cast<int>(rows()));
  //check for integer pixel coordinates
  if((du-iu) == 0.0 && (dv-iv) == 0.0)
    return rays_[0][iv][iu];
  // u or v is sub-pixel so find interpolated ray
  //find neighboring rays and pixel distances to the sub-pixel location
  vcl_vector<double> dist;
  vcl_vector<vgl_ray_3d<T> > nrays;
  // ray above
  if(iv>0){
    vgl_ray_3d<T> ru = rays_[0][iv-1][iu];
    nrays.push_back(ru);
    double d = vcl_sqrt((iv-1-dv)*(iv-1-dv) + (iu-du)*(iu-du));
    if(d==0.0) 
      return ru;
    dist.push_back(1.0/d);
  }
  // ray to the left
  if(iu>0){
    vgl_ray_3d<T> rl = rays_[0][iv][iu-1];
    nrays.push_back(rl);
    double d = vcl_sqrt((iv-dv)*(iv-dv) + (iu-1-du)*(iu-1-du));
    if(d==0.0) 
      return rl;
    dist.push_back(1.0/d);
  }
  // ray to the right
  int nrght = static_cast<int>(cols())-1;
  if(iu<nrght){
    vgl_ray_3d<T> rr = rays_[0][iv][iu+1];
    nrays.push_back(rr);
    double d = vcl_sqrt((iv-dv)*(iv-dv) + (iu+1-du)*(iu+1-du));
    if(d==0.0) 
      return rr;
    dist.push_back(1.0/d);
  }
  // ray below
  int nbl = static_cast<int>(rows())-1;
  if(iv<nbl){
    vgl_ray_3d<T> rd = rays_[0][iv+1][iu];
    nrays.push_back(rd);
    double d = vcl_sqrt((iv+1-dv)*(iv+1-dv) + (iu-du)*(iu-du));
    if(d==0.0) 
      return rd;
    dist.push_back(1.0/d);
  }
  // compute the interpolated ray
  double ox = 0.0, oy = 0.0, oz = 0.0, dx = 0.0, dy = 0.0, dz = 0.0;
  double sumw = 0.0;
  for(unsigned i = 0; i<nrays.size(); ++i){
    vgl_ray_3d<T> r = nrays[i];
    vgl_point_3d<T> org = r.origin();
    vgl_vector_3d<T> dir = r.direction();
    double w = dist[i];
    sumw += w;
    ox += w*org.x(); oy += w*org.y(); oz += w*org.z();
    dx += w*dir.x(); dy += w*dir.y(); dz += w*dir.z();
  }
  ox /= sumw;  oy /= sumw; oz /= sumw;
  dx /= sumw;  dy /= sumw; dz /= sumw;
  vgl_point_3d<T> avg_org(static_cast<T>(ox),
                          static_cast<T>(oy),
                          static_cast<T>(oz));
  vgl_vector_3d<T> avg_dir(static_cast<T>(dx),
                           static_cast<T>(dy),
                           static_cast<T>(dz));
  return vgl_ray_3d<T>(avg_org, avg_dir);
}


template <class T>
void vpgl_generic_camera<T>::print_orig(int level)
{
  for(int r = 0; r<nr_[level]; ++r){
    for(int c = 0; c<nc_[level]; ++c){
      vgl_point_3d<T> o = rays_[level][r][c].origin();
      vcl_cout << "(" << o.x() << ' ' << o.y() << ") "; 
    }
    vcl_cout << '\n';
  }
}
// Code for easy instantiation.
#undef vpgl_GENERIC_CAMERA_INSTANTIATE
#define vpgl_GENERIC_CAMERA_INSTANTIATE(T) \
template class vpgl_generic_camera<T >


#endif // vpgl_generic_camera_txx_
