#ifndef bvpl_voxel_subgrid_hxx_
#define bvpl_voxel_subgrid_hxx_

#include <iostream>
#include "bvpl_voxel_subgrid.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bvpl_voxel_subgrid<T>::bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab,
                                          vgl_point_3d<int> center,
                                          vgl_point_3d<int> min_point,
                                          vgl_point_3d<int> max_point)
: bvpl_subgrid_base(center), slab_(slab)
{
  vgl_point_3d<int> min_pt(center.x() + min_point.x(), center.y()+min_point.y(), center.z()-max_point.z());
  vgl_point_3d<int> max_pt(center.x() + max_point.x(), center.y()+max_point.y(), center.z()-min_point.z());
  box_=(vgl_box_3d<int>(min_pt, max_pt));
}

template <class T>
bool bvpl_voxel_subgrid<T>::voxel(int x, int y, int z, T& v)
{
  vgl_point_3d<int> c = center_;

  //are the z coordinates of bvxm_voxel grid are
  x = c.x()+x;
  y = c.y()+y;
  z = c.z()-z;

  vgl_box_3d<int> slab_box(vgl_point_3d<int>(0,0,0), vgl_point_3d<int>(slab_.nx()-1,slab_.ny()-1,slab_.nz()-1));
  // make sure that the point is inside the box
  if (!box_.contains(x,y,z)){
#ifdef DEBUG
    std::cerr << "bvpl_subgrid_voxel_iterator: The index is out of subgrid boundaries!\n";
#endif
    return false;
  }
  else if (!slab_box.contains(x,y,z)) {
#ifdef DEBUG
    std::cerr << "bvpl_subgrid_voxel_iterator: The index is out of grid boundaries!\n";
#endif
    return false;
  }
  else {
#if 0
    std::cout << "Slab idx " << x << ' ' << y << ' ' << z << std::endl;
#endif
    v = slab_(x,y,z);
    return true;
  }
}

template <class T>
void bvpl_voxel_subgrid<T>::set_voxel_at(int x, int y, int z, T const &v)
{
  vgl_point_3d<int> c = center_;

  //are the z coordinates of bvxm_voxel grid are
  x = c.x()+x;
  y = c.y()+y;
  z = c.z()-z;

  vgl_box_3d<int> slab_box(vgl_point_3d<int>(0,0,0), vgl_point_3d<int>(slab_.nx()-1,slab_.ny()-1,slab_.nz()-1));
  // make sure that the point is inside the box
  if (!box_.contains(x,y,z)){
#ifdef DEBUG
    std::cerr << "bvpl_subgrid_voxel_iterator: The index is out of subgrid boundaries!\n";
#endif
    return;
  }
  else if (!slab_box.contains(x,y,z)) {
#ifdef DEBUG
    std::cerr << "bvpl_subgrid_voxel_iterator: The index is out of grid boundaries!\n";
#endif
    return;
  }
  else {
    //std::cout << "Slab idx " << x <<" " << y  <<" "<< z << std::endl;
    slab_(x,y,z)= v;
    return;
  }
}

template <class T>
void bvpl_voxel_subgrid<T>::set_voxel(const T& v)
{
  vgl_point_3d<int> c = center_;

 //  std::cout << "Setting " << c << ' ' << v << std::endl;

  T& val = slab_(c.x(), c.y(), c.z());
  val = v;
}

template <class T>
T bvpl_voxel_subgrid<T>::get_voxel()
{
  vgl_point_3d<int> c = center_;
  return slab_(c.x(), c.y(), c.z());
}

#define BVPL_VOXEL_SUBGRID_INSTANTIATE(T) \
template class bvpl_voxel_subgrid<T >

#endif // bvpl_voxel_subgrid_hxx_
