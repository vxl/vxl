#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <boxm/boxm_block.txx>
#include <boct/boct_tree.txx>
#include <boxm/opt/boxm_opt_rt_sample.h>

typedef boct_tree<short, boxm_opt_rt_sample<float> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
