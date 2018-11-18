// This is brl/bseg/brec/pro/processes/brec_prob_map_area_process.cxx

//:
// \file
// \brief A process to find expected area at each pixel for a given prob map
//
// \author Ozge Can Ozcanli
// \date November 06, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <brec/brec_glitch.h>

//: Constructor
bool brec_prob_map_area_process_cons(bprb_func_process& pro)
{
  //input
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame's mask
  input_types.emplace_back("unsigned"); // size of the inner-square to measure area (e.g. 5 means 5x5 area mask)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_prob_map_area_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cout << "brec_prob_map_area_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> map = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = map.ni(), nj = map.nj();

  temp = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  auto c_size = pro.get_input<unsigned>(i++);

  vil_image_view<float> out(ni, nj, 1);
  out.fill(0);

  //: g_b will contain the normalized glitch probability and will be returned
  vil_image_view<float> g_b(ni, nj, 1), g_bb(ni, nj, 1), g_ff(ni, nj, 1), g_fb(ni, nj, 1);
  g_b.fill(-1.0f); g_bb.fill(-1.0f); g_ff.fill(-1.0f); g_fb.fill(-1.0f);

  std::vector<std::pair<int, int> > neighborhood;
  std::vector<std::pair<int, int> > neighborhood_outer;
  //: use square glitch to find a center of c_size x c_size
  brec_glitch::square_glitch(c_size, neighborhood, neighborhood_outer);

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        float sum = 0.0f;
        bool all_inside = true;

        for (auto & t : neighborhood) {
          int ii = i+t.first;
          int jj = j+t.second;
          if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) {
            if (!input_mask(ii, jj)) {
              all_inside = false;
              break;
            }

            sum += map(ii,jj);
          }
        }
        if (all_inside) {
          out(i,j) = sum;
        }
      }
    }

  vil_image_view<vxl_byte> out_byte(ni, nj, 1);
  float min, max;
  vil_math_value_range(out, min, max);
  std::cout << "\t area map min: " << min << " max: " << max << std::endl;
  vil_convert_stretch_range_limited(out, out_byte, 0.0f, max);

  //vil_image_view<float> dummy(ni, nj, 1), dummy2(ni, nj, 1);
  //dummy.fill(max);
  //vil_math_image_difference(dummy, out, dummy2);
  //vil_convert_stretch_range_limited(dummy2, out_byte, 0.0f, max);

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out));
  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vxl_byte>(out_byte));

  return true;
}
