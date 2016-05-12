#include <bvxm/grid/bvxm_voxel_slab.hxx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf2.h>

typedef bsta_num_obs<bsta_gauss_sf2> gauss_type;
BVXM_VOXEL_SLAB_INSTANTIATE(gauss_type);
