#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_block.txx>
#include <boxm/boxm_scene.txx>
#include <vbl/vbl_array_3d.txx>
#include <vcl_string.h>

typedef boct_tree<short, bvpl_octree_sample<float> > tree_type;

BOXM_BLOCK_INSTANTIATE(tree_type);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
BOXM_SCENE_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);

