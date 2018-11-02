#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_3d.hxx>
#include <vgl/io/vgl_io_point_3d.h>
#include <boxm/boxm_block.hxx>
#include <boct/boct_tree.hxx>

typedef boct_tree<short, vgl_point_3d<double> > tree_type;
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type>*);
