// This is brl/bpro/core/vil_pro/processes/vil_pixelwise_roc_process.cxx
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <bbas_pro/bbas_1d_array_float.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// do pixelwise sort on the image, and then
struct Pair
{
  float         change;
  unsigned char gt;
  unsigned i;
  unsigned j;
};

bool pair_sorter(Pair const& lhs, Pair const& rhs)
{
  return lhs.change < rhs.change;
}

bool pair_sorter2(Pair const& lhs, Pair const& rhs)
{
  return lhs.change > rhs.change;
}


//: Constructor
bool vil_pixelwise_roc_process_cons(bprb_func_process& pro)
{
  // this process takes 4 inputs, 2 of which are optional:
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // change image
  input_types.emplace_back("vil_image_view_base_sptr");  // ground truth map
  input_types.emplace_back("vil_image_view_base_sptr");  // mask image
  input_types.emplace_back("bool");                      // if true use pair_sorter, otherwise use pair_sorter2, if want low values in change image to signify true classification use pair_sorter
  if (! pro.set_input_types(input_types))
    return false;

  // default arguments
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(2, empty_mask);

  brdb_value_sptr idx = new brdb_value_t<bool>(true);
  pro.set_input(3, idx);

  // this process takes 7 outputs:
  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tpr
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fpr
  output_types.emplace_back("vil_image_view_base_sptr");  // output image with pixels given by threshold of 0.8 tpr marked red

  return pro.set_output_types(output_types);
}

//: Execute the process
bool vil_pixelwise_roc_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << "vil_pixelwise_roc_process: The number of inputs should be 2 (with optional 3rd (num thresh) and 4th (mask image))\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr detection_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  bool use_pair_sorter = pro.get_input<bool>(i++);

  // catch a "null" mask (not really null because that throws an error)
  bool use_mask = true;
  if (mask_map_sptr->ni()==1 && mask_map_sptr->nj()==1) {
    std::cout<<"USE mask = false"<<std::endl;
    use_mask = false;
  }

  // true positive, true negative, false positive, false negative
  constexpr unsigned int numPoints = 10000;
  auto * tp=new bbas_1d_array_float(numPoints);
  auto * tn=new bbas_1d_array_float(numPoints);
  auto * fp=new bbas_1d_array_float(numPoints);
  auto * fn=new bbas_1d_array_float(numPoints);
  vil_image_view<float> * detection_map;

  // check bounds to make sure they match
  if (detection_map_sptr->ni() != ground_truth_map_sptr->ni() ||
      detection_map_sptr->nj() != ground_truth_map_sptr->nj() ) {
    std::cout<<"vil_pixelwise_roc_process:: detection map doesn't match ground truth map"<<std::endl;
    return false;
  }
  if (use_mask) {
    if (detection_map_sptr->ni()!=mask_map_sptr->ni() ||
        detection_map_sptr->nj()!=mask_map_sptr->nj() ) {
      std::cout<<"vil_pixelwise_roc_process:: detection map doesn't match mask map"<<std::endl;
      return false;
    }
  }

  // convert detection map to [0,1] float
  if (auto * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr()))
  {
    detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr()))
  {
    detection_map=dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr());
    std::cout << "detection map is float!\n";
  }
  else
  {
    std::cout<<"Detection Map cannot be converted to float image"<<std::endl;
    return false;
  }

  // cast to usable image views
  auto * ground_truth_map = dynamic_cast<vil_image_view<unsigned char> *>(ground_truth_map_sptr.ptr());
  if ( !ground_truth_map )
  {
    std::cout<<"vil_pixelwise_roc_process:: gt map is not an unsigned char map"<<std::endl;
    return false;
  }
  auto * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr());
  if (!mask_map)
  {
    std::cout<<"vil_pixelwise_roc_process:: mask map is not an unsigned char map"<<std::endl;
    return false;
  }

  // sort pixel/gt pairs
  //Pair* pairs = new Pair[ detection_map->ni() * detection_map->nj() ];
  std::vector<Pair> pairs;
  unsigned c = 0;
  for (unsigned j=0; j<detection_map->nj(); ++j) {
    for (unsigned i=0; i<detection_map->ni(); ++i) {
      if (use_mask && (*mask_map)(i,j) == 0)
        continue;
      if(  (*ground_truth_map)(i,j) == 0 || (*ground_truth_map)(i,j) == 255)
      {
      Pair p;
      p.change = (*detection_map)(i,j);
      p.gt = (*ground_truth_map)(i,j);
      p.i = i;
      p.j = j;
      //pairs[c] = p;
      pairs.push_back(p);
      ++c;
      }
    }
  }

  //
  //unsigned totPix = detection_map->ni() * detection_map->nj();
  unsigned totPix = pairs.size();
  //std::sort(pairs, pairs + totPix, &pair_sorter);
  if (use_pair_sorter)
    std::sort(pairs.begin(), pairs.end(), &pair_sorter);
  else
    std::sort(pairs.begin(), pairs.end(), &pair_sorter2);

  // grab 100 points for the ROC curve
  unsigned int incr = totPix / numPoints;
  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
     if (pnt%100 == 0 )
         std::cout<<".";
    tp->data_array[pnt]=0.0f;
    fp->data_array[pnt]=0.0f;
    tn->data_array[pnt]=0.0f;
    fn->data_array[pnt]=0.0f;

    unsigned int exampleIdx = pnt*incr;

    // all classified examples in this loop are negative
    for (unsigned int i=0; i<exampleIdx; ++i) {
      bool truth = (pairs[i].gt == 255);
      bool ignore= (pairs[i].gt > 0 && pairs[i].gt < 255);
      if(!ignore)
      {
      if (truth)
      {
       fn->data_array[pnt]++;
      }// gt=true, class=false => false neg
      else
      {
        tn->data_array[pnt]++;
       }
     }      // gt=false, class=false => true neg
    }

    // all classified examples in this loop are positive
    for (unsigned int i=exampleIdx; i<totPix; ++i) {
      bool truth = (pairs[i].gt == 255);
      bool ignore= (pairs[i].gt > 0 && pairs[i].gt < 255);
      if(!ignore) {
        if (truth ) {
          tp->data_array[pnt]++; // gt = true, class = true => true pos
        }
        else
          fp->data_array[pnt]++; // gt = false, class = true => false pos
        }
    }
  }

  auto * tpr=new bbas_1d_array_float(numPoints);
  auto * fpr=new bbas_1d_array_float(numPoints);

  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
    tpr->data_array[pnt]= tp->data_array[pnt] / (tp->data_array[pnt] + fn->data_array[pnt]);
    fpr->data_array[pnt]= fp->data_array[pnt] / (fp->data_array[pnt] + tn->data_array[pnt]);
  }
  //: find the point when tpr > 0.8
  //float change = 0.0f;
  //for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
  //  if (tpr->data_array[pnt] > 0.8f) {
  //    unsigned int exampleIdx = pnt*incr;
  //    change = pairs[exampleIdx].change;
  //    break;
  //  }
  //}

  auto* temp = new vil_image_view<vxl_byte>(detection_map->ni(), detection_map->nj(), 3);
  temp->fill(0);

  //for (unsigned k = 0; k < totPix; ++k) {
  //  unsigned i = pairs[k].i;
  //  unsigned j = pairs[k].j;
  //  if (pairs[k].change >= change) {
  //    (*temp)(i,j,1) = 0;
  //    (*temp)(i,j,2) = 0;
  //    (*temp)(i,j,0) = 255;
  //  }
  //  else {
  //    (*temp)(i,j,1) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //    (*temp)(i,j,2) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //    (*temp)(i,j,0) = 255*(*detection_map)(i,j) > 255 ? 255 : (vxl_byte)(255*(*detection_map)(i,j));
  //  }
  //}

  pro.set_output_val<bbas_1d_array_float_sptr>(0, tp);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, tn);
  pro.set_output_val<bbas_1d_array_float_sptr>(2, fp);
  pro.set_output_val<bbas_1d_array_float_sptr>(3, fn);
  pro.set_output_val<bbas_1d_array_float_sptr>(4, tpr);
  pro.set_output_val<bbas_1d_array_float_sptr>(5, fpr);
  pro.set_output_val<vil_image_view_base_sptr>(6, temp);

  return true;
}

//: process to compute ROC curve of classified probability image with respect to ground truth image
//  Note that the operational threshold ranges from min and max of the input probability image.
//  Both ground truth image are binary mask with 0 and 255.
//  If negative ground truth image is not given, then the negative ground truth are obtained from zero pixels in positive ground truth image
namespace vil_pixelwise_roc_process2_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 7;
}

bool vil_pixelwise_roc_process2_cons(bprb_func_process& pro)
{
  using namespace vil_pixelwise_roc_process2_globals;
  // this process takes 5 inputs, mask image and positive sign is optional
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // probability image
  input_types_[1] = "vil_image_view_base_sptr";  // positive ground truth image
  input_types_[2] = "vil_image_view_base_sptr";  // negative ground truth image
  input_types_[3] = "vil_image_view_base_sptr";  // mask image
  input_types_[4] = "vcl_string";                // option to define ground truth being lower than threshold or higher than threshold
  // this process takes 5 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_float_sptr";  // threshold array
  output_types_[1] = "bbas_1d_array_float_sptr";  // tp
  output_types_[2] = "bbas_1d_array_float_sptr";  // tn
  output_types_[3] = "bbas_1d_array_float_sptr";  // fp
  output_types_[4] = "bbas_1d_array_float_sptr";  // fp
  output_types_[5] = "bbas_1d_array_float_sptr";  // tpr
  output_types_[6] = "bbas_1d_array_float_sptr";  // fpr
  // default arguments
  brdb_value_sptr empty_neg_gt = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(2, empty_neg_gt);
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(3, empty_mask);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vil_pixelwise_roc_process2(bprb_func_process& pro)
{
  using namespace vil_pixelwise_roc_process2_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr detection_map_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr neg_gt_map_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr mask_map_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  std::string positive_sign = pro.get_input<std::string>(in_i++);

  // catch a "null" mask (not really null because that throws an error)
  bool use_mask = true;
  if (mask_map_sptr->ni()==1 && mask_map_sptr->nj()==1) {
    std::cout << "USE mask = false" << std::endl;
    use_mask = false;
  }

  bool is_neg_gt = true;
  if (neg_gt_map_sptr->ni() == 1 && neg_gt_map_sptr->nj() == 1) {
    std::cout << "negative ground truth exist = false" << std::endl;
    is_neg_gt = false;
  }

  // check bounds to make sure they match
  if (detection_map_sptr->ni() != ground_truth_map_sptr->ni() || detection_map_sptr->nj() != ground_truth_map_sptr->nj() ) {
    std::cerr << pro.name() << ": detection map doesn't match ground truth map" << std::endl;
    return false;
  }
  if (use_mask) {
    if (detection_map_sptr->ni()!=mask_map_sptr->ni() || detection_map_sptr->nj()!=mask_map_sptr->nj() ) {
      std::cerr << pro.name() << ": detection map doesn't match mask map" << std::endl;
      return false;
    }
  }
  if (is_neg_gt) {
    if (detection_map_sptr->ni() != neg_gt_map_sptr->ni() || detection_map_sptr->nj() != neg_gt_map_sptr->nj() ) {
      std::cerr << pro.name() << ": detection map doesn't match negative ground truth map" << std::endl;
      return false;
    }
    if (ground_truth_map_sptr->ni() != neg_gt_map_sptr->ni() || ground_truth_map_sptr->nj() != neg_gt_map_sptr->nj()) {
      std::cerr << pro.name() << ": positive ground truth map doesn't match negative ground truth map" << std::endl;
      return false;
    }
    if (use_mask) {
      if (neg_gt_map_sptr->ni() != mask_map_sptr->ni() || neg_gt_map_sptr->nj() != mask_map_sptr->nj()) {
        std::cerr << pro.name() << ": negative ground truth map doesn't match mask map" << std::endl;
        return false;
      }
    }
  }

  // convert detection map to [0,1] float
  vil_image_view<float> * detection_map;
  if (auto * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr()))
  {
    detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr()))
  {
    detection_map=dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr());
    std::cout << "detection map is float!\n";
  }
  else
  {
    std::cerr << pro.name() << ": detection map cannot be converted to float image" << std::endl;
    return false;
  }

  // cast to usable image views
  auto * gt_map = dynamic_cast<vil_image_view<unsigned char> *>(ground_truth_map_sptr.ptr());
  if ( !gt_map )
  {
    std::cerr << pro.name() << ": gt map is not an unsigned char map" << std::endl;
    return false;
  }
  auto * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr());
  if (!mask_map)
  {
    std::cerr << pro.name() << ": mask map is not an unsigned char map" << std::endl;
    return false;
  }
  auto * neg_gt_map = dynamic_cast<vil_image_view<unsigned char>*>(neg_gt_map_sptr.ptr());
  if (!neg_gt_map) {
    std::cerr << pro.name() << ": negative ground truth map is not an unsigned char map" << std::endl;
    return false;
  }

  // go over the ground truth image to collect positive and negative pixels
  unsigned ni = gt_map->ni();
  unsigned nj = gt_map->nj();
  std::vector<std::pair<unsigned, unsigned> > pos_pixels;
  std::vector<std::pair<unsigned, unsigned> > neg_pixels;
  std::cout << "Collect ground truth positive and negative pixels..." << std::flush << std::endl;
  if (is_neg_gt)
  {
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        if (use_mask && (*mask_map)(i,j) == 0)
          continue;
        if ((*gt_map)(i,j))
          pos_pixels.emplace_back(i,j);
        if ((*neg_gt_map)(i,j))
          neg_pixels.emplace_back(i,j);
      }
    }
  }
  else
  {
    for (unsigned i = 0; i < ni; i++) {
      for (unsigned j = 0; j < nj; j++) {
        if (use_mask && (*mask_map)(i,j) == 0)
          continue;
        if ((*gt_map)(i,j))
          pos_pixels.emplace_back(i,j);
        else
          neg_pixels.emplace_back(i,j);
      }
    }
  }


  std::cout << "In ground truth map, " << pos_pixels.size() << " pixels are positive and " << neg_pixels.size() << " are negative." << std::endl;

  // create threshold based on image range
  float min_val, max_val;
  vil_math_value_range(*detection_map, min_val, max_val);
  std::vector<float> thresholds;
  unsigned num_thres = 200;
  float delta = (max_val - min_val) / num_thres;
  for (unsigned i = 0; i <= (num_thres+1); i++) {
    thresholds.push_back(min_val + delta*i);
  }
  const unsigned n_thres = thresholds.size();
  std::cout << "Start ROC count using " << n_thres << " thresholds, ranging from " << min_val << " to " << max_val << "..." << std::endl;

  auto* tp = new bbas_1d_array_float(n_thres);
  auto* tn = new bbas_1d_array_float(n_thres);
  auto* fp = new bbas_1d_array_float(n_thres);
  auto* fn = new bbas_1d_array_float(n_thres);
  auto* tpr = new bbas_1d_array_float(n_thres);
  auto* fpr = new bbas_1d_array_float(n_thres);

  // initialize
  for (unsigned i = 0; i < n_thres; i++) {
    tp->data_array[i] = 0.0f;  tn->data_array[i] = 0.0f;
    fp->data_array[i] = 0.0f;  fn->data_array[i] = 0.0f;
    tpr->data_array[i] = 0.0f;
    fpr->data_array[i] = 0.0f;
  }

  // count
  if (positive_sign == "high")
  {
    for (auto & pos_pixel : pos_pixels)
    {
      unsigned i = pos_pixel.first;
      unsigned j = pos_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) >= thresholds[tidx] )  // ground truth is positive, detection is true  --> true positive
          tp->data_array[tidx] += 1;
        else                                              // ground truth is positive, detection is false --> false negative
          fn->data_array[tidx] += 1;
      }
    }
    for (auto & neg_pixel : neg_pixels)
    {
      unsigned i = neg_pixel.first;
      unsigned j = neg_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) >= thresholds[tidx] )  // ground truth is negative, detection is true  --> false positive
          fp->data_array[tidx] += 1;
        else                                              // ground truth is negative, detection is false --> true negative
          tn->data_array[tidx] += 1;
      }
    }
  }
  else if (positive_sign == "low")
  {
    for (auto & pos_pixel : pos_pixels)
    {
      unsigned i = pos_pixel.first;
      unsigned j = pos_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) <= thresholds[tidx] )  // ground truth is positive, detection is true  --> true positive
          tp->data_array[tidx] += 1;
        else                                              // ground truth is positive, detection is false --> false negative
          fn->data_array[tidx] += 1;
      }
    }
    for (auto & neg_pixel : neg_pixels)
    {
      unsigned i = neg_pixel.first;
      unsigned j = neg_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) <= thresholds[tidx] )  // ground truth is negative, detection is true  --> false positive
          fp->data_array[tidx] += 1;
        else                                              // ground truth is negative, detection is false --> true negative
          tn->data_array[tidx] += 1;
      }
    }
  }
  else if (positive_sign == "equal")
  {
    for (auto & pos_pixel : pos_pixels)
    {
      unsigned i = pos_pixel.first;
      unsigned j = pos_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) == thresholds[tidx] )  // ground truth is positive, detection is true  --> true positive
          tp->data_array[tidx] += 1;
        else                                              // ground truth is positive, detection is false --> false negative
          fn->data_array[tidx] += 1;
      }
    }
    for (auto & neg_pixel : neg_pixels)
    {
      unsigned i = neg_pixel.first;
      unsigned j = neg_pixel.second;
      for (unsigned tidx = 0; tidx < n_thres; tidx++)
      {
        if ( (*detection_map)(i,j) == thresholds[tidx] )  // ground truth is negative, detection is true  --> false positive
          fp->data_array[tidx] += 1;
        else                                              // ground truth is negative, detection is false --> true negative
          tn->data_array[tidx] += 1;
      }
    }
  }
  else {
    std::cerr << pro.name() << ": Unknown positive sign -- " << positive_sign << ", only 'high', 'low', 'equal' are allowed!\n";
    return false;
  }

  // calculate true positive rate and false positive rate
  for (unsigned tidx = 0; tidx < n_thres; tidx++) {
    tpr->data_array[tidx] = tp->data_array[tidx] / (tp->data_array[tidx] + fn->data_array[tidx]);
    fpr->data_array[tidx] = fp->data_array[tidx] / (fp->data_array[tidx] + tn->data_array[tidx]);
  }

  auto * thres_out=new bbas_1d_array_float(n_thres);
  for (unsigned k = 0; k < n_thres; k++) {
    thres_out->data_array[k] = thresholds[k];
  }

  // output
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
