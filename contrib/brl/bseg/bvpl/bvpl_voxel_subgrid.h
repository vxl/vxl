#ifndef bvpl_voxel_subgrid_h
#define bvpl_voxel_subgrid_h
//:
// \file
// \brief A class to retrieve a subgrid within a bigger grid
// \author Gamze Tunali
// \date  June 24, 2009
//
//  \verbatim
//                                 MaxPosition
//                       |<--width-->|
//                       O-----------O  ---
//                      /           /|   ^
//                     /           / |   |
//                    O-----------O  | height
//                    |       o   |  |   |
//                    |  centroid |  |   v
//                    |   (0,0,0) |  O  ---
//     Y              |           | /   /_____depth
//     |   Z          |           |/   /
//     |  /           O-----------O  ---
//     | /         MinPosition
//     O-----X
// \endverbatim
//
// \verbatim
//  Modifications
//   9/30/09 Isabel Restrepo: Changed coordinate system to match that of kernels.
//           Within a subgrid the centroid is placed at the origin. The maximum
//           and minimum position are as shown below. The methods in this class must take care
//           of retrieving the appropriate index of the big grid (where z is inverted).
// \endverbatim

#include <bvxm/grid/bvxm_voxel_slab.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

class bvpl_subgrid_base
{
 public:
  bvpl_subgrid_base(vgl_point_3d<int> center)
    : center_(center) {}

  virtual ~bvpl_subgrid_base() = default;

  vgl_point_3d<int> center() const { return center_; }

 protected:
  vgl_point_3d<int> center_;
};

template <class T>
class bvpl_voxel_subgrid : public bvpl_subgrid_base
{
 public:
  bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab,
                     vgl_point_3d<int> center,
                     vgl_point_3d<int> max_point,
                     vgl_point_3d<int> min_point);

  ~bvpl_voxel_subgrid() override = default;

  bool voxel(int x, int y, int z, T& v);
  bool voxel(vgl_point_3d<int> p, T& v) { return voxel(p.x(), p.y(), p.z(), v); }
  void set_voxel(const T& v);
  void set_voxel_at(vgl_point_3d<int> p, T& v) { return set_voxel_at(p.x(), p.y(), p.z(), v); }
  void set_voxel_at(int x, int y, int z, T const &v);
  T get_voxel();

 protected:
  vgl_box_3d<int> box_;
  bvxm_voxel_slab<T> slab_;
};

#endif
