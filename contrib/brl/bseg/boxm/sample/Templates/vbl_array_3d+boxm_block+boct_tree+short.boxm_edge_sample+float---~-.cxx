#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_3d.hxx>
#include <vgl/io/vgl_io_point_3d.h>
#include <boxm/boxm_block.hxx>
#include <boxm/sample/boxm_edge_sample.h>
#include <boct/boct_tree.h>

typedef boct_tree<short, boxm_edge_sample<float> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);

typedef boct_tree<short, boxm_aux_edge_sample<float> > aux_tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<aux_tree_type>*);
