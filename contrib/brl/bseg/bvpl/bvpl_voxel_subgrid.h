#ifndef bvpl_voxel_subgrid_h
#define bvpl_voxel_subgrid_h

#include <bvxm/bvxm_voxel_slab.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_vector_3d.h>

class bvpl_subgrid_base
{
 public:
  bvpl_subgrid_base(vgl_point_3d<int> center, vgl_vector_3d<int> dimensions)
    : center_(center), dim_(dimensions) {}

  virtual ~bvpl_subgrid_base(){}

  vgl_point_3d<int> center() { return center_; }

 protected:
   vgl_point_3d<int> center_;
   vgl_vector_3d<int> dim_;
};

template <class T>
class bvpl_voxel_subgrid : public bvpl_subgrid_base
{
 public:
  bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab,
                     vgl_point_3d<int> center,
                     vgl_vector_3d<int> dimensions);

  virtual ~bvpl_voxel_subgrid(){}

  bool voxel(int x, int y, int z, T& v);
  bool voxel(vgl_point_3d<int> p, T& v) { return voxel(p.x(), p.y(), p.z(), v); }
  void set_voxel(const T& v);
 protected:
  vgl_box_3d<int> box_;
  bvxm_voxel_slab<T> slab_;
};

#endif
