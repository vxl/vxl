#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <vgl/io/vgl_io_point_3d.h>
#include <boxm/boxm_block.txx>
#include <boxm/sample/boxm_opt3_sample.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_apm_traits.h>

typedef boct_tree<short, boxm_opt3_sample<BOXM_APM_MOG_GREY> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
