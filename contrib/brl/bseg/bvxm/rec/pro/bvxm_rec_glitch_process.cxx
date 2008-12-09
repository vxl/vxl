// This is brl/bseg/bvxm/rec/pro/bvxm_rec_glitch_process.cxx
#include "bvxm_rec_glitch_process.h"
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
#include <rec/bvxm_glitch.h>

//: Constructor
bvxm_rec_glitch_process::bvxm_rec_glitch_process()
{
  //input
  input_data_.resize(3,brdb_value_sptr(0));
  input_types_.resize(3);
  input_types_[0]= "vil_image_view_base_sptr"; //input probability frame
  input_types_[1]= "vil_image_view_base_sptr"; //input probability frame's mask
  input_types_[2]= "unsigned"; // size of the inner-square for the glitch mask (e.g. 5 means we're detecting foreground islands of 5x5 on background)

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
bvxm_rec_glitch_process::execute()
{
  // Sanity check
  if (!this->verify_inputs()){
    vcl_cerr << "In bvxm_rec_glitch_process::execute() -"
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
  bvxm_glitch::square_glitch(c_size, neighborhood, neighborhood_outer);

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
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
            
            if (i == 516 && j == 477) {
              vcl_cout << "map(" << ii << ", " << jj << "): " << map(ii, jj) << vcl_endl;
            }
            sum += map(ii,jj);
            prod_back *= (1.0f-map(ii, jj));
            prod_fore *= map(ii, jj);
            if (i == 516 && j == 477) {
              vcl_cout << "prod_back: " << prod_back << " prod_fore: " << prod_fore << vcl_endl;
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
            vcl_cout << "g_b(516, 477): " << g_b(i, j) << vcl_endl;
            vcl_cout << "g_bb(516, 477): " << g_bb(i, j) << vcl_endl;
            vcl_cout << "g_ff(516, 477): " << g_ff(i, j) << vcl_endl;
            vcl_cout << "g_fb(516, 477): " << g_fb(i, j) << vcl_endl;
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
        for (unsigned t = 0; t < neighborhood_outer.size(); t++) {
          int ii = i+neighborhood_outer[t].first;
          int jj = j+neighborhood_outer[t].second;
          if (ii >= 0 && jj >= 0 && ii < (int)ni && jj < (int)nj) {
            if (!input_mask(ii, jj)) {
              all_inside = false;
              break;
            }
            if (i == 516 && j == 477) {
              vcl_cout << "map(" << ii << ", " << jj << "): " << map(ii, jj) << vcl_endl;
            }
            sum += map(ii,jj);
            prod_back *= (1.0f-map(ii, jj));
            prod_fore *= map(ii, jj);
            if (i == 516 && j == 477) {
              vcl_cout << "prod_back: " << prod_back << " prod_fore: " << prod_fore << vcl_endl;
            }
          }
        }
        if (all_inside) {
          out2(i,j) = (float)vcl_abs(sum-out(i,j));
          g_b(i,j) = g_b(i,j)*prod_back;
          g_bb(i,j) = g_bb(i,j)*prod_back;
          g_ff(i,j) = g_ff(i,j)*prod_fore;
          g_fb(i,j) = g_fb(i,j)*prod_fore;
          if (i == 516 && j == 477) {
            vcl_cout << "g_b(516, 477): " << g_b(i, j) << vcl_endl;
            vcl_cout << "g_bb(516, 477): " << g_bb(i, j) << vcl_endl;
            vcl_cout << "g_ff(516, 477): " << g_ff(i, j) << vcl_endl;
            vcl_cout << "g_fb(516, 477): " << g_fb(i, j) << vcl_endl;
          }
        }
      }
    }

  brdb_value_sptr output0 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(out2));
  output_data_[0] = output0;

  float min, max;
  vil_math_value_range(out2, min, max);
  vcl_cout << "\t glitch map min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> out_byte2(ni, nj, 1);
  vil_convert_stretch_range_limited(out2, out_byte2, 0.0f, max);

  brdb_value_sptr output1 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte2));
  output_data_[1] = output1;
  
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

  brdb_value_sptr output2 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<float>(g_b));
  output_data_[2] = output2;

  vil_math_value_range(g_b, min, max);
  vcl_cout << "\t glitch normalized prob map min: " << min << " max: " << max << vcl_endl;
  vil_image_view<vxl_byte> out_byte3(ni, nj, 1);
  vil_convert_stretch_range_limited(g_b, out_byte3, 0.0f, max);
  brdb_value_sptr output3 = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<vxl_byte>(out_byte3));
  output_data_[3] = output3;

  return true;
}

