#include <vcl_string.h>
#include <vbl/vbl_array_3d.txx>
#include <vgl/io/vgl_io_point_3d.h>
#include <boxm/boxm_block.txx>
#include <boct/boct_tree.txx>

typedef boct_tree<short, vgl_point_3d<double> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
