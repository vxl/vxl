#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_3d.hxx>
#include <boxm/boxm_block.h>
#include <boct/boct_tree.h>
#include <vnl/vnl_vector_fixed.h>

typedef boct_tree<short,vnl_vector_fixed<float,3> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
