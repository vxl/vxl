#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <boxm/boxm_block.hxx>
#include <vbl/vbl_array_3d.hxx>
#include <boct/boct_tree.hxx>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
typedef boct_tree<short,gauss_type> tree_type;

VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);
