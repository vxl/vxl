// This is brl/bpro/core/sdet_pro/processes/sdet_texture_classifier_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <sdet/sdet_atmospheric_image_classifier.h>
#include <sdet/sdet_texture_classifier_params.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_random.h>
#include <vcl_algorithm.h>

// no need to check cause get data checks it anyway
void get_block(int i, int j, int bb, vcl_vector<vcl_pair<int, int> >& pixels)
{
  for (int ii = i-bb; ii < i+bb; ii++)
    for (int jj = j-bb; jj < j+bb; jj++) {
      pixels.push_back(vcl_pair<int, int>(ii, jj));
    }
}

//: initialize input and output types
bool sdet_texture_classifier_process2_cons(bprb_func_process& pro)
{
  // process takes 3 inputs:
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); //texton dictionary
  //input_types.push_back("vil_image_view_base_sptr"); //input image 
  input_types.push_back("unsigned");   //texture block size
  if (!pro.set_input_types(input_types))
    return false;

  // process has 1 output:
  // output[0]: output texture color image
  vcl_vector<vcl_string> output_types;
  output_types.push_back("vil_image_view_base_sptr");  // out prob image
  output_types.push_back("vil_image_view_base_sptr");  // colored output image
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process2 inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr dict = pro.get_input<sdet_texture_classifier_sptr>(0);
  vcl_cout << "max filter radius in dictionary: " << dict->max_filter_radius() << vcl_endl;
  unsigned ntextons = dict->get_number_of_textons();
  vcl_cout << " testing using the dictionary with the number of textons: " << ntextons << "\n categories:\n";
  vcl_vector<vcl_string> cats = dict->get_dictionary_categories();
  vcl_map<vcl_string, vil_rgb<vxl_byte> > cat_color_map;
  vnl_random rng(100);  // will always give the same colors
  for (unsigned kk = 0; kk < cats.size(); kk++) {
    cat_color_map[cats[kk]] = vil_rgb<vxl_byte>(rng.drand32()*255, rng.drand32()*255, rng.drand32()*255);
    vcl_cout << "\t\t" << cats[kk] << " color: " << cat_color_map[cats[kk]] << '\n';
    vcl_cout.flush();
  }

  //assumes a float image on the range [0, 1];
  /*vil_image_view_base_sptr view_ptr = pro.get_input<vil_image_view_base_sptr>(1);
  if (!view_ptr)
  {
    vcl_cout << "null image in sdet_texture_classifier_process\n";
    return false;
  }

  vil_image_view<float> fview(view_ptr);
  if (!dict->compute_filter_bank(fview))
  {
    vcl_cout << "problems computing filter bank on the image in sdet_texture_classifier_process\n";
    return false;
  }*/
  // assumes the filter bank is computed at the dictionary
  unsigned ni = dict->filter_responses().ni();
  unsigned nj = dict->filter_responses().nj();
  vcl_cout << "filter bank already computed at the dictionary, ni: " << ni << " nj: " << nj << vcl_endl;

  int bb = (int)(pro.get_input<unsigned>(1)); 
  int invalid = dict->max_filter_radius() + bb;
  
  vil_image_view<float> out(ni, nj);
  vil_image_view<vil_rgb<vxl_byte> > out_rgb(ni, nj);
  out.fill(0); out_rgb.fill(vil_rgb<vxl_byte>(0,0,0));
  
  /*
  // find the local coords of each pixel to retrieve corresponding img pixels
  for (int i = bb+1; i < ni-bb; i++) {
    for (int j = bb+1; j < nj-bb; j++) {

      // generate a block of pixels
      vcl_vector<vcl_pair<int, int> > pixels;
      get_block(i, j, bb, pixels);
      vcl_vector<vnl_vector<double> > data;
      dict->compute_data(pixels, data);
                
      // create texton histogram and classify, use the same weight for all samples put into the histogram
      float weight = 1.0f / (data.size());
      vcl_vector<float> hist(ntextons, 0.0f);
      dict->update_hist(data, weight, hist);
      vcl_pair<vcl_string, float> hc = dict->highest_prob_class(hist);
      out_rgb(i,j) = cat_color_map[hc.first];  
      out(i,j) = hc.second;
    }
    //vcl_cout << i << " ";
  }*/
  vil_image_view<int> texton_img(dict->filter_responses().ni(), dict->filter_responses().nj());
  texton_img.fill(0);
  dict->compute_textons_of_pixels(texton_img);
  vcl_cout << " computed textons of pixels..!\n"; vcl_cout.flush();
  // find the local coords of each pixel to retrieve corresponding img pixels
  float weight = 1.0f / (4*bb*bb);
  vcl_vector<float> hist(ntextons);
  for (int i = invalid; i < ni-invalid; i++) {
    for (int j = invalid; j < nj-invalid; j++) {
      
      vcl_fill(hist.begin(), hist.end(), 0.0f);
      for (int ii = i-bb; ii < i+bb; ii++)
        for (int jj = j-bb; jj < j+bb; jj++) {
          int indx = texton_img(ii, jj);
          hist[indx]+=weight;// for example, counts are normalized to probability
        }
      
      vcl_pair<vcl_string, float> hc = dict->highest_prob_class(hist);
      out_rgb(i,j) = cat_color_map[hc.first];  
      out(i,j) = hc.second;
    }
  }
  
  // return the output image
  vil_image_view_base_sptr img_ptr = new vil_image_view<float>(out);
  pro.set_output_val<vil_image_view_base_sptr>(0, img_ptr);
  vil_image_view_base_sptr img_ptr2 = new vil_image_view<vil_rgb<vxl_byte> >(out_rgb);
  pro.set_output_val<vil_image_view_base_sptr>(1, img_ptr2);
  return true;
}
