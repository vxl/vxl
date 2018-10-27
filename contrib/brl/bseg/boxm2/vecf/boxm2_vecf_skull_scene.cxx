#include "boxm2_vecf_skull_scene.h"
#include <vul/vul_file.h>
boxm2_vecf_skull_scene::boxm2_vecf_skull_scene( std::string const& base_scene_path, std::string const& mandible_geo_path)
  : boxm2_vecf_articulated_scene(), target_data_extracted_(false)
{
  //resolve the mandible and cranium paths
  std::string mandible_path = base_scene_path + "mandible/mandible.xml";
  std::string cranium_path = base_scene_path + "cranium/cranium.xml";
  mandible_scene_ = new boxm2_vecf_mandible_scene(mandible_path, mandible_geo_path);
  cranium_scene_ = new boxm2_vecf_cranium_scene(cranium_path);
}

bool boxm2_vecf_skull_scene::set_params(boxm2_vecf_articulated_params const& params)
{
  try{
    auto const& params_ref = dynamic_cast<boxm2_vecf_skull_params const &>(params);
    params_ =boxm2_vecf_skull_params(params_ref);
  }catch(std::exception e){
    std::cout<<" Can't downcast to skull parameters! PARAMATER ASSIGNMENT PHAILED!"<<std::endl;
    return false;
  }

  mandible_scene_->set_params( params_.mandible_params_);
  return true;
}


void boxm2_vecf_skull_scene::map_to_target(boxm2_scene_sptr target)
{

  // deal with mapping entire skull later -- FIXME !!
  // map components
  mandible_scene_->map_to_target(target);
  cranium_scene_->map_to_target(target);

}
