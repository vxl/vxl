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
#include <vil/vil_math.h>

//: initialize input and output types
bool sdet_texture_classifier_roc_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texton dictionary, category names and colors are in the dictionary
  input_types.emplace_back("vil_image_view_base_sptr"); // classification output - prob values for each outputted category
  input_types.emplace_back("vil_image_view_base_sptr"); // classification output as a color image - shows which category had the max for each pixel
  input_types.emplace_back("vil_image_view_base_sptr"); // input original image, to mark some pixels red
  input_types.emplace_back("vcl_string"); // prefix for ground truth binary vsol spatial object files, <prefix>_<category_name>.bin
  input_types.emplace_back("vcl_string"); // name of the category to use as positive (the rest of the available categories will be used as negatives)
  input_types.emplace_back("vcl_string");  // a simple text file with the list of ids&colors for each category, if passed as "" just use 0, 1, 2, .. etc.
  if (!pro.set_input_types(input_types))
    return false;

  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_float_sptr");  // threshold
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tpr
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fpr
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red
  return pro.set_output_types(output_types);
}

//: generates the edge map
bool sdet_texture_classifier_roc_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cout << pro.name() << "texture classifier process2 inputs are not valid"<< std::endl;
    return false;
  }
  // get inputs
  sdet_texture_classifier_sptr dict = pro.get_input<sdet_texture_classifier_sptr>(0);
  std::cout << "max filter radius in dictionary: " << dict->max_filter_radius() << std::endl;
  vil_image_view<float> input_prob_img(pro.get_input<vil_image_view_base_sptr>(1));
  vil_image_view<vil_rgb<vxl_byte> > input_img(pro.get_input<vil_image_view_base_sptr>(2));
  unsigned ni = input_img.ni(); unsigned nj = input_img.nj();
  vil_image_view<vxl_byte> input_orig_img(pro.get_input<vil_image_view_base_sptr>(3));
  std::string prefix = pro.get_input<std::string>(4);
  std::string pos_category = pro.get_input<std::string>(5);
  std::string cat_ids_file = pro.get_input<std::string>(6);

   // load the positive pixels
  std::string pos_file_name = prefix + "_" + pos_category + ".bin";
  std::vector<vgl_polygon<double> > positives = sdet_texture_classifier::load_polys(pos_file_name);
  std::cout << "there are " << positives.size() << " positive example polygons read!\n";

  std::vector<std::string> cats = dict->get_dictionary_categories();

  // now load the rest of polygons as negative examples
  std::vector<vgl_polygon<double> > negatives;
  std::cout << " using : ";
  for (auto & cat : cats) {
    if (cat.compare(pos_category) == 0)
      continue;
    std::string filename = prefix + "_" + cat + ".bin";
    if (!vul_file::exists(filename))
      continue;
    std::vector<vgl_polygon<double> > negs = sdet_texture_classifier::load_polys(filename);
    std::cout << cat << " ";
    for (const auto & neg : negs)
      negatives.push_back(neg);
  }
  std::cout << " total of " << negatives.size() << " polygons as negatives!\n";

  std::map<std::string, vil_rgb<vxl_byte> > cat_color_map;
  vil_rgb<vxl_byte> positive_color;
  /*
  vnl_random rng(100);  // will always give the same colors  // WARNING, assumes that the input color map is generated with the same dictionary, and same order of categories
  for (unsigned kk = 0; kk < cats.size(); kk++) {
    cat_color_map[cats[kk]] = vil_rgb<vxl_byte>(rng.drand32()*255, rng.drand32()*255, rng.drand32()*255);
    std::cout << "\t\t" << cats[kk] << " color: " << cat_color_map[cats[kk]] << '\n';
    std::cout.flush();
    if (cats[kk].compare(pos_category) == 0)
      positive_color = cat_color_map[cats[kk]];
  }*/
  std::ifstream ifs(cat_ids_file.c_str());
  std::string cat_name; int id; int r, g, b;
  ifs >> cat_name;
  while (!ifs.eof()) {
    ifs >> id; ifs >> r; ifs >> g; ifs >> b;
    //cat_id_map[cat_name] = (unsigned char)id;
    cat_color_map[cat_name] = vil_rgb<vxl_byte>(r,g,b);
    std::cout << "\t\t" << cat_name << " color: " << cat_color_map[cat_name] << '\n';
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
  std::vector<std::pair<int, int> > pos_pixels;
  std::vector<std::pair<int, int> > neg_pixels;
  for (const auto & positive : positives) {
    vgl_polygon_scan_iterator<double> psi(positive);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        pos_pixels.emplace_back(i,j);
      }
    }
  }
  for (const auto & negative : negatives) {
    vgl_polygon_scan_iterator<double> psi(negative);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        neg_pixels.emplace_back(i,j);
      }
    }
  }
  //double thresholds[12] = {0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001, 0.00000001, 0.000000001, 0.0000000001, 0.00000000001, 0.000000000001};
  float min_val = 0.0f;
  float max_val = 1.0f;
  //vil_math_value_range(input_prob_img, min_val, max_val);
  std::vector<float> thresholds;
  unsigned num_thres = 200;
  float delta = (max_val-min_val) / num_thres;
  for (unsigned i = 0; i < num_thres; i++)
    thresholds.push_back(min_val + delta*i);
  thresholds.push_back(max_val);
  thresholds.push_back(max_val+delta);
  const unsigned int num_thresholds = thresholds.size();
  std::cout << "image min: " << min_val << ", max: " << max_val << "\n threshold: ";
  for (unsigned i = 0; i < num_thresholds; i++)
    std::cout << thresholds[i] << ' ';
  std::cout << std::flush << std::endl;

  auto * tp=new bbas_1d_array_float(num_thresholds);
  auto * tn=new bbas_1d_array_float(num_thresholds);
  auto * fp=new bbas_1d_array_float(num_thresholds);
  auto * fn=new bbas_1d_array_float(num_thresholds);
  auto * tpr=new bbas_1d_array_float(num_thresholds);
  auto * fpr=new bbas_1d_array_float(num_thresholds);

  for (unsigned k = 0; k < num_thresholds; k++) {
    tp->data_array[k] = 0;
    fp->data_array[k] = 0;
    tn->data_array[k] = 0;
    fn->data_array[k] = 0;
  }

  // count
  for (auto & pos_pixel : pos_pixels) {
    unsigned i = pos_pixel.first;
    unsigned j = pos_pixel.second;
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
  for (auto & neg_pixel : neg_pixels) {
    unsigned i = neg_pixel.first;
    unsigned j = neg_pixel.second;
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

  auto * thres_out=new bbas_1d_array_float(num_thresholds);
  for (unsigned k = 0; k < num_thresholds; k++) {
    thres_out->data_array[k] = thresholds[k];
  }
  pro.set_output_val<bbas_1d_array_float_sptr>(0, thres_out);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(4, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(5, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(6, fpr);
  // return the output image
  vil_image_view_base_sptr img_ptr = new vil_image_view<vil_rgb<vxl_byte> >(out_rgb);
  pro.set_output_val<vil_image_view_base_sptr>(7, img_ptr);
  return true;
}



// ROC evaluation for a binary classifier
//: initialize input and output types
bool sdet_texture_classifier_roc_process2_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(3);
  input_types_[0] = "vil_image_view_base_sptr";  // classification output image
  input_types_[1] = "vcl_string";                // ground truth positive
  input_types_[2] = "vcl_string";                // option to define ground truth being lower than threshold or higher than threshold
  std::vector<std::string> output_types_(7);
  output_types_[0] = "bbas_1d_array_float_sptr";  // thresholds
  output_types_[1] = "bbas_1d_array_float_sptr";  // TP
  output_types_[2] = "bbas_1d_array_float_sptr";  // TN
  output_types_[3] = "bbas_1d_array_float_sptr";  // FP
  output_types_[4] = "bbas_1d_array_float_sptr";  // FN
  output_types_[5] = "bbas_1d_array_float_sptr";  // TPR
  output_types_[6] = "bbas_1d_array_float_sptr";  // FPR
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: generate the ROC
bool sdet_texture_classifier_roc_process2(bprb_func_process& pro)
{
  if (!pro.verify_inputs())
  {
    std::cerr << pro.name() << ": invalid inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  std::string gt_pos_file   = pro.get_input<std::string>(in_i++);
  std::string positive_sign = pro.get_input<std::string>(in_i++);

  // load the ground truth positive
  // a polygon file store all the positive pixels
  if (!vul_file::exists(gt_pos_file)) {
    std::cerr << pro.name() << ": can not find input ground truth positive!\n";
    return false;
  }
  std::vector<vgl_polygon<double> > positives = sdet_texture_classifier::load_polys(gt_pos_file);

  auto* in_img = dynamic_cast<vil_image_view<float>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": unsupported image pixel format: " << in_img_sptr->pixel_format() << ", only float allowed!\n";
    return false;
  }
  if (in_img->nplanes() != 1) {
    std::cerr << pro.name() << ": process support single plane image!\n";
    return false;
  }

  unsigned ni = in_img->ni();
  unsigned nj = in_img->nj();

  // go over the pixels and collect pixel of positive and negative
  std::vector<std::pair<int, int> > pos_pixels;
  std::vector<std::pair<int, int> > neg_pixels;
  for (const auto & positive : positives)
  {
    vgl_polygon_scan_iterator<double> psi(positive);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        pos_pixels.emplace_back(i, j);
      }
    }
  }
  std::cout << "location negative pixels...";
  for (unsigned i = 0; i < ni; i++) {
    for (unsigned j = 0; j < nj; j++) {
      bool is_neg = true;
      for (unsigned k = 0; (k < positives.size() && is_neg); k++) {
        if (positives[k].contains((double)i, (double)j))
          is_neg = false;
      }
      if (is_neg)
        neg_pixels.emplace_back(i,j);
    }
  }
  std::cout << '\n';
  std::cout << positives.size() << " positive regions are loaded and " << pos_pixels.size() << " pixels are positive and "
           << neg_pixels.size() << " pixels are negative" << std::endl;
  std::cout << "positive sign are: " << positive_sign << std::endl;
  std::cout << "start ROC count..." << std::endl;
  // create threshold based on image range
  float min_val, max_val;
  vil_math_value_range(*in_img, min_val, max_val);
  std::vector<float> thresholds;
  unsigned num_thres = 200;
  float delta = (max_val-min_val) / num_thres;
  for (unsigned i = 0; i < num_thres; i++)
    thresholds.push_back(min_val + delta*i);
  const unsigned int num_thresholds = thresholds.size();

  std::cout << "image min: " << min_val << ", max: " << max_val << "\n threshold: ";
  for (unsigned i = 0; i < num_thresholds; i++)
    std::cout << thresholds[i] << ' ';
  std::cout << std::flush << std::endl;


  auto * tp=new bbas_1d_array_float(num_thresholds);
  auto * tn=new bbas_1d_array_float(num_thresholds);
  auto * fp=new bbas_1d_array_float(num_thresholds);
  auto * fn=new bbas_1d_array_float(num_thresholds);
  auto * tpr=new bbas_1d_array_float(num_thresholds);
  auto * fpr=new bbas_1d_array_float(num_thresholds);
  // initialize
  for (unsigned k = 0; k < num_thresholds; k++) {
    tp->data_array[k]  = 0;  tn->data_array[k]  = 0;
    fp->data_array[k]  = 0;  fn->data_array[k]  = 0;
    tpr->data_array[k] = 0;  fpr->data_array[k] = 0;
  }

  // count
  if (positive_sign == "low")
  {
    for (auto & pos_pixel : pos_pixels) {
      unsigned i = pos_pixel.first;  unsigned j = pos_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) <= thresholds[k] )
          tp->data_array[k] = tp->data_array[k]+1;
        else
          fn->data_array[k] = fn->data_array[k]+1;
      }
    }
    for (auto & neg_pixel : neg_pixels) {
      unsigned i = neg_pixel.first;  unsigned j = neg_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) <= thresholds[k])
          fp->data_array[k] = fp->data_array[k]+1;
        else
          tn->data_array[k] = tn->data_array[k]+1;
      }
    }
  }
  else if (positive_sign == "high")
  {
    for (auto & pos_pixel : pos_pixels) {
      unsigned i = pos_pixel.first;  unsigned j = pos_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) >= thresholds[k] )
          tp->data_array[k] = tp->data_array[k]+1;
        else
          fn->data_array[k] = fn->data_array[k]+1;
      }
    }
    for (auto & neg_pixel : neg_pixels) {
      unsigned i = neg_pixel.first;  unsigned j = neg_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) >= thresholds[k])
          fp->data_array[k] = fp->data_array[k]+1;
        else
          tn->data_array[k] = tn->data_array[k]+1;
      }
    }
  }
  else if (positive_sign == "equal")
  {
    for (auto & pos_pixel : pos_pixels) {
      unsigned i = pos_pixel.first;  unsigned j = pos_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) == thresholds[k])
          tp->data_array[k] = tp->data_array[k]+1;
        else
          fn->data_array[k] = fn->data_array[k]+1;
      }
    }
    for (auto & neg_pixel : neg_pixels) {
      unsigned i = neg_pixel.first;  unsigned j = neg_pixel.second;
      for (unsigned k = 0; k < num_thresholds; k++) {
        if ( (*in_img)(i,j) == thresholds[k])
          fp->data_array[k] = fp->data_array[k]+1;
        else
          tn->data_array[k] = tn->data_array[k]+1;
      }
    }
  }
  else {
    std::cerr << pro.name() << ": unknown positive sign -- " << positive_sign << "!\n";
    return false;
  }

  // calculate true positive rate and false positive rate
  for (unsigned pnt = 0; pnt < num_thresholds; pnt++)
  {
    tpr->data_array[pnt] = tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt] = fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
  }

  auto * thres_out=new bbas_1d_array_float(num_thresholds);
  for (unsigned k = 0; k < num_thresholds; k++) {
    thres_out->data_array[k] = thresholds[k];
  }

  // generate output
  unsigned out_i = 0;
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, thres_out);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(out_i++, fpr);

  return true;
}

//: process that takes a classification probability image of certain category and analyze its ROC,
//  given series of labeled positive and negative ground truth
bool sdet_texture_classifier_roc_process3_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("sdet_texture_classifier_sptr"); //texton dictionary, category names and colors are in the dictionary
  input_types.emplace_back("vil_image_view_base_sptr"); // classification output - prob values for each outputted category
  input_types.emplace_back("vcl_string"); // prefix for ground truth binary vsol spatial object files, <prefix>_<category_name>.bin
  input_types.emplace_back("vcl_string"); // name of the category to use as positive (the rest of the available categories will be used as negatives)
  if (!pro.set_input_types(input_types))
    return false;
  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_float_sptr");  // threshold
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tpr
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fpr
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red
  return pro.set_output_types(output_types);
}

bool sdet_texture_classifier_roc_process3(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!" << std::endl;
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  sdet_texture_classifier_sptr dict = pro.get_input<sdet_texture_classifier_sptr>(in_i++);
  vil_image_view<float> input_prob_img(pro.get_input<vil_image_view_base_sptr>(in_i++));
  std::string prefix = pro.get_input<std::string>(in_i++);
  std::string pos_category = pro.get_input<std::string>(in_i++);

  // load positive pixels
  std::string pos_file_name = prefix + "_" + pos_category + ".bin";
  std::vector<vgl_polygon<double> > positives = sdet_texture_classifier::load_polys(pos_file_name);
  std::cout << "there are " << positives.size() << " positive example polygons read!\n";

  // load the rest of labeled polygons as negative examples
  std::vector<std::string> cats = dict->get_dictionary_categories();
  std::vector<vgl_polygon<double> > negatives;
  std::cout << " using : ";
  for (auto & cat : cats) {
    if (cat.compare(pos_category) == 0)
      continue;
    std::string filename = prefix + "_" + cat + ".bin";
    if (!vul_file::exists(filename))
      continue;
    std::vector<vgl_polygon<double> > negs = sdet_texture_classifier::load_polys(filename);
    std::cout << cat << " ";
    for (const auto & neg : negs)
      negatives.push_back(neg);
  }
  std::cout << " total of " << negatives.size() << " polygons as negatives!\n";

  // go over the pixels and collect pixels of positives and negatives
  unsigned ni = input_prob_img.ni(); unsigned nj = input_prob_img.nj();
  std::vector<std::pair<int, int> > pos_pixels;
  std::vector<std::pair<int, int> > neg_pixels;
  for (const auto & positive : positives) {
    vgl_polygon_scan_iterator<double> psi(positive);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        pos_pixels.emplace_back(i,j);
      }
    }
  }
  for (const auto & negative : negatives) {
    vgl_polygon_scan_iterator<double> psi(negative);
    for (psi.reset(); psi.next(); ) {
      int j = psi.scany();
      for (int i  = psi.startx(); i <= psi.endx(); ++i) {
        if (i < 0 || j < 0 || i >= ni || j >= nj)
          continue;
        neg_pixels.emplace_back(i,j);
      }
    }
  }

  // start to compute tp, fn, tn, fp
  float min_val = 0.0f;
  float max_val = 1.0f;
  vil_math_value_range(input_prob_img, min_val, max_val);
  std::vector<float> thresholds;
  unsigned num_thres = 200;
  float delta = (max_val-min_val) / num_thres;
  for (unsigned i = 0; i <= num_thres; i++)
    thresholds.push_back(min_val + delta*i);
  const unsigned int num_thresholds = thresholds.size();
  std::cout << "image min: " << min_val << ", max: " << max_val << ", number of thresholds used: " << num_thresholds << std::endl;
  std::cout << "number of positive pixels: " << pos_pixels.size() << std::endl;
  std::cout << "number of negative pixels: " << neg_pixels.size() << std::endl;
  auto * tp=new bbas_1d_array_float(num_thresholds);
  auto * tn=new bbas_1d_array_float(num_thresholds);
  auto * fp=new bbas_1d_array_float(num_thresholds);
  auto * fn=new bbas_1d_array_float(num_thresholds);
  auto * tpr=new bbas_1d_array_float(num_thresholds);
  auto * fpr=new bbas_1d_array_float(num_thresholds);

  // initialize
  for (unsigned k = 0; k < num_thresholds; k++) {
    tp->data_array[k]  = 0.0f;  fp->data_array[k]  = 0.0f;
    tn->data_array[k]  = 0.0f;  fn->data_array[k]  = 0.0f;
    tpr->data_array[k] = 0.0f;  fpr->data_array[k] = 0.0f;
  }
  // count based on positive samples
  for (auto & pos_pixel : pos_pixels)
  {
    unsigned i = pos_pixel.first;
    unsigned j = pos_pixel.second;
    for (unsigned k = 0; k < num_thresholds; k++)
    {
      if (input_prob_img(i,j) >= thresholds[k])
        tp->data_array[k] += 1.0f;
      else
        fn->data_array[k] += 1.0f;
    }
  }
  // count based on negative samples
  for (auto & neg_pixel : neg_pixels)
  {
    unsigned i = neg_pixel.first;
    unsigned j = neg_pixel.second;
    for (unsigned k = 0; k < num_thresholds; k++) {
      if (input_prob_img(i,j) >= thresholds[k])
        fp->data_array[k] += 1.0f;
      else
        tn->data_array[k] += 1.0f;
    }
  }
  // compute tpr and fpr
  for (unsigned pnt = 0; pnt < num_thresholds; pnt++) {
    tpr->data_array[pnt] = tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt] = fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
  }
  auto * thres_out=new bbas_1d_array_float(num_thresholds);
  for (unsigned k = 0; k < num_thresholds; k++) {
    thres_out->data_array[k] = thresholds[k];
  }
  pro.set_output_val<bbas_1d_array_float_sptr>(0, thres_out);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(4, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(5, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(6, fpr);
  return true;
}
