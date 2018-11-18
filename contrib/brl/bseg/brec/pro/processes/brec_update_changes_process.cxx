// This is brl/bseg/brec/pro/processes/brec_update_changes_process.cxx

//:
// \file
// \brief A process to update a change map iteratively based on background and foreground models
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli
// \date October 01, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vil/io/vil_io_image_view.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>

#include <brec/brec_bg_pair_density.h>
#include <brec/brec_fg_pair_density.h>
#include <brec/brec_fg_bg_pair_density.h>
#include <brec/brec_bayesian_propagation.h>

//: Constructor
bool brec_update_changes_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");      // input change map
  input_types.emplace_back("vil_image_view_base_sptr");      // input orig view
  input_types.emplace_back("unsigned");      // number of passes
  input_types.emplace_back("float");      // sigma for foreground similarity
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");      // output new prob map
  output_types.emplace_back("vil_image_view_base_sptr");      // output new prob map as a byte image
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_update_changes_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_recognize_structure_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  vil_image_view_base_sptr change_map = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> change_map_f(change_map);

  vil_image_view_base_sptr orig_view = pro.get_input<vil_image_view_base_sptr>(i++);
  auto n = pro.get_input<unsigned>(i++);
  auto sigma = pro.get_input<float>(i++);

  vul_timer t2;
  t2.mark();

  vil_image_view<vxl_byte> orig_img(orig_view);

  brec_fg_pair_density fgp;
  //fgp.set_sigma(0.058); // set manually for now
  //fgp.set_sigma(0.02); // set manually for now
  //fgp.set_sigma(0.2); // set manually for now
  fgp.set_sigma(sigma);
  brec_fg_bg_pair_density fgbgp;
  //fgbgp.set_sigma(0.058);
  //fgbgp.set_sigma(0.02);
  //fgbgp.set_sigma(0.2);
  fgbgp.set_sigma(sigma);
  std::cout << "\t update process running with sigma: " << sigma << std::endl;

  brec_bayesian_propagation bp(orig_img, change_map_f);
  bp.initialize_bg_map(); // just turning change_map_f (a density map) into a probability map with values in [0,1]
  bp.run_using_prob_density_as_bgp(fgp, fgbgp, n);

  vil_image_view_base_sptr out_map_sptr = new vil_image_view<float>(bp.bg_map_);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_map_sptr);

  vil_image_view<vxl_byte> out_b(bp.bg_map_.ni(), bp.bg_map_.nj());
  vil_convert_stretch_range_limited(bp.bg_map_, out_b, 0.0f, 1.0f);

  vil_image_view_base_sptr out_map_sptr2 = new vil_image_view<vxl_byte>(out_b);
  pro.set_output_val<vil_image_view_base_sptr>(1, out_map_sptr2);

  std::cout << " whole process took: " << t2.real() / 60000.0 << " mins.\n";

  return true;
}
