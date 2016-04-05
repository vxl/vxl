#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.hxx>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_gaussian_sphere.h>
#include <boct/boct_tree.hxx>
#include <boct/boct_tree_cell.hxx>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

BOCT_TREE_INSTANTIATE(short, bvpl_octree_sample<gauss_type>);
BOCT_TREE_CELL_INSTANTIATE(short, bvpl_octree_sample<gauss_type>);
