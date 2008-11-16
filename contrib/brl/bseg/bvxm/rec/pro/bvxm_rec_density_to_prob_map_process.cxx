#include "bvxm_rec_density_to_prob_map_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <rec/bvxm_bg_pair_density.h>

bvxm_rec_density_to_prob_map_process::bvxm_rec_density_to_prob_map_process()
{
  //inputs
  input_data_.resize(1,brdb_value_sptr(0));
  input_types_.resize(1);
  input_types_[0] = "vil_image_view_base_sptr";      // input density map
  
  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "vil_image_view_base_sptr";      // output prob map

  parameters()->add("top percentile in density map", "tp", 0.1f);
  parameters()->add("value to correspond to top percentile in prob map", "pv", 0.9f);
}


bool bvxm_rec_density_to_prob_map_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  //get the inputs
  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr density_map = input0->value();

  if (density_map->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  float tp = 0.1f, pv = 0.9f;
  parameters()->get_value("tp", tp);
  parameters()->get_value("pv", pv);

  vil_image_view<float> density_map_f(density_map);
  
  vil_image_view<float> prob_map(density_map->ni(), density_map->nj());
  bvxm_bg_pair_density::initialize_bg_map(density_map_f, prob_map, pv, tp);
  
  brdb_value_sptr output = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(prob_map));
  output_data_[0] = output;

  return true;
}

