#include <bvxm/grid/bvxm_voxel_slab_iterator.hxx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sd3.h>
#include <bsta/io/bsta_io_attributes.h>

typedef bsta_vsum_num_obs<bsta_gauss_sd3> dist_type;
BVXM_VOXEL_SLAB_ITERATOR_INSTANTIATE(dist_type);
