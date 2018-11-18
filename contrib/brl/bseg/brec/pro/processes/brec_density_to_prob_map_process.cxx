// This is brl/bseg/brec/pro/processes/brec_density_to_prob_map_process.cxx

//:
// \file
// \brief A process to convert a prob density image into a probability map, output will be of type float with values in [0,1]
//
//  CAUTION: Input image is assumed to have type float (not necessarily with values in [0,1])
//
// \author Ozge Can Ozcanli
// \date September 01, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <brec/brec_bg_pair_density.h>

//: Constructor
bool brec_density_to_prob_map_process_cons(bprb_func_process& pro)
{
   //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input density map
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");      // output prob map
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  pro.parameters()->add("top percentile in density map", "tp", 0.1f);
  pro.parameters()->add("value to correspond to top percentile in prob map", "pv", 0.9f);

  return true;
}


bool brec_density_to_prob_map_process(bprb_func_process& pro)
{
// Sanity check
  if (pro.n_inputs() < 1){
    std::cerr << "brec_density_to_prob_map_process - invalid inputs\n";
    return false;
  }

  //get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr density_map = pro.get_input<vil_image_view_base_sptr>(i++);
  if (density_map->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;

  float tp = 0.1f, pv = 0.9f;
  pro.parameters()->get_value("tp", tp);
  pro.parameters()->get_value("pv", pv);

  vil_image_view<float> density_map_f(density_map);

  vil_image_view<float> prob_map(density_map->ni(), density_map->nj());
  brec_bg_pair_density::initialize_bg_map(density_map_f, prob_map, pv, tp);

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(prob_map));

  return true;
}
