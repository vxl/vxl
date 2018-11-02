#include <iostream>
#include <string>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <boct/boct_tree.h>
#include <boxm/boxm_block.hxx>
#include <boxm/boxm_scene.hxx>
#include <vbl/vbl_array_3d.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
typedef boct_tree<short, bvpl_octree_sample< gauss_type> > tree_type;

BOXM_BLOCK_INSTANTIATE(tree_type);
BOXM_BLOCK_ITERATOR_INSTANTIATE(tree_type);
BOXM_SCENE_INSTANTIATE(tree_type);
VBL_ARRAY_3D_INSTANTIATE(boxm_block<tree_type> *);
