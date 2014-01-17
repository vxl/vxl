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
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>
#include <bbas_pro/bbas_1d_array_float.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vul/vul_file.h>

//: initialize input and output types
bool sdet_texture_classifier_roc_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("sdet_texture_classifier_sptr"); //texton dictionary, category names and colors are in the dictionary
  input_types.push_back("vil_image_view_base_sptr"); // classification output - prob values for each outputted category
  input_types.push_back("vil_image_view_base_sptr"); // classification output as a color image - shows which category had the max for each pixel
  input_types.push_back("vil_image_view_base_sptr"); // input original image, to mark some pixels red
  input_types.push_back("vcl_string"); // prefix for ground truth binary vsol spatial object files, <prefix>_<category_name>.bin
  input_types.push_back("vcl_string"); // name of the category to use as positive (the rest of the available categories will be used as negatives)
  input_types.push_back("vcl_string");  // a simple text file with the list of ids&colors for each category, if passed as "" just use 0, 1, 2, .. etc.
  if (!pro.set_input_types(input_types))
    return false;

  vcl_vector<vcl_string> output_types;
  output_types.push_back("bbas_1d_array_float_sptr");  // tp
  output_types.push_back("bbas_1d_array_float_sptr");  // tn
  output_types.push_back("bbas_1d_array_float_sptr");  // fp
  output_types.push_back("bbas_1d_array_float_sptr");  // fn
  output_types.push_back("bbas_1d_array_float_sptr");  // tpr
  output_types.push_back("bbas_1d_array_float_sptr");  // fpr
  output_types.push_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_roc_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    vcl_cout << pro.name() << "texture classifier process2 inputs are not valid"<< vcl_endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr dict = pro.get_input<sdet_texture_classifier_sptr>(0);
  vcl_cout << "max filter radius in dictionary: " << dict->max_filter_radius() << vcl_endl;
  vil_image_view<float> input_prob_img(pro.get_input<vil_image_view_base_sptr>(1));
  vil_image_view<vil_rgb<vxl_byte> > input_img(pro.get_input<vil_image_view_base_sptr>(2));
  unsigned ni = input_img.ni(); unsigned nj = input_img.nj();
  vil_image_view<vxl_byte> input_orig_img(pro.get_input<vil_image_view_base_sptr>(3));
  vcl_string prefix = pro.get_input<vcl_string>(4);
  vcl_string pos_category = pro.get_input<vcl_string>(5);
  vcl_string cat_ids_file = pro.get_input<vcl_string>(6);

   // load the positive pixels
  vcl_string pos_file_name = prefix + "_" + pos_category + ".bin";
  vcl_vector<vgl_polygon<double> > positives = sdet_texture_classifier::load_polys(pos_file_name);
  vcl_cout << "there are " << positives.size() << " positive example polygons read!\n";

  vcl_vector<vcl_string> cats = dict->get_dictionary_categories();

  // now load the rest of polygons as negative examples
  vcl_vector<vgl_polygon<double> > negatives;
  vcl_cout << " using : ";
  for (unsigned kk = 0; kk < cats.size(); kk++) {
    if (cats[kk].compare(pos_category) == 0)
      continue;
    vcl_string filename = prefix + "_" + cats[kk] + ".bin";
    if (!vul_file::exists(filename))
      continue;
    vcl_vector<vgl_polygon<double> > negs = sdet_texture_classifier::load_polys(filename);
    vcl_cout << cats[kk] << " ";
    for (unsigned ii = 0; ii < negs.size(); ii++)
      negatives.push_back(negs[ii]);
  }
  vcl_cout << " total of " << negatives.size() << " polygons as negatives!\n";

  vcl_map<vcl_string, vil_rgb<vxl_byte> > cat_color_map;
  vil_rgb<vxl_byte> positive_color;
  /*
  vnl_random rng(100);  // will always give the same colors  // WARNING, assumes that the input color map is generated with the same dictionary, and same order of categories
  for (unsigned kk = 0; kk < cats.size(); kk++) {
    cat_color_map[cats[kk]] = vil_rgb<vxl_byte>(rng.drand32()*255, rng.drand32()*255, rng.drand32()*255);
    vcl_cout << "\t\t" << cats[kk] << " color: " << cat_color_map[cats[kk]] << '\n';
    vcl_cout.flush();
    if (cats[kk].compare(pos_category) == 0)
      positive_color = cat_color_map[cats[kk]];
  }*/
  vcl_ifstream ifs(cat_ids_file.c_str());
  vcl_string cat_name; int id; int r, g, b;
  ifs >> cat_name;
  while (!ifs.eof()) {
    ifs >> id; ifs >> r; ifs >> g; ifs >> b;
    //cat_id_map[cat_name] = (unsigned char)id;
    cat_color_map[cat_name] = vil_rgb<vxl_byte>(r,g,b);
    vcl_cout << "\t\t" << cat_name << " color: " << cat_color_map[cat_name] << '\n';
    if (cat_name.compare(pos_category) == 0)
      positive_color = cat_color_map[cat_name];
    ifs >> cat_name;
  }
  ifs.close();

  vil_image_view<vil_rgb<vxl_byte> > out_rgb(ni, nj);
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      out_rgb(i,j).r = input_orig_img(i,j);
      out_rgb(i,j).g = input_orig_img(i,j);
      out_rgb(i,j).b = input_orig_img(i,j);
    }

  // go over the pixels and collect pixels of positives and negatives
  vcl_vector<vcl_pair<int, int> > pos_pixels;
  vcl_vector<vcl_pair<int, int> > neg_pixels;
  for (unsigned ii = 0; ii < positives.size(); ii++) {
    vgl_polygon_scan_iterator<double> psi(positives[ii]);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        pos_pixels.push_back(vcl_pair<int, int>(i,j));
      }
    }
  }
  for (unsigned ii = 0; ii < negatives.size(); ii++) {
    vgl_polygon_scan_iterator<double> psi(negatives[ii]);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        neg_pixels.push_back(vcl_pair<int, int>(i,j));
      }
    }
  }
  double thresholds[9] = {0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001};
  const unsigned int num_thresholds = 9;
  bbas_1d_array_float * tp=new bbas_1d_array_float(num_thresholds);
  bbas_1d_array_float * tn=new bbas_1d_array_float(num_thresholds);
  bbas_1d_array_float * fp=new bbas_1d_array_float(num_thresholds);
  bbas_1d_array_float * fn=new bbas_1d_array_float(num_thresholds);
  bbas_1d_array_float * tpr=new bbas_1d_array_float(num_thresholds);
  bbas_1d_array_float * fpr=new bbas_1d_array_float(num_thresholds);

  for (unsigned k = 0; k < num_thresholds; k++) {
    tp->data_array[k] = 0;
    fp->data_array[k] = 0;
    tn->data_array[k] = 0;
    fn->data_array[k] = 0;
  }

  // count 
  for (unsigned ii = 0; ii < pos_pixels.size(); ii++) {
    unsigned i = pos_pixels[ii].first; 
    unsigned j = pos_pixels[ii].second;
    if (input_img(i, j) == positive_color) {
      for (unsigned k = 0; k < num_thresholds; k++)
      {
        if (input_prob_img(i, j) > thresholds[k])
          tp->data_array[k] = tp->data_array[k] + 1;
        else
          fn->data_array[k] = fn->data_array[k] + 1;
      }
    } else {
      for (unsigned k = 0; k < num_thresholds; k++)
        fn->data_array[k] = fn->data_array[k] + 1;
    }
  }
  for (unsigned ii = 0; ii < neg_pixels.size(); ii++) {
    unsigned i = neg_pixels[ii].first; 
    unsigned j = neg_pixels[ii].second;
    if (input_img(i, j) == positive_color) {
      for (unsigned k = 0; k < num_thresholds; k++)
      {
        if (input_prob_img(i, j) > thresholds[k])
          fp->data_array[k] = fp->data_array[k] + 1;
        else
          tn->data_array[k] = tn->data_array[k] + 1;
      }
    } else {
      for (unsigned k = 0; k < num_thresholds; k++)
        tn->data_array[k] = tn->data_array[k] + 1;
    }
  }

  for (unsigned int pnt=0; pnt<num_thresholds; ++pnt) {
    tpr->data_array[pnt]= tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt]= fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
  }
  
  pro.set_output_val<bbas_1d_array_float_sptr>(0, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(4, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(5, fpr);
  // return the output image
  vil_image_view_base_sptr img_ptr = new vil_image_view<vil_rgb<vxl_byte> >(out_rgb);
  pro.set_output_val<vil_image_view_base_sptr>(6, img_ptr);
  return true;
}
