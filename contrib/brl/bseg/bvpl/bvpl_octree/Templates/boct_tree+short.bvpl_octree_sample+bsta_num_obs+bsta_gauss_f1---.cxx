#include <bvpl/bvpl_octree/bvpl_octree_sample.h>
#include <boct/boct_tree.txx>
#include <boct/boct_tree_cell.txx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_f1.h>

typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
BOCT_TREE_INSTANTIATE(short, bvpl_octree_sample<gauss_type>);
BOCT_TREE_CELL_INSTANTIATE(short, bvpl_octree_sample<gauss_type>);
