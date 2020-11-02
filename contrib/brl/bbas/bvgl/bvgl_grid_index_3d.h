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

  //: geometric queries
  //: the point contained in the grid closest to p or optionally to the normal plane passing through the closest point
  bool closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc) const;
  bool closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc, Type& scalar) const;
  //: the distance from p to the closest point or optionally its normal plane
  Type distance(vgl_point_3d<Type> const& p) const;
  Type distance(vgl_point_3d<Type> const& p, Type& scalar) const;

  std::vector<vgl_pointset_3d<Type> > intersecting_cells(vgl_box_3d<Type> const& box) const;
  //: accessors

  vgl_box_3d<Type> bounding_box() const {return bbox_;}
  unsigned npts() const {return npts_;}
  bool has_normals() const {return has_normals_;}
  bool has_scalars() const {return has_scalars_;}
 protected:
  bool has_scalars_;
  bool has_normals_;
  vbl_array_3d<std::vector<vgl_point_3d<Type > > >   p_grid_;
  vbl_array_3d<std::vector<vgl_vector_3d<Type > > >  n_grid_;//optional normal grid
  vbl_array_3d<std::vector<Type> >  s_grid_;//optional scalar grid
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
bool bvgl_grid_index_3d<Type>::closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& pc) const{
  Type scalar = Type(0);
  return this->closest_point(p, pc, scalar);
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
#endif// bvgl_grid_index_3d
