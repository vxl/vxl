#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <boxm/boxm_block.txx>
#include <boct/boct_tree.txx>

typedef boct_tree<short, int > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);

typedef boct_tree<short, short > short_tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<short_tree_type>*);
