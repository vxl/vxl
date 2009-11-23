#include <bvxm/grid/bvxm_voxel_slab.txx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_sphere.h>

typedef bsta_num_obs<bsta_gaussian_sphere<double, 2> > gauss_type;
BVXM_VOXEL_SLAB_INSTANTIATE(gauss_type);
