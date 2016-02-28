#include <bvxm/grid/bvxm_voxel_storage_disk_cached.hxx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_von_mises.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_von_mises.h>

typedef bsta_vsum_num_obs<bsta_von_mises<float,3> > gauss_type;
BVXM_VOXEL_STORAGE_DISK_CACHED_INSTANTIATE(gauss_type);
