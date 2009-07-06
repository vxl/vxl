#include <bvxm/grid/bvxm_voxel_slab.txx>
#include <bvxm/grid/bvxm_voxel_slab_iterator.txx>
#include <vnl/vnl_vector_fixed.h>

typedef vnl_vector_fixed<float,4> vnl_vector_fixed_f4;

BVXM_VOXEL_SLAB_INSTANTIATE(vnl_vector_fixed_f4);
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(vnl_vector_fixed_f4);
