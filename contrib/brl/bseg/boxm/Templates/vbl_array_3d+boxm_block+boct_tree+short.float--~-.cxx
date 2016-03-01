#include <vcl_string.h>
#include <vbl/vbl_array_3d.hxx>
#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.hxx>

typedef boct_tree<short,float> tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
