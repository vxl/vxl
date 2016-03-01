#include <bvpl/bvpl_voxel_subgrid.hxx>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_attributes.h>

BVPL_VOXEL_SUBGRID_INSTANTIATE(bsta_gauss_sf1);
typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
BVPL_VOXEL_SUBGRID_INSTANTIATE(gauss_type);
