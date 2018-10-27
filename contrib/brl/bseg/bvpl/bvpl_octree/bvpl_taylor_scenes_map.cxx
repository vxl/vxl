//:
// \file
// \author Isabel Restrepo
// \date 15-Feb-2011

#include "bvpl_taylor_scenes_map.h"
#include <bvpl/bvpl_octree/sample/bvpl_taylor_basis_sample.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>

#include <vul/vul_file.h>

bvpl_taylor_scenes_map::bvpl_taylor_scenes_map(bvpl_taylor_basis_loader loader)
{
  loader_ = loader;
  typedef boct_tree<short,float> tree_type;

  std::vector<std::string> kernel_names;
  loader.files(kernel_names);

  //Load scenes one-by-one an insert them in the map
  for (auto & kernel_name : kernel_names)
  {
    std::string scene_in_file = loader.path() + '/' + kernel_name + "/float_response_scene.xml";
    boxm_scene_parser parser;
    boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
    scene_ptr->load_scene(scene_in_file, parser);

    if (scene_ptr->appearence_model() != BOXM_FLOAT) {
      std::cerr << " bvpl_taylor_scenes, scenes must be of type float\n";
    }

    auto* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;

    this->scenes_.insert(std::pair<std::string, boxm_scene_base_sptr>( kernel_name, scene_ptr));
  }

  // Create a scene to later save the taylor reconstruction (parameters of the  scene are the same as those of the input scene)
  auto* scene_in = static_cast<boxm_scene<tree_type>* > (scenes_[kernel_names[0]].as_pointer());
  {
    boxm_scene<tree_type> *scene_out =
    new boxm_scene<tree_type>(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    scene_out->set_paths(loader.path(), "error");
    scene_out->set_appearance_model(BOXM_FLOAT);
    if (!vul_file::exists(loader.path() + "/error_scene.xml"))
      scene_out->write_scene("error_scene.xml");
    this->scenes_.insert(std::pair<std::string, boxm_scene_base_sptr>( "error", scene_out));
  }

  {
    boxm_scene<boct_tree<short, bvpl_taylor_basis2_sample> > *scene_out =
    new boxm_scene<boct_tree<short, bvpl_taylor_basis2_sample> >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim(), scene_in->max_level(), scene_in->init_level());
    scene_out->set_paths(loader.path(), "basis");
    scene_out->set_appearance_model(BOXM_FLOAT);
    if (!vul_file::exists(loader.path() + "/basis_scene.xml"))
      scene_out->write_scene("basis_scene.xml");
    this->scenes_.insert(std::pair<std::string, boxm_scene_base_sptr>( "basis", scene_out));
  }
}
