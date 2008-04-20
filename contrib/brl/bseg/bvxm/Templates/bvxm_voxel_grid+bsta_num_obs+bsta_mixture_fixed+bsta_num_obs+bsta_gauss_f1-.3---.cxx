#include "bvxm_voxel_grid.txx"
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_mixture.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>

typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
typedef bsta_num_obs<bsta_mixture_fixed<gauss_type, 3> > mix_gauss_type;
BVXM_VOXEL_GRID_INSTANTIATE(mix_gauss_type);
