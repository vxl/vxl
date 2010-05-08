#include <boxm2/boxm_block.txx>
#include <boct/boct_tree.h>
#include <boxm2/sample/boxm_scalar_sample.h>

typedef boct_tree<short,boxm_scalar_sample<float> > tree;
BOXM2_BLOCK_INSTANTIATE(tree);

#include <boxm2/boxm_scene.txx>
BOXM2_BLOCK_ITERATOR_INSTANTIATE(tree);
