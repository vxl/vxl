#include <bvxm/grid/bvxm_voxel_slab.hxx>
#include <bvxm/grid/bvxm_voxel_slab_iterator.hxx>
#include <vnl/vnl_vector_fixed.h>

typedef vnl_vector_fixed<float,3> vnl_vector_fixed_f3;

BVXM_VOXEL_SLAB_INSTANTIATE(vnl_vector_fixed_f3);
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(vnl_vector_fixed_f3);
