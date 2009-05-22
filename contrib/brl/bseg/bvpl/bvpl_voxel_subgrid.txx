#include "bvpl_voxel_subgrid.h"

template <class T>
bvpl_voxel_subgrid<T>::bvpl_voxel_subgrid(bvxm_voxel_grid_base_sptr grid, 
                                          vgl_point_3d<unsigned> center, 
                                          vgl_vector_3d<double> dimensions)
 : bvpl_subgrid_base(center, dimensions)
 
 {
   box_(center, dim_.x(), dim_.x(), dim_.x(), point_type::centre);
   iter_ = grid_.slab_iterator(box.min_z(), dim_.z());
   slab_ = *iter_;
 }
 
template <class T>
bvpl_voxel_subgrid<T>::bvpl_voxel_subgrid(bvxm_voxel_slab<T>& slab, 
                                          vgl_point_3d<unsigned> center, 
                                          vgl_vector_3d<double> dimensions)
 : bvpl_subgrid_base(center, dimensions), slab_(slab);
 
 {
   box_(center, dim_.x(), dim_.x(), dim_.x(), point_type::centre);

 }
 
 template <class T>
 T& bvpl_voxel_subgrid<T>::voxel(unsigned x, unsigned y, unsigned z)
 {
   unsigned x_min = box.min_x();
   unsigned y_min = box.min_y(); 
   unsigned z_max = box.max_z(); 
   
   T& v = iter_(x+x_min, y+y_min, z-m_mx);
   return v;
 }
 
 