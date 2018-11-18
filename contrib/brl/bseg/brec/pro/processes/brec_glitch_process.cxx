// This is brl/bseg/brec/pro/processes/brec_glitch_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to generate a detection map for foreground glitches on background with a given center-surround geometry
//
// \author Ozge Can Ozcanli
// \date Dec 09, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - 02/03/09 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

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
bool brec_glitch_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame
  input_types.emplace_back("vil_image_view_base_sptr"); //input probability frame's mask
  input_types.emplace_back("unsigned"); // size of the inner-square for the glitch mask (e.g. 5 means we're detecting foreground islands of 5x5 on background)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  output_types.emplace_back("vil_image_view_base_sptr");
  //output_types.push_back("vil_image_view_base_sptr");  // output the glitch map as well
  //output_types.push_back("vil_image_view_base_sptr");  // output the glitch map as well
  ok = pro.set_output_types(output_types);
  return ok;
}

//: Execute the process
bool brec_glitch_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3){
    std::cerr << " brec_glitch_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
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
  brec_glitch::square_glitch(c_size, neighborhood, neighborhood_outer);

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        float sum = 0.0f;
        bool all_inside = true;
        float prod_back = 1.0f; float prod_fore = 1.0f;
        for (auto & t : neighborhood) {
          int ii = i+t.first;
          int jj = j+t.second;
          if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) {
            if (!input_mask(ii, jj)) {
              all_inside = false;
              break;
            }

            if (i == 516 && j == 477) {
              std::cout << "map(" << ii << ", " << jj << "): " << map(ii, jj) << std::endl;
            }
            sum += map(ii,jj);
            prod_back *= (1.0f-map(ii, jj));
            prod_fore *= map(ii, jj);
            if (i == 516 && j == 477) {
              std::cout << "prod_back: " << prod_back << " prod_fore: " << prod_fore << std::endl;
            }
          }
        }
        if (all_inside) {
          out(i,j) = sum;
          g_b(i,j) = prod_fore;
          g_bb(i,j) = prod_back;
          g_ff(i,j) = prod_fore;
          g_fb(i,j) = prod_back;
          if (i == 516 && j == 477) {
            std::cout << "g_b(516, 477): " << g_b(i, j) << std::endl;
            std::cout << "g_bb(516, 477): " << g_bb(i, j) << std::endl;
            std::cout << "g_ff(516, 477): " << g_ff(i, j) << std::endl;
            std::cout << "g_fb(516, 477): " << g_fb(i, j) << std::endl;
          }
        }
      }
    }

  vil_image_view<float> out2(ni, nj, 1);
  out2.fill(0);

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        float sum = 0.0f;
        bool all_inside = true;
        float prod_back = 1.0f; float prod_fore = 1.0f;
        for (auto & t : neighborhood_outer) {
          int ii = i+t.first;
          int jj = j+t.second;
          if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) {
            if (!input_mask(ii, jj)) {
              all_inside = false;
              break;
            }
            if (i == 516 && j == 477) {
              std::cout << "map(" << ii << ", " << jj << "): " << map(ii, jj) << std::endl;
            }
            sum += map(ii,jj);
            prod_back *= (1.0f-map(ii, jj));
            prod_fore *= map(ii, jj);
            if (i == 516 && j == 477) {
              std::cout << "prod_back: " << prod_back << " prod_fore: " << prod_fore << std::endl;
            }
          }
        }
        if (all_inside) {
          out2(i,j) = (float)std::abs(sum-out(i,j));
          g_b(i,j) = g_b(i,j)*prod_back;
          g_bb(i,j) = g_bb(i,j)*prod_back;
          g_ff(i,j) = g_ff(i,j)*prod_fore;
          g_fb(i,j) = g_fb(i,j)*prod_fore;
          if (i == 516 && j == 477) {
            std::cout << "g_b(516, 477): " << g_b(i, j) << std::endl;
            std::cout << "g_bb(516, 477): " << g_bb(i, j) << std::endl;
            std::cout << "g_ff(516, 477): " << g_ff(i, j) << std::endl;
            std::cout << "g_fb(516, 477): " << g_fb(i, j) << std::endl;
          }
        }
      }
    }

  pro.set_output_val<vil_image_view_base_sptr>(0, new vil_image_view<float>(out2));

  float min, max;
  vil_math_value_range(out2, min, max);
  std::cout << "\t glitch map min: " << min << " max: " << max << std::endl;
  vil_image_view<vxl_byte> out_byte2(ni, nj, 1);
  vil_convert_stretch_range_limited(out2, out_byte2, 0.0f, max);

  pro.set_output_val<vil_image_view_base_sptr>(1, new vil_image_view<vxl_byte>(out_byte2));

  //: normalize the g_b map
  for (unsigned j = 0; j<nj; ++j) {
    for (unsigned i = 0; i<ni; ++i) {
      if (g_bb(i,j) < 0) { // if not on a valid pixel continue;
        g_b(i,j) = 0.0f;
        continue;
      }
      float margin = g_bb(i,j) + g_ff(i,j) + g_fb(i,j) + g_b(i,j);
      //if (margin > 0.0f)
        g_b(i,j) = g_b(i,j)/margin;
    }
  }

  pro.set_output_val<vil_image_view_base_sptr>(2, new vil_image_view<float>(g_b));

  vil_math_value_range(g_b, min, max);
  std::cout << "\t glitch normalized prob map min: " << min << " max: " << max << std::endl;
  vil_image_view<vxl_byte> out_byte3(ni, nj, 1);
  vil_convert_stretch_range_limited(g_b, out_byte3, 0.0f, max);

  pro.set_output_val<vil_image_view_base_sptr>(3, new vil_image_view<vxl_byte>(out_byte3));

  return true;
}
