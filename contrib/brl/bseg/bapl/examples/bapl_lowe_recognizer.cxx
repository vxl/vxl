// This is brl/bseg/bapl/examples/bapl_lowe_recognizer.cxx

//:
// \file

#include <iostream>
#include <vector>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_lowe_cluster.h>
#include <bapl/bapl_affine2d_est.h>

#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_ran_sam_search.h>
#include <rrel/rrel_muset_obj.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <ipts/ipts_draw.h>

#include <bapl/bapl_bbf_tree.h>

#if 0
void print_usage()
{
  std::cout<<"find_dog_peaks -i input_image -o out_image -d dog_image"<<std::endl;
}
#endif

int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path1("-i1","Input image path 1");
  vul_arg<std::string> in_path2("-i2","Input image path 2");
  //vul_arg<std::string> out_path("-o","Output image file (peaks)");
  vul_arg_parse(argc, argv);

  if (!in_path1.set() || !in_path2.set() )
  {
#if 0
    print_usage();
#endif
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> image1 = vil_convert_to_grey_using_rgb_weighting (vil_load(in_path1().c_str()));
  if (image1.ni()==0)
  {
    std::cout<<"Failed to load image1."<<std::endl;
    return 1;
  }

  vil_image_view<vxl_byte> image2 = vil_convert_to_grey_using_rgb_weighting (vil_load(in_path2().c_str()));
  if (image2.ni()==0)
  {
    std::cout<<"Failed to load image2."<<std::endl;
    return 1;
  }

  std::cout << "Finding Keypoints" << std::endl;

  std::vector< bapl_keypoint_sptr > keypoints1;
  vil_image_resource_sptr image1_sptr = vil_new_image_resource_of_view(image1);
  bapl_keypoint_extractor( image1_sptr, keypoints1);

  std::vector< bapl_keypoint_sptr > keypoints2;
  vil_image_resource_sptr image2_sptr = vil_new_image_resource_of_view(image2);
  bapl_keypoint_extractor( image2_sptr, keypoints2);

  bapl_bbf_tree bbf(keypoints1);

  int max_dim1 = (image1.ni()>image1.nj())?image1.ni():image1.nj();
  int max_dim2 = (image2.ni()>image2.nj())?image2.ni():image2.nj();
  bapl_lowe_clusterer clusterer(max_dim1, max_dim2, 10.0);

  for (const auto& query : keypoints2){
    std::vector<bapl_keypoint_sptr> matches;
    bbf.n_nearest(query, matches, 2, 50);
    if ( vnl_vector_ssd(query->descriptor(),matches[0]->descriptor()) <
        vnl_vector_ssd(query->descriptor(),matches[1]->descriptor())*.8){
      std::cout << "Matched!" << std::endl;
      bapl_lowe_keypoint_sptr kp1, kp2;
      kp1.vertical_cast(matches[0]);
      kp2.vertical_cast(query);
      clusterer.hash(bapl_keypoint_match(kp1, kp2));
#if 0
      ipts_draw_cross(image1, int(kp1->location_i()+0.5), int(kp1->location_j()+0.5),
                      unsigned(kp1->scale()+0.5), (vxl_byte)255);
      ipts_draw_cross(image2, int(kp2->location_i()+0.5), int(kp2->location_j()+0.5),
                      unsigned(kp2->scale()+0.5), (vxl_byte)255);
      for ( int i = 0; i<matches.size(); ++i)
        std::cout << "sq_dist: "<< vnl_vector_ssd(query->descriptor(),matches[i]->descriptor()) << std::endl;
#endif
    }
  }

  std::vector< std::vector < bapl_keypoint_match > > clusters = clusterer.get_sorted_clusters();
#ifdef DEBUG
  std::cout <<  "size:" <<cluster.size() <<std::endl;
  for ( int i=0; i<clusters.size(); ++i){
    if ( clusters[i].size() > 2 )
      std::cout << "count: "<<clusters[i].size() << std::endl;
  }
#endif

  // Construct the estimation problem
  auto * est = new bapl_affine2d_est( clusters[0] );

  //double max_outlier_frac = 0.5;
  double desired_prob_good = 0.99;
  int max_pops = 1;
  int trace_level = 0;

  est->set_no_prior_scale();

  auto* muset = new rrel_muset_obj( clusters[0].size()+1 );
  auto * ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(trace_level);
  ransam->set_sampling_params( 1 - muset->min_inlier_fraction(),
                               desired_prob_good,
                               max_pops);

  if ( !ransam->estimate( est, muset ) )
    std::cout << "MUSE failed!!\n";
  else {
    std::cout << "MUSE succeeded.\n"
             << "estimate = " << ransam->params() << '\n'
             << "scale = " << ransam->scale() << std::endl;
  }
  std::cout << std::endl;
  delete muset;
  delete ransam;

#if 0
  int num_dep_res = est->num_samples_to_instantiate();
  rrel_objective* lms = new rrel_lms_obj( num_dep_res );

  rrel_ran_sam_search* ransam = new rrel_ran_sam_search;
  ransam->set_trace_level(trace_level);
  ransam->set_sampling_params( max_outlier_frac, desired_prob_good, max_pops);

  if ( !ransam->estimate( est, lms ) )
    std::cout << "LMS failed!!\n";
  else {
    std::cout << "LMS succeeded.\n"
             << "estimate = " << ransam->params() << '\n'
             << "scale = " << ransam->scale() << std::endl;
  }
  std::cout << std::endl;
  delete lms;
  delete ransam;
#endif
  delete est;

#if 0
  vil_save(image1, "/home/mleotta/vision/out1.png");
  vil_save(image2, "/home/mleotta/vision/out2.png");
  std::vector<bapl_keypoint_sptr> matches;
  bbf.n_nearest(query, matches, 10);

  for ( std::vector< bapl_keypoint_sptr >::iterator itr = keypoints.begin();
        itr != keypoints.end();  ++itr ) {
    (*itr)->print_summary(std::cout);
    std::cout << (*itr)->descriptor() << std::endl;
  }
#endif

  std::cout <<  "done!" <<std::endl;
  return 0;
}
