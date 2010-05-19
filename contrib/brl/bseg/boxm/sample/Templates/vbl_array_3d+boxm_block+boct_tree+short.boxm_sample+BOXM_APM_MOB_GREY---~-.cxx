#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_block.txx>
#include <boct/boct_tree.txx>
#include <boxm/sample/boxm_sample.h>

typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
