// This is brl/bseg/brec/pro/processes/brec_bayesian_update_process.cxx

//:
// \file
// \brief A class to apply Bayesian rule to find posterior probability of an entity given a measurement map based on background and foreground models of the given entity
//
// e.g. given a map P(Entity) and a measurement map: M.
// and given models: P(Measurement | Entity) and P(Measurement | not Entity)
// return P(Entity | Measurement)
// for instance: given expected area difference map (measurement) for "foreground glitches on background" (entity)
//               return glitch detection map
//
// \author Ozge Can Ozcanli
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/vil_convert.h>
#include <vil/io/vil_io_image_view.h>
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>

#include <brec/brec_bg_pair_density.h>
#include <brec/brec_fg_pair_density.h>
#include <brec/brec_fg_bg_pair_density.h>
#include <brec/brec_bayesian_propagation.h>

//: Constructor
bool brec_bayesian_update_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input prob map p(x in B) (float map with values in [0,1]
  input_types.emplace_back("vil_image_view_base_sptr"); // input measurement map
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output new prob map
  output_types.emplace_back("vil_image_view_base_sptr");  // output new prob map as a byte image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  pro.parameters()->add("lambda for p(ad | gb)", "lambda1", 3.9645f);
  pro.parameters()->add("k for p(ad | gb)", "k1", 1.4778f);
  pro.parameters()->add("lambda for p(ad | not gb)", "lambda2", 0.9667f);
  pro.parameters()->add("k for p(ad | not gb)", "k2", 1.0f); // if k is 1, weibull becomes an exponential distribution

  return true;
}

bool brec_bayesian_update_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cout << "brec_bayesian_update_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr bg_map_v = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> bg_map(bg_map_v);

  vil_image_view_base_sptr glitch_map_v = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> glitch_map(glitch_map_v);

  vul_timer t2;
  t2.mark();

  brec_bayesian_propagation bp(bg_map);
  // to construct P(Ad | Gb), values based on stats collected from data
  float lambda1=-1.f; pro.parameters()->get_value("lambda1", lambda1);
  float k1=-1.f;      pro.parameters()->get_value("k1", k1);
  // to construct P(Ad | not Gb), values based on stats collected from data
  float lambda2=-1.f; pro.parameters()->get_value("lambda2", lambda2);
  float k2=-1.f;      pro.parameters()->get_value("k2", k2);
  bp.run_area(glitch_map, lambda1, k1, lambda2, k2);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(bp.bg_map_);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view<vxl_byte> out_b(bp.bg_map_.ni(), bp.bg_map_.nj());
  vil_convert_stretch_range_limited(bp.bg_map_, out_b, 0.0f, 1.0f);

  vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vxl_byte>(out_b);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr2);

  std::cout << " whole process took: " << t2.real() / 60000.0 << " mins.\n";

  return true;
}
