#include <boxm2/boxm_block.txx>
#include <boct/boct_tree.h>
#include <boxm2/sample/boxm_opt2_sample.h>

typedef boct_tree<short,boxm_opt2_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);
#include <boxm2/boxm_scene.txx>
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree);
