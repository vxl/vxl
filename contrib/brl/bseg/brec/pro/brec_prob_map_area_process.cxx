// This is brl/bseg/brec/pro/brec_prob_map_area_process.cxx
#include "brec_prob_map_area_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <brdb/brdb_value.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl2_pro/vidl2_pro_utils.h>
#include <brec/brec_glitch.h>

//: Constructor
brec_prob_map_area_process::brec_prob_map_area_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask
  input_types_[2]= "unsigned"; // size of the inner-square to measure area (e.g. 5 means 5x5 area mask)

  //output
  output_data_.resize(2, brdb_value_sptr(0));
  output_types_.resize(2);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
}

//: Execute the process
bool
brec_prob_map_area_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In brec_prob_map_area_process::execute() -"
             << " invalid inputs\n";
    return false;
  }

  brdb_value_t<vil_image_view_base_sptr>* input0 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[0].ptr());
  vil_image_view_base_sptr temp = input0->value();
  vil_image_view<float> map = *vil_convert_cast(float(), temp);
  if (temp->pixel_format() != VIL_PIXEL_FORMAT_FLOAT)
    return false;
  unsigned ni = map.ni(), nj = map.nj();

  brdb_value_t<vil_image_view_base_sptr>* input1 =
    static_cast<brdb_value_t<vil_image_view_base_sptr>* >(input_data_[1].ptr());
  temp = input1->value();
  vil_image_view<bool> input_mask = *vil_convert_cast(bool(), temp);

  brdb_value_t<unsigned>* input2 = static_cast<brdb_value_t<unsigned>* >(input_data_[2].ptr());
  unsigned c_size = input2->value();  // center size

  vil_image_view<float> out(ni, nj, 1);
  out.fill(0);

  //: g_b will contain the normalized glitch probability and will be returned
  vil_image_view<float> g_b(ni, nj, 1), g_bb(ni, nj, 1), g_ff(ni, nj, 1), g_fb(ni, nj, 1);
  g_b.fill(-1.0f); g_bb.fill(-1.0f); g_ff.fill(-1.0f); g_fb.fill(-1.0f);

  vcl_vector<vcl_pair<int, int> > neighborhood;
  vcl_vector<vcl_pair<int, int> > neighborhood_outer;
  //: use square glitch to find a center of c_size x c_size
  brec_glitch::square_glitch(c_size, neighborhood, neighborhood_outer);

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (input_mask(i,j)) {
        float sum = 0.0f;
        bool all_inside = true;
      
        for (unsigned t = 0; t < neighborhood.size(); t++) {
          int ii = i+neighborhood[t].first;
          int jj = j+neighborhood[t].second;
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
  vcl_cout << "\t area map min: " << min << " max: " << max << vcl_endl;
  vil_convert_stretch_range_limited(out, out_byte, 0.0f, max);

  //vil_image_view<float> dummy(ni, nj, 1), dummy2(ni, nj, 1);
  //dummy.fill(max);
  //vil_math_image_difference(dummy, out, dummy2);
  //vil_convert_stretch_range_limited(dummy2, out_byte, 0.0f, max);

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(out));
  //brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(dummy2));
  output_data_[0] = output0;

  brdb_value_sptr output1 =
    new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte));
  output_data_[1] = output1;

  return true;
}

