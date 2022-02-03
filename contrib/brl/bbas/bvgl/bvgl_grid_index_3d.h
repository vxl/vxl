#ifndef bvgl_grid_index_3d_h_
#define bvgl_grid_index_3d_h_
//:
// \file
// \brief  A simple grid index to store a pointset, optionally with normals and optional scalar value
//
// \author J.L. Mundy
// \date   6 November 2015
//
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_bounding_box.h>
#include <vgl/vgl_line_3d_2_points.h>
template <class Type>
class bvgl_grid_index_3d{
 public:
 bvgl_grid_index_3d():
   has_scalars_(false),
   has_normals_(false),
   delta_x_(1.0),
   delta_y_(1.0),
   delta_z_(1.0),
   nx_(0),
   ny_(0),
   nz_(0),
   npts_(0),
   thresh_(1.0)
{}
  bvgl_grid_index_3d(unsigned nx, unsigned ny, unsigned nz, vgl_pointset_3d<Type> ptset, Type thresh = Type(1));
  bvgl_grid_index_3d(unsigned nx, unsigned ny, unsigned nz, vgl_pointset_3d<Type> ptset, std::vector<Type> scalars, Type thresh = Type(1));

  void index(vgl_point_3d<Type> const& p, unsigned& ix, unsigned& iy, unsigned& iz) const;

  vgl_pointset_3d<Type> cell_centroids(size_t min_pts_in_cell = 50);

  //: geometric queries
  //: the point contained in the grid closest to p or optionally to the normal plane passing through the closest point
  bool closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc) const;
  bool closest_point_as_index(vgl_point_3d<Type> const& p, size_t& pc_idx) const;
 
  //: closest point to p with associated scalar
  bool closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc, Type& scalar) const;
  bool closest_point_as_index(vgl_point_3d<Type> const& p, size_t& pc_idx, Type& scalar) const;

  // return up to k_max pts - returns false if none can be found
  bool k_nearest_pts_by_index(vgl_point_3d<Type> const& p, size_t k_max, std::vector<size_t> & pc_indices) const;
  
  //: the distance from p to the closest point or optionally its normal plane
  Type distance(vgl_point_3d<Type> const& p) const;
  Type distance(vgl_point_3d<Type> const& p, Type& scalar) const;

  std::vector<vgl_pointset_3d<Type> > intersecting_cells(vgl_box_3d<Type> const& box) const;

  void directional_neighbors(vgl_point_3d<Type> const& p, vgl_vector_3d<Type> const& dir, Type radius, vnl_vector<size_t>& indices) const;
  vgl_vector_3d<Type> cell_direction(vgl_point_3d<Type> const& p) const;
  //: accessors
  vgl_box_3d<Type> bounding_box() const {return bbox_;}
  unsigned npts() const {return npts_;}
  bool has_normals() const {return has_normals_;}
  bool has_scalars() const {return has_scalars_;}
 protected:
  static bool dist_less(const std::pair<size_t, Type>& pa, const std::pair<size_t, Type>& pb){
    return pa.second < pb.second;
  }
  bool has_scalars_;
  bool has_normals_;
  vbl_array_3d<std::vector<vgl_point_3d<Type > > >   p_grid_;
  vbl_array_3d<std::vector<vgl_vector_3d<Type > > >  n_grid_;//optional normal grid
  vbl_array_3d<std::vector<Type> >  s_grid_;//optional scalar grid
  vbl_array_3d<std::vector<size_t> >  idx_grid_;//grid of indices
  vgl_box_3d<Type> bbox_;
  Type xmin_;
  Type ymin_;
  Type zmin_;
  Type delta_x_;
  Type delta_y_;
  Type delta_z_;
  unsigned nx_;
  unsigned ny_;
  unsigned nz_;
  unsigned npts_;
  Type thresh_;
};
// implementation relatively short so include in the .h file
template <class Type>
//: compute 3-d grid index values from point coordinates
void bvgl_grid_index_3d<Type>::index(vgl_point_3d<Type> const& p, unsigned& ix, unsigned& iy, unsigned& iz) const{
  int ixi = static_cast<int>((p.x()-xmin_)/delta_x_);
  int iyi = static_cast<int>((p.y()-ymin_)/delta_y_);
  int izi = static_cast<int>((p.z()-zmin_)/delta_z_);
  ix = ixi;
  if(ixi<0) ix = 0;
  if(ixi>=int(nx_)) ix = nx_-1;
  iy = iyi;
  if(iyi<0) iy = 0;
  if(iyi>=int(ny_)) iy = ny_-1;
  iz = izi;
  if(izi<0) iz = 0;
  if(izi>=int(nz_)) iz = nz_-1;
}
//: constructors
template <class Type>
bvgl_grid_index_3d<Type>::bvgl_grid_index_3d(unsigned nx, unsigned ny, unsigned nz, vgl_pointset_3d<Type> ptset, Type thresh):
nx_(nx), ny_(ny), nz_(nz), has_normals_(ptset.has_normals()), npts_(0), thresh_(thresh), has_scalars_(ptset.has_scalars()){
  p_grid_ = vbl_array_3d<std::vector<vgl_point_3d<Type> > >(nx, ny, nz);
  if(has_normals_)
    n_grid_ = vbl_array_3d<std::vector<vgl_vector_3d<Type> > >(nx, ny, nz);
  if(has_scalars_)
    s_grid_ = vbl_array_3d<std::vector<Type> >(nx, ny, nz);
  idx_grid_ = vbl_array_3d<std::vector<size_t> >(nx, ny, nz);
  bbox_ = vgl_bounding_box(ptset);
  xmin_ = bbox_.min_x();
  ymin_ = bbox_.min_y();
  zmin_ = bbox_.min_z();
  Type dnx = static_cast<Type>(nx), dny = static_cast<Type>(ny), dnz = static_cast<Type>(nz);
  delta_x_ = bbox_.width()/dnx;
  delta_y_ = bbox_.height()/dny;
  delta_z_ = bbox_.depth()/dnz;
  unsigned ix =0, iy=0, iz =0;
  unsigned npts = ptset.npts();
  for(unsigned i = 0; i<npts; ++i){
    vgl_point_3d<Type> pi = ptset.p(i);
    this->index(pi, ix, iy, iz);
    p_grid_[ix][iy][iz].push_back(pi);
    idx_grid_[ix][iy][iz].push_back(i);
    npts_++;
    if(has_normals_)
      n_grid_[ix][iy][iz].push_back(ptset.n(i));
    if(has_scalars_)
      s_grid_[ix][iy][iz].push_back(ptset.sc(i));
  }
}

template <class Type>
bvgl_grid_index_3d<Type>::bvgl_grid_index_3d(unsigned nx, unsigned ny, unsigned nz,
                                             vgl_pointset_3d<Type> ptset, std::vector<Type> scalars, Type thresh):
  nx_(nx), ny_(ny), nz_(nz), has_normals_(ptset.has_normals()), npts_(0), thresh_(thresh), has_scalars_(true){
  p_grid_ = vbl_array_3d<std::vector<vgl_point_3d<Type> > >(nx, ny, nz);
  s_grid_ = vbl_array_3d<std::vector<Type> >(nx, ny, nz);
  idx_grid_ = vbl_array_3d<std::vector<size_t> >(nx, ny, nz);
  if(has_normals_)
    n_grid_ = vbl_array_3d<std::vector<vgl_vector_3d<Type> > >(nx, ny, nz);
  bbox_ = vgl_bounding_box(ptset);
  xmin_ = bbox_.min_x();
  ymin_ = bbox_.min_y();
  zmin_ = bbox_.min_z();
  Type dnx = static_cast<Type>(nx), dny = static_cast<Type>(ny), dnz = static_cast<Type>(nz);
  delta_x_ = bbox_.width()/dnx;
  delta_y_ = bbox_.height()/dny;
  delta_z_ = bbox_.depth()/dnz;
  unsigned ix =0, iy=0, iz =0;
  unsigned npts = ptset.npts();
  for(unsigned i = 0; i<npts; ++i){
    vgl_point_3d<Type> pi = ptset.p(i);
    this->index(pi, ix, iy, iz);
    p_grid_[ix][iy][iz].push_back(pi);
    idx_grid_[ix][iy][iz].push_back(i);
    npts_++;
    if(has_normals_)
      n_grid_[ix][iy][iz].push_back(ptset.n(i));
    s_grid_[ix][iy][iz].push_back(scalars[i]);
  }
}
//: the point contained in the grid closest to p or optionally to the normal plane passing through the closest point
template <class Type>
bool bvgl_grid_index_3d<Type>::closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc, Type& scalar) const{
  scalar = 0.0;
  if(!npts_)
    return false;
  unsigned ix =0, iy=0, iz =0;
  this->index(p, ix, iy, iz);
  unsigned ixs = ix, ixe = ix;
  unsigned iys = iy, iye = iy;
  unsigned izs = iz, ize = iz;
  if(ix>=1) ixs = ix-1;
  if(iy>=1) iys = iy-1;
  if(iz>=1) izs = iz-1;
  if(ix<(nx_-1)) ixe = ix+1;
  if(iy<(ny_-1)) iye = iy+1;
  if(iz<(nz_-1)) ize = iz+1;
  Type dmin = std::numeric_limits<Type>::max();
  unsigned ixc = 0, iyc = 0,  izc = 0, ipc = 0;
  bool found = false;
  for(unsigned i = ixs; i<=ixe; ++i)
    for(unsigned j = iys; j<=iye; ++j)
      for(unsigned k = izs; k<=ize; ++k){
        unsigned np = static_cast<unsigned>(p_grid_[i][j][k].size());
        if(!np) continue;
        for(unsigned ip = 0; ip<np; ++ip){
          vgl_point_3d<Type> pp = p_grid_[i][j][k][ip];
          Type d = (pp-p).length();
          if(d<dmin){
            dmin = d; ixc = i; iyc = j; izc = k; ipc = ip; found = true;
          }
        }
      }
  if(!found)
    return false;
  vgl_point_3d<Type> pmin = p_grid_[ixc][iyc][izc][ipc];
  if(!has_normals_){
    pc = pmin;
    if(this->has_scalars())
    scalar = s_grid_[ixc][iyc][izc][ipc];
    return true;
  }
  vgl_vector_3d<Type> nc = n_grid_[ixc][iyc][izc][ipc];
  vgl_plane_3d<Type> pl(nc, pmin);
  pc = vgl_closest_point(pl, p);
  if((pc-pmin).length()>thresh_)
    pc = pmin;
  if(this->has_scalars())
    scalar = s_grid_[ixc][iyc][izc][ipc];
  return true;
}
template <class Type>
bool bvgl_grid_index_3d<Type>::closest_point_as_index(vgl_point_3d<Type> const& p, size_t& pc_indx, Type& scalar) const{
  scalar = 0.0;
  if(!npts_)
    return false;
  unsigned ix =0, iy=0, iz =0;
  this->index(p, ix, iy, iz);
  unsigned ixs = ix, ixe = ix;
  unsigned iys = iy, iye = iy;
  unsigned izs = iz, ize = iz;
  if(ix>=1) ixs = ix-1;
  if(iy>=1) iys = iy-1;
  if(iz>=1) izs = iz-1;
  if(ix<(nx_-1)) ixe = ix+1;
  if(iy<(ny_-1)) iye = iy+1;
  if(iz<(nz_-1)) ize = iz+1;
  Type dmin = std::numeric_limits<Type>::max();
  unsigned ixc = 0, iyc = 0,  izc = 0, ipc = 0;
  bool found = false;
  for(unsigned i = ixs; i<=ixe; ++i)
    for(unsigned j = iys; j<=iye; ++j)
      for(unsigned k = izs; k<=ize; ++k){
        unsigned np = static_cast<unsigned>(p_grid_[i][j][k].size());
        if(!np) continue;
        for(unsigned ip = 0; ip<np; ++ip){
          vgl_point_3d<Type> pp = p_grid_[i][j][k][ip];
          Type d = (pp-p).length();
          if(d < Type(1.0e-10))
            continue; // same point
          if(d<dmin){
            dmin = d; ixc = i; iyc = j; izc = k; ipc = ip; found = true;
          }
        }
      }
  if(!found)
    return false;
  
  pc_indx = idx_grid_[ixc][iyc][izc][ipc];
  if(this->has_scalars())
    scalar = s_grid_[ixc][iyc][izc][ipc];
   
  return true;
}
template <class Type>
bool bvgl_grid_index_3d<Type>::k_nearest_pts_by_index(vgl_point_3d<Type> const& p, size_t k_max, std::vector<size_t> & pc_indices) const{
  if(!npts_)
    return false;
  unsigned ix =0, iy=0, iz =0;
  this->index(p, ix, iy, iz);
  unsigned ixs = ix, ixe = ix;
  unsigned iys = iy, iye = iy;
  unsigned izs = iz, ize = iz;
  if(ix>=1) ixs = ix-1;
  if(iy>=1) iys = iy-1;
  if(iz>=1) izs = iz-1;
  if(ix<(nx_-1)) ixe = ix+1;
  if(iy<(ny_-1)) iye = iy+1;
  if(iz<(nz_-1)) ize = iz+1;
  Type dmin = std::numeric_limits<Type>::max();
  bool found = false;
  std::vector<std::pair<size_t, Type> > indx_dist_pairs;
  for(unsigned i = ixs; i<=ixe; ++i)
    for(unsigned j = iys; j<=iye; ++j)
      for(unsigned k = izs; k<=ize; ++k){
        unsigned np = static_cast<unsigned>(p_grid_[i][j][k].size());
        for(unsigned ip = 0; ip<np; ++ip){
          vgl_point_3d<Type> pp = p_grid_[i][j][k][ip];
          Type d = (pp-p).length();
          if(d<dmin)
            found = true;
          size_t pt_indx = idx_grid_[i][j][k][ip];
          indx_dist_pairs.emplace_back(pt_indx, d);
        }
      }
  if(!found)
    return false;

  std::sort(indx_dist_pairs.begin(), indx_dist_pairs.end(), bvgl_grid_index_3d<Type>::dist_less);
  size_t nk = indx_dist_pairs.size();
  if(nk > k_max) nk = k_max;
  for(size_t k = 0; k<nk; ++k)
    pc_indices.push_back(indx_dist_pairs[k].first);

  return true;
}
template <class Type>
bool bvgl_grid_index_3d<Type>::closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc) const{
  Type scalar = Type(0);
  return this->closest_point(p, pc, scalar);
}
template <class Type>
bool bvgl_grid_index_3d<Type>::closest_point_as_index(vgl_point_3d<Type> const& p,  size_t& pc_idx) const{
  Type scalar = Type(0);
  return this->closest_point_as_index(p, pc_idx, scalar);
}
template <class Type>
Type bvgl_grid_index_3d<Type>::distance(vgl_point_3d<Type> const& p, Type& scalar) const{
  vgl_point_3d<Type> pc;
  bool good = this->closest_point(p, pc, scalar);
  if(!good)
    return std::numeric_limits<Type>::max();
  return (p-pc).length();
}

//: the distance from p to the closest point or optionally its normal plane
template <class Type>
Type bvgl_grid_index_3d<Type>::distance(vgl_point_3d<Type> const& p) const{
  Type scalar = Type(0);
  return this->distance(p, scalar);
}
template <class Type>
std::vector<vgl_pointset_3d<Type> > bvgl_grid_index_3d<Type>::intersecting_cells(vgl_box_3d<Type> const& box) const{
  std::vector<vgl_pointset_3d<Type> > ret;
  vgl_point_3d<Type> minp = box.min_point(), maxp = box.max_point();
  unsigned ix_min,  iy_min,  iz_min;
  unsigned ix_max,  iy_max,  iz_max;
  index(minp, ix_min, iy_min, iz_min);
  index(maxp, ix_max, iy_max, iz_max);
  unsigned ixs = ix_min, ixe = ix_max;
  unsigned iys = iy_min, iye = iy_max;
  unsigned izs = iz_min, ize = iz_max;
  unsigned temp;
  if(ixs>ixe){
    temp = ixs;
    ixs = ixe;
    ixe = temp;
  }
  if(iys>iye){
    temp = iys;
    iys = iye;
    iye = temp;
  }
  if(izs>ize){
    temp = izs;
    izs = ize;
    ize = temp;
  }
  for(unsigned iz = izs; iz<=ize; ++iz)
    for(unsigned iy = iys; iy<=iye; ++iy)
      for(unsigned ix = ixs; ix<=ixe; ++ix){
        const std::vector<vgl_point_3d<Type> >& pcell = p_grid_[ix][iy][iz];
        vgl_pointset_3d<Type> temp;
        size_t n = pcell.size();
        
          if (has_normals_ && has_scalars_) {
            const std::vector<vgl_vector_3d<Type> >& ncell = n_grid_[ix][iy][iz];
            const std::vector<Type>& scell = s_grid_[ix][iy][iz];
            for (size_t k = 0; k < n; ++k) 
            temp.add_point_with_normal_and_scalar(pcell[k], ncell[k], scell[k]);
          }
          else if (has_normals_ && !has_scalars_) {
            const std::vector<vgl_vector_3d<Type> >& ncell = n_grid_[ix][iy][iz];
            for (size_t k = 0; k < n; ++k)
              temp.add_point_with_normal(pcell[k], ncell[k]);
          }
          else if (!has_normals_ && has_scalars_) {
            const std::vector<Type>& scell = s_grid_[ix][iy][iz];
            for (size_t k = 0; k < n; ++k)
             temp.add_point_with_scalar(pcell[k], scell[k]);
          }
          else
            for (size_t k = 0; k < n; ++k)
             temp.add_point(pcell[k]);

        ret.push_back(temp);
      }//ix
  return ret;
}
template <class Type>
 vgl_pointset_3d<Type> bvgl_grid_index_3d<Type>::cell_centroids(size_t min_pts_in_cell){
  vgl_pointset_3d<Type> ret;
  for(size_t iz = 0; iz<nz_; ++iz)
    for(size_t iy = 0; iy<ny_; ++iy)
      for(size_t ix = 0; ix<nx_; ++ix){
        size_t np = p_grid_[ix][iy][iz].size();
        if(np < min_pts_in_cell)
          continue;
        vgl_point_3d<Type> c(Type(0), Type(0), Type(0));
        for(size_t i = 0; i<np; ++i){
          const vgl_point_3d<Type>& p = p_grid_[ix][iy][iz][i];
          c.set(c.x()+p.x(), c.y()+p.y(), c.z()+p.z());
        }
        c.set(c.x()/np, c.y()/np, c.z()/np);
        ret.add_point(c);
      }
  return ret;
}
template <class Type>
bool near_line_seg(vgl_point_3d<Type> const& p0, vgl_point_3d<Type> const& p1, vgl_point_3d<Type> q, Type tol){
  vgl_line_3d_2_points<Type> l2p(p0, p1);
  vgl_point_3d<Type> cp = vgl_closest_point(l2p, q);
  Type d0 = vgl_distance(p0,cp);
  Type d1 = vgl_distance(p1,cp);
  Type d01 = vgl_distance(p0,p1);
  //is point interior to segment?, i.e. d0 + d1 = d10
  if(fabs(d0+d1-d01)> Type(1.0e-6))
    return false;
  Type dq = vgl_distance(cp, q);
  return dq < tol;
}

template <class Type>
void bvgl_grid_index_3d<Type>::directional_neighbors(vgl_point_3d<Type> const& p, vgl_vector_3d<Type> const& dir, Type radius, vnl_vector<size_t>& indices) const{
  // insure unit vector
  vgl_vector_3d<Type> unit_dir = dir;
  unit_dir /= unit_dir.length();
  // get cell tolerance
  vgl_vector_3d<Type> unit_x(Type(1), Type(0), Type(0));
  vgl_vector_3d<Type> unit_y(Type(0), Type(1), Type(0));
  vgl_vector_3d<Type> unit_z(Type(0), Type(0), Type(1));
  Type dpx = fabs(dot_product(unit_dir, unit_x));
  Type dpy = fabs(dot_product(unit_dir, unit_y));
  Type dpz = fabs(dot_product(unit_dir, unit_z));
  Type tol = Type(0);
  if(dpx>dpy){
    if(dpx>dpz)
      tol = std::min(delta_y_, delta_z_); // x dir
    else if(dpz > dpy)
      tol = std::min(delta_x_, delta_y_); // z dir
  }else if(dpy > dpz){
    tol = std::min(delta_x_, delta_z_); // y dir
  }else{
    tol = std::min(delta_x_, delta_y_); // z dir
  }

  // construct box around vector with radius
  vgl_point_3d<Type> minp = p - radius*unit_dir;
  vgl_point_3d<Type> maxp = p + radius*unit_dir;
  unsigned ix_min,  iy_min,  iz_min;
  unsigned ix_max,  iy_max,  iz_max;
  index(minp, ix_min, iy_min, iz_min);
  index(maxp, ix_max, iy_max, iz_max);
  unsigned ixs = ix_min, ixe = ix_max;
  unsigned iys = iy_min, iye = iy_max;
  unsigned izs = iz_min, ize = iz_max;
  unsigned temp;
  if(ixs>ixe){
    temp = ixs;
    ixs = ixe;
    ixe = temp;
  }
  if(iys>iye){
    temp = iys;
    iys = iye;
    iye = temp;
  }
  if(izs>ize){
    temp = izs;
    izs = ize;
    ize = temp;
  }
  std::vector<size_t> tind;
  for(unsigned iz = izs; iz<=ize; ++iz)
    for(unsigned iy = iys; iy<=iye; ++iy)
      for(unsigned ix = ixs; ix<=ixe; ++ix){
        const std::vector<vgl_point_3d<Type> >& pcell = p_grid_[ix][iy][iz];
        const std::vector<size_t>& icell = idx_grid_[ix][iy][iz];
        size_t nc = pcell.size();
        if(nc == 0)
          continue;
        for(size_t i = 0; i<nc; ++i){
          vgl_point_3d<Type> q = pcell[i];
          Type d = (p-q).length();
          if(d<Type(1.0e-6))
            continue; //don't return query point
          if(near_line_seg(minp, maxp, q, tol))
            tind.push_back(icell[i]);
        }
      }
  indices.set_size(tind.size());
  for(size_t t = 0; t<tind.size(); ++t)
    indices[t]=tind[t];
}

template <class Type>
vgl_vector_3d<Type> bvgl_grid_index_3d<Type>::cell_direction(vgl_point_3d<Type> const& p) const{
  unsigned ix, iy, iz;
  this->index(p, ix, iy, iz);
  const std::vector<vgl_vector_3d<Type> >& ncell = n_grid_[ix][iy][iz];
  size_t nn = ncell.size();
  vgl_vector_3d<Type> avg_dir(Type(0),Type(0),Type(0));
  for(size_t i = 0; i<nn; ++i)
    avg_dir += ncell[i];
  avg_dir /= nn;
  return avg_dir;
}

#endif// bvgl_grid_index_3d
