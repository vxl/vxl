#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.h>

typedef boct_tree<short,int>  tree_type;
BOXM_BLOCK_INSTANTIATE(tree_type);

typedef boct_tree<short,short int>  short_tree_type;
BOXM_BLOCK_INSTANTIATE(short_tree_type);
