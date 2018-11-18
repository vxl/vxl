#include <iostream>
#include <string>
#include <boct/boct_tree.h>
#include <boxm/boxm_block.hxx>
#include <boxm/boxm_scene.hxx>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_3d.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

typedef boct_tree<short, vnl_vector_fixed<double,10> > tree_type;

BOXM_BLOCK_INSTANTIATE(tree_type);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
BOXM_SCENE_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);
