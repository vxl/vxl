#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <boxm/boxm_scene.hxx>

typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
typedef boct_tree<short,gauss_type> tree_type;

BOXM_SCENE_INSTANTIATE(tree_type);
