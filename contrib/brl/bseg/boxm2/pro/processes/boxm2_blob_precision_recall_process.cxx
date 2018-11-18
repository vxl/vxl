// This is brl/bseg/boxm2/pro/processes/boxm2_blob_precision_recall_process.cxx
#include <iostream>
#include <algorithm>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file

#include <boxm2/util/boxm2_detect_change_blobs.h>
#include <bil/algo/bil_blob_finder.h>
#include <bprb/bprb_parameters.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <bbas_pro/bbas_1d_array_float.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace boxm2_blob_precision_recall_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 4;

  // do pixelwise sort on the image, and then
  struct Pair {
    float change;
    bool  gt;
  };
  bool pair_sorter(Pair const& lhs, Pair const& rhs) { return lhs.change < rhs.change; }
}


//: Constructor
bool boxm2_blob_precision_recall_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_blob_precision_recall_process_globals;

  // this process takes 3 inputs:
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr");  // image
  input_types.emplace_back("vil_image_view_base_sptr");  // ground truth map
  input_types.emplace_back("vil_image_view_base_sptr");  // mask image
  if (! pro.set_input_types(input_types))
    return false;

  // default arguments
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(2, empty_mask);

  // this process takes 4 outputs:
  std::vector<std::string> output_types;
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // tn
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  output_types.emplace_back("bbas_1d_array_float_sptr");  // fp
  return pro.set_output_types(output_types);
}

//: Execute the process
bool boxm2_blob_precision_recall_process(bprb_func_process& pro)
{
  using namespace boxm2_blob_precision_recall_process_globals;

  // Sanity check
  if (pro.n_inputs() < n_inputs_) {
    std::cerr << "boxm2_blob_precision_recall_process: The number of inputs should be 2 (with optional 3rd (num thresh) and 4th (mask image))\n";
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr detection_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr ground_truth_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr mask_map_sptr = pro.get_input<vil_image_view_base_sptr>(i++);

  // catch a "null" mask (not really null because that throws an error)
  bool use_mask = true;
  if (mask_map_sptr->ni()==1 && mask_map_sptr->nj()==1) {
    std::cout<<"USE mask = false"<<std::endl;
    use_mask = false;
  }

  // true positive, true negative, false positive, false negative
  constexpr unsigned int numPoints = 100;
  auto * precision = new bbas_1d_array_float(numPoints);
  auto * recall = new bbas_1d_array_float(numPoints);
  vil_image_view<float> * detection_map;

  // check bounds to make sure they match
  if (detection_map_sptr->ni() != ground_truth_map_sptr->ni() ||
      detection_map_sptr->nj() != ground_truth_map_sptr->nj() ) {
    std::cout<<"boxm2_blob_precision_recall_process:: detection map doesn't match ground truth map"<<std::endl;
    return false;
  }
  if (use_mask) {
    if (detection_map_sptr->ni()!=mask_map_sptr->ni() ||
        detection_map_sptr->nj()!=mask_map_sptr->nj() ) {
      std::cout<<"boxm2_blob_precision_recall_process:: detection map doesn't match mask map"<<std::endl;
      return false;
    }
  }

  // convert detection map to [0,1] float
  if (auto * detection_map_uchar=dynamic_cast<vil_image_view<unsigned char> *>(detection_map_sptr.ptr())) {
    detection_map =new vil_image_view<float>(detection_map_uchar->ni(),detection_map_uchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_uchar,*detection_map,0,255,0.0f,1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr())) {
    detection_map=dynamic_cast<vil_image_view<float>*>(detection_map_sptr.ptr());
  }
  else {
    std::cout<<"Detection Map cannot be converted to float image"<<std::endl;
    return false;
  }

  // cast to usable image views
  auto * gt_uchar = dynamic_cast<vil_image_view<vxl_byte> *>(ground_truth_map_sptr.ptr());
  if ( !gt_uchar ) {
    std::cout<<"boxm2_blob_precision_recall_process:: gt map is not an unsigned char map"<<std::endl;
    return false;
  }
  auto * mask_map=dynamic_cast<vil_image_view<unsigned char> *>(mask_map_sptr.ptr());
  if (!mask_map) {
    std::cout<<"boxm2_blob_precision_recall_process:: mask map is not an unsigned char map"<<std::endl;
    return false;
  }

  //create boolean gtmap
  vil_image_view<bool>  gt_map(gt_uchar->ni(), gt_uchar->nj());
  for (unsigned int i=0; i<gt_uchar->ni(); ++i)
    for (unsigned int j=0; j<gt_uchar->nj(); ++j)
      gt_map(i,j) = (*gt_uchar)(i,j) == 0 ? false : true;

#if 0
  // sort pixel/gt pairs
  Pair* pairs = new Pair[ detection_map->ni() * detection_map->nj() ];
  unsigned c = 0;
  for (unsigned j=0; j<detection_map->nj(); ++j) {
    for (unsigned i=0; i<detection_map->ni(); ++i) {
      Pair p;
      p.change = (*detection_map)(i,j);
      p.gt = gt_map(i,j);
      pairs[c] = p;
      ++c;
    }
  }
  unsigned totPix = detection_map->ni() * detection_map->nj();
  std::sort(pairs, pairs + totPix, &pair_sorter);
#endif // 0

  //grab thresholds by evenly dispersing them through examples
  auto* thresholds = new float[numPoints];
  float  incr = 1.0f/(float)numPoints; // = totPix / numPoints;
  for (unsigned int i=0; i<numPoints; ++i) {
    thresholds[i] = i*incr + .01f; //pairs[i*incr].change;
#ifdef DEBUG
    std::cout<<" thresh "<<i<<": "<<thresholds[i]<<',';
#endif
  }
#ifdef DEBUG
  std::cout<<std::endl;
#endif

  //run blob finder on ground truth image just once
  std::vector<boxm2_change_blob> gt_blobs;
  boxm2_util_detect_blobs( gt_map, gt_blobs);
  std::cout<<"detected "<< gt_blobs.size() <<" blobs in ground truth"<<std::endl;

  //for each threshold detect blobs and calc #tps, fps, tns, fns
  for (unsigned int pnt=0; pnt<numPoints; ++pnt) {
    float true_positives = 0.0f;

    //detect change blobs
    std::vector<boxm2_change_blob> blobs;
    boxm2_util_detect_change_blobs( *detection_map, thresholds[pnt], blobs );
    std::cout<<"  thresh "<<thresholds[pnt]<<" detected "<<blobs.size()<<" blobs"<<std::endl;

    vil_image_view<vxl_byte> blbImage(gt_map.ni(), gt_map.nj());
    boxm2_util_blob_to_image(blobs, blbImage);
    std::stringstream fname;
    fname<<"blobImage_"<<thresholds[pnt]<<".png";
    vil_save(blbImage, fname.str().c_str());

    //cross check each ground truth blob against change blobs for coverage
    for (auto & gt_blob : gt_blobs) {
      for (auto & blob : blobs) {
        if ( gt_blob.percent_overlap( blob ) > .25f )
          true_positives++;
      }
    }
    std::cout<<" num true positives: "<<true_positives<<std::endl;

    //set precision and recall
    precision->data_array[pnt] = blobs.size()==0 ? 0 : true_positives / blobs.size();
    recall->data_array[pnt] = true_positives / gt_blobs.size();
  }

  // set outputs
  if (pro.n_outputs() < n_outputs_) {
    std::cerr << "boxm2_blob_precision_recall_process: The number of outputs should be "<<n_outputs_<<'\n';
    return false;
  }
  pro.set_output_val<bbas_1d_array_float_sptr>(0, precision);
  pro.set_output_val<bbas_1d_array_float_sptr>(1, recall);
  return true;
}
