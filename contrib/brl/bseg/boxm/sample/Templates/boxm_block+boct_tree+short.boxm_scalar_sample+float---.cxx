#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_scalar_sample.h>

typedef boct_tree<short,boxm_scalar_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);

#include <boxm/boxm_scene.hxx>
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree);
