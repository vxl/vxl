#include <boct/boct_tree.h>
#include <boxm/boxm_block.txx>
#include <boxm/boxm_scene.txx>
#include <bvpl/bvpl_octree/sample/bvpl_taylor_basis_sample.h>
#include <vbl/vbl_array_3d.txx>

typedef boct_tree<short, bvpl_taylor_basis2_sample> tree_type;

BOXM_BLOCK_INSTANTIATE(tree_type);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
BOXM_SCENE_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);




