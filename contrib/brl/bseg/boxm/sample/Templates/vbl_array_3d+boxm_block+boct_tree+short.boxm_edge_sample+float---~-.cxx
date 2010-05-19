#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <vgl/io/vgl_io_point_3d.h>
#include <boxm/boxm_block.txx>
#include <boxm/sample/boxm_edge_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short, boxm_edge_sample<float> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);

typedef boct_tree<short, boxm_aux_edge_sample<float> > aux_tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<aux_tree_type>*);
