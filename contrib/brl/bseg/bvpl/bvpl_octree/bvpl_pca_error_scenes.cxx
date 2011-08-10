//:
// \file
// \author Isabel Restrepo
// \date 18-Feb-2011

#include "bvpl_pca_error_scenes.h"

#include <boxm/boxm_scene.h>

#include <vul/vul_file.h>
#include <vcl_sstream.h>

bvpl_pca_error_scenes::bvpl_pca_error_scenes(boxm_scene_base_sptr data_scene_base, vcl_string pca_path, unsigned dim)
{
  typedef boct_tree<short,float> tree_type;
  for (unsigned i =0; i<dim+1; i++)
    scenes_.push_back(new boxm_scene_base());

  boxm_scene<tree_type>* data_scene= dynamic_cast<boxm_scene<tree_type>* > (data_scene_base.as_pointer());
  if (!data_scene)
    vcl_cerr << "In bvpl_pca_error_scenes: Null Scene\n";

  for (unsigned i = 0; i<dim+1; i++)
  {
    //path to error scenes
    vcl_stringstream error_path_ss;
    error_path_ss << pca_path << "/error_" << i;
    vcl_string error_path = error_path_ss.str();
    if (!vul_file::is_directory(error_path))
      vul_file::make_directory(error_path);

    boxm_scene<tree_type> *error_scene =
    new boxm_scene<tree_type>(data_scene->lvcs(), data_scene->origin(), data_scene->block_dim(), data_scene->world_dim(), data_scene->max_level(), data_scene->init_level());
    error_scene->set_appearance_model(BOXM_FLOAT);

    vcl_string scene_name = "error_scene";
    error_scene->set_paths(error_path, scene_name);

    if (!vul_file::exists(error_path + "/" + scene_name +  ".xml"))
      error_scene->write_scene("/" +scene_name +  ".xml");

    //data_scene->clone_blocks(*error_scene, -1.0);

    scenes_[i] = error_scene;
  }
}
