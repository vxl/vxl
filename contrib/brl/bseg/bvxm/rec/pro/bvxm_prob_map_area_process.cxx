// This is brl/bseg/bbgm/pro/bvxm_prob_map_area_process.cxx
#include "bvxm_prob_map_area_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_list.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <brip/brip_vil_float_ops.h>
#include <core/vidl2_pro/vidl2_pro_utils.h>


//: Constructor
bvxm_prob_map_area_process::bvxm_prob_map_area_process()
{
  //input
  input_data_.resize(2,brdb_value_sptr(0));
  input_types_.resize(2);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask

  //output 
  output_data_.resize(6, brdb_value_sptr(0));
  output_types_.resize(6);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";  // output the glitch map as well
  output_types_[3] = "vil_image_view_base_sptr";  // output the glitch map as well
  output_types_[4] = "vil_image_view_base_sptr";  // output the glitch map as well
  output_types_[5] = "vil_image_view_base_sptr";  // output the glitch map as well
}

//: Execute the process
bool
bvxm_prob_map_area_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bvxm_prob_map_area_process::execute() -"
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

  vil_image_view<float> out(ni, nj, 1);
  out.fill(0);

  //: g_b will contain the normalized glitch probability and will be returned
  vil_image_view<float> g_b(ni, nj, 1), g_bb(ni, nj, 1), g_ff(ni, nj, 1), g_fb(ni, nj, 1);
  g_b.fill(0); g_bb.fill(0); g_ff.fill(0); g_fb.fill(0);

  vcl_vector<vcl_pair<int, int> > neighborhood;
  int ns = 2; // for 5x5 neighborhood
  //int ns = 1; // for 3x3 neighborhood
  for (int l = -ns; l < ns+1; l++)
    for (int m = -ns; m < ns+1; m++)
      neighborhood.push_back(vcl_pair<int, int>(l,m));
  neighborhood.pop_back(); // pop the last one to have equal number of pixels in the center and in the surround

  for(unsigned j = 3; j<nj-3; ++j)
    for(unsigned i = 3; i<ni-3; ++i)
      {
        if (input_mask(i,j)) {
          float sum = 0.0f;
          bool all_inside = true;
          float prod_back = 1.0f; float prod_fore = 1.0f;
          for (unsigned t = 0; t < neighborhood.size(); t++) {
            int ii = i+neighborhood[t].first;
            int jj = j+neighborhood[t].second;
            if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) { 
              if (!input_mask(ii, jj)) {
                all_inside = false;
                break;
              }
              sum += map(ii,jj);
              prod_back *= (1.0f-map(ii, jj));
              prod_fore *= map(ii, jj);
            }
          }
          if (all_inside) {
            out(i,j) = sum;
            g_b(i,j) = prod_fore;
            g_bb(i,j) = prod_back;
            g_ff(i,j) = prod_fore;
            g_fb(i,j) = prod_back;
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

  vil_image_view<float> out2(ni, nj, 1);
  out2.fill(0);
  vcl_vector<vcl_pair<int, int> > neighborhood_outer;
  for (int l = -(ns+1); l < (ns+2); l++)
    neighborhood_outer.push_back(vcl_pair<int, int>(l,-(ns+1)));
  for (int l = -(ns+1); l < (ns+2); l++)
    neighborhood_outer.push_back(vcl_pair<int, int>(l,(ns+1)));
  for (int m = -(ns+1); m < (ns+2); m++)
    neighborhood_outer.push_back(vcl_pair<int, int>(-(ns+1),m));
  for (int m = -(ns+1); m < (ns+2); m++)
    neighborhood_outer.push_back(vcl_pair<int, int>((ns+1),m));

  for(unsigned j = 3; j<nj-3; ++j)
    for(unsigned i = 3; i<ni-3; ++i)
      {
        if (input_mask(i,j)) {
          float sum = 0.0f;
          bool all_inside = true;
          float prod_back = 1.0f; float prod_fore = 1.0f;
          for (unsigned t = 0; t < neighborhood_outer.size(); t++) {
            int ii = i+neighborhood_outer[t].first;
            int jj = j+neighborhood_outer[t].second;
            if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) {
              if (!input_mask(ii, jj)) {
                all_inside = false;
                break;
              }
              sum += map(ii,jj);
              prod_back *= (1.0f-map(ii, jj));
              prod_fore *= map(ii, jj);
            }
          }
          if (all_inside) {
            out2(i,j) = (float)vcl_abs(sum-out(i,j));
            g_b(i,j) = g_b(i,j)*prod_back;
            g_bb(i,j) = g_bb(i,j)*prod_back;
            g_ff(i,j) = g_ff(i,j)*prod_fore;
            g_fb(i,j) = g_fb(i,j)*prod_fore;
          }
        }
      }

  brdb_value_sptr output2 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(out2));
  output_data_[2] = output2;

  vil_math_value_range(out2, min, max);
  vcl_cout << "\t glitch map min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> out_byte2(ni, nj, 1);
  vil_convert_stretch_range_limited(out2, out_byte2, 0.0f, max);
  
  brdb_value_sptr output3 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte2));
  output_data_[3] = output3;

  //: normalize the g_b map 
  for(unsigned j = 3; j<nj-3; ++j) {
    for(unsigned i = 3; i<ni-3; ++i) {
      if (g_bb(i,j) <= 0)  // if not on a valid pixel continue;
        continue;
      g_b(i,j) = g_b(i,j)/(g_bb(i,j) + g_ff(i,j) + g_fb(i,j) + g_b(i,j));
    }
  }

  brdb_value_sptr output4 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(g_b));
  output_data_[4] = output4;

  vil_math_value_range(g_b, min, max);
  vcl_cout << "\t glitch normalized prob map min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> out_byte3(ni, nj, 1);
  vil_convert_stretch_range_limited(g_b, out_byte3, 0.0f, max);
  brdb_value_sptr output5 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte3));
  output_data_[5] = output5;

  return true;
}

  
