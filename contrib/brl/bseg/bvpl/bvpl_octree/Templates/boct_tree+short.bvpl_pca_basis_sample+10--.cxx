#include <bvpl/bvpl_octree/sample/bvpl_pca_basis_sample.h>
#include <boct/boct_tree.txx>
#include <boct/boct_tree_cell.txx>

BOCT_TREE_INSTANTIATE(short, bvpl_pca_basis_sample<10> );
BOCT_TREE_CELL_INSTANTIATE(short, bvpl_pca_basis_sample<10> );
