#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.h>
#include <boxm/sample/boxm_inf_line_sample.h>

typedef boct_tree<short,boxm_inf_line_sample<float> > tree;
BOXM_BLOCK_INSTANTIATE(tree);
