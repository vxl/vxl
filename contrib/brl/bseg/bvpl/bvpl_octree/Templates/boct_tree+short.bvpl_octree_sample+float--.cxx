#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <boct/boct_tree.hxx>
#include <boct/boct_tree_cell.hxx>

BOCT_TREE_INSTANTIATE(short, bvpl_octree_sample<float>);
BOCT_TREE_CELL_INSTANTIATE(short, bvpl_octree_sample<float>);
