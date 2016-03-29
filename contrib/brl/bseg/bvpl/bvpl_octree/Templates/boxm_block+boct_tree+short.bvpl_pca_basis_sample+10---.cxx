#include <bvpl/bvpl_octree/sample/bvpl_pca_basis_sample.h>
#include <boxm/boxm_block.hxx>
#include <boxm/boxm_scene.hxx>
#include <vbl/vbl_array_3d.hxx>

typedef boct_tree<short, bvpl_pca_basis_sample<10> > tree_type;

BOXM_BLOCK_INSTANTIATE(tree_type);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
BOXM_SCENE_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);
