#include <boxm/boxm_block.txx>
#include <boct/boct_tree.h>

typedef boct_tree<short, float> tree;
BOXM_BLOCK_INSTANTIATE(tree);
#include <boxm/boxm_scene.txx>
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree);
