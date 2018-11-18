// This is brl/bseg/bapl/pro/processes/bapl_match_keypoints_process.cxx

//:
// \file
// \brief A class to match two sets of keypoints extracted from two images
//
// \author Ozge Can Ozcanli
// \date September 16, 2010
//
// \verbatim
//  Modifications
//    none
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_bbf_tree.h>
#include <bapl/bapl_keypoint_set.h>
#include <bapl/bapl_keypoint_set_sptr.h>

#include <vil/vil_plane.h>
#include <vnl/vnl_random.h>

#include <ipts/ipts_draw.h>

#include <vul/vul_timer.h>

//: Constructor
bool bapl_match_keypoints_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bapl_keypoint_set_sptr");    // first set of keypoints
  input_types.emplace_back("bapl_keypoint_set_sptr");    // second set of keypoints
  input_types.emplace_back("int");   // id of the first image, left image
  input_types.emplace_back("int");   // id of the second image, right image
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("bapl_keypoint_match_set_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_match_keypoints_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cout << "bapl_match_keypoints_process: The input number should be 4" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  bapl_keypoint_set_sptr set1 = pro.get_input<bapl_keypoint_set_sptr>(i++);
  bapl_keypoint_set_sptr set2 = pro.get_input<bapl_keypoint_set_sptr>(i++);
  int id1 = pro.get_input<int>(i++);
  int id2 = pro.get_input<int>(i++);

  vul_timer t;

  bapl_bbf_tree bbf(set2->keys_, 16);  // create a kd-tree for features of J (second image)
  std::vector<bapl_keypoint_sptr>& keypoints1 = set1->keys_;

  std::vector<bapl_key_match> matches;

  for (auto query : keypoints1) {  // for each feature in I (first image)
    std::vector<bapl_keypoint_sptr> match;
    bbf.n_nearest(query, match, 2, 200);       // find its two nearest neighbors, 200 is parameter value used in bundler package
    if ( vnl_vector_ssd(query->descriptor(),match[0]->descriptor()) <
         vnl_vector_ssd(query->descriptor(),match[1]->descriptor())*.6*.6) {   // 0.6*.6 is parameter value used in bundler package
      bapl_key_match k_p(query, match[0]);
      matches.push_back(k_p);                      // accepted match
    }
  }

  std::vector<bapl_key_match> matches_pruned(matches);
  bapl_keypoint_match_set::prune_spurious_matches(matches_pruned);
  std::cout << "After pruning found: " << matches_pruned.size() << " matches, whole process took " << t.real()/(1000.0*60.0) << " mins.\n";

  bapl_keypoint_match_set_sptr key_set = new bapl_keypoint_match_set(id1, id2, matches_pruned);
  pro.set_output_val<bapl_keypoint_match_set_sptr>(0, key_set);

  return true;
}

//: Constructor
bool bapl_match_display_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image 1
  input_types.emplace_back("vil_image_view_base_sptr"); // input image 2
  input_types.emplace_back("bapl_keypoint_match_set_sptr");    // key matches
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr"); // out img1 with keys marked same color
  output_types.emplace_back("vil_image_view_base_sptr"); // out img2
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_match_display_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cout << "bapl_match_display_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr input_image1_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image1(input_image1_sptr);

  vil_image_view<vxl_byte> output_img1(input_image1.ni(), input_image1.nj(), 3);
  vil_image_view<vxl_byte> output_img1_r = vil_plane(output_img1, 0);
  vil_image_view<vxl_byte> output_img1_g = vil_plane(output_img1, 1);
  vil_image_view<vxl_byte> output_img1_b = vil_plane(output_img1, 2);
  if (input_image1.nplanes() == 3) {
    vil_image_view<vxl_byte> input_img1_r = vil_plane(input_image1, 0);
    vil_image_view<vxl_byte> input_img1_g = vil_plane(input_image1, 1);
    vil_image_view<vxl_byte> input_img1_b = vil_plane(input_image1, 2);
    output_img1_r.deep_copy(input_img1_r);
    output_img1_g.deep_copy(input_img1_g);
    output_img1_b.deep_copy(input_img1_b);
  }
  else {
    output_img1_r.deep_copy(input_image1);
    output_img1_g.deep_copy(input_image1);
    output_img1_b.deep_copy(input_image1);
  }

  vil_image_view_base_sptr input_image2_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image2(input_image2_sptr);
  vil_image_view<vxl_byte> output_img2(input_image2.ni(), input_image2.nj(), 3);
  vil_image_view<vxl_byte> output_img2_r = vil_plane(output_img2, 0);
  vil_image_view<vxl_byte> output_img2_g = vil_plane(output_img2, 1);
  vil_image_view<vxl_byte> output_img2_b = vil_plane(output_img2, 2);
  if (input_image2.nplanes() == 3) {
    vil_image_view<vxl_byte> input_img2_r = vil_plane(input_image2, 0);
    vil_image_view<vxl_byte> input_img2_g = vil_plane(input_image2, 1);
    vil_image_view<vxl_byte> input_img2_b = vil_plane(input_image2, 2);
    output_img2_r.deep_copy(input_img2_r);
    output_img2_g.deep_copy(input_img2_g);
    output_img2_b.deep_copy(input_img2_b);
  }
  else {
    output_img2_r.deep_copy(input_image2);
    output_img2_g.deep_copy(input_image2);
    output_img2_b.deep_copy(input_image2);
  }


  bapl_keypoint_match_set_sptr match_set = pro.get_input<bapl_keypoint_match_set_sptr>(i++);

  std::vector<bapl_key_match>& matches = match_set->matches_;

  vnl_random rng;
  for (const auto& m : matches) {
    bapl_lowe_keypoint_sptr kp1;
    kp1.vertical_cast(m.first);
    bapl_lowe_keypoint_sptr kp2;
    kp2.vertical_cast(m.second);

    auto color_r = (vxl_byte)rng.lrand32(0,255);
    auto color_g = (vxl_byte)rng.lrand32(0,255);
    auto color_b = (vxl_byte)rng.lrand32(0,255);
    int ii = int(kp1->location_i()+0.5); int jj = int(kp1->location_j()+0.5);
    if (ii >= 0 && jj >= 0 && ii < (int)input_image1.ni() && jj < (int)input_image1.nj()) {
      //ipts_draw_cross(output_img1, ii,jj,int(kp1->scale()+0.5), vxl_byte(255) );
      ipts_draw_cross(output_img1_r, ii, jj, int(kp1->scale()+0.5), color_r);
      ipts_draw_cross(output_img1_g, ii, jj, int(kp1->scale()+0.5), color_g);
      ipts_draw_cross(output_img1_b, ii, jj, int(kp1->scale()+0.5), color_b);
    }

    ii = int(kp2->location_i()+0.5); jj = int(kp2->location_j()+0.5);
    if (ii >= 0 && jj >= 0 && ii < (int)input_image2.ni() && jj < (int)input_image2.nj()) {
      //ipts_draw_cross(output_img2, ii,jj,int(kp2->scale()+0.5), vxl_byte(255) );
      ipts_draw_cross(output_img2_r, ii, jj, int(kp2->scale()+0.5), color_r);
      ipts_draw_cross(output_img2_g, ii, jj, int(kp2->scale()+0.5), color_g);
      ipts_draw_cross(output_img2_b, ii, jj, int(kp2->scale()+0.5), color_b);
    }
  }

  vil_image_view_base_sptr output_img1_sptr = new vil_image_view<vxl_byte>(output_img1);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_img1_sptr);
  vil_image_view_base_sptr output_img2_sptr = new vil_image_view<vxl_byte>(output_img2);
  pro.set_output_val<vil_image_view_base_sptr>(1, output_img2_sptr);

  return true;
}

//: Constructor - Refine a current set of keypoint matches by computing a fundamental matrix through robust estimation and eliminating outliers
bool bapl_refine_match_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bapl_keypoint_match_set_sptr");
  input_types.emplace_back("float");  // outlier threshold
  input_types.emplace_back("int"); // min number of matches to even try to compute F, if the set contains less matches just remove them all and return an empty match set
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("bapl_keypoint_match_set_sptr");
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_refine_match_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cout << "bapl_match_keypoints_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  bapl_keypoint_match_set_sptr match_set = pro.get_input<bapl_keypoint_match_set_sptr>(i++);
  std::vector<bapl_key_match> matches = match_set->matches_;
  auto outlier_threshold = pro.get_input<float>(i++);
  int min_number_of_matches = pro.get_input<int>(i++);
  min_number_of_matches = min_number_of_matches < 9 ? 9 : min_number_of_matches;  // F computation requires at least 8 matches

  vul_timer t;
  std::vector<bapl_key_match> matches_pruned;

  if ((int)matches.size() >= min_number_of_matches) {  // compute F only if there are enough matches, otherwise prune all
    match_set->refine_matches(outlier_threshold, matches_pruned);
  }

  std::cout << "Match Refinement took " << t.real()/(1000.0*60.0) << " mins.\n";

  //bapl_keypoint_match_set_sptr key_set = new bapl_keypoint_match_set(matches_pruned2);
  bapl_keypoint_match_set_sptr key_set = new bapl_keypoint_match_set(match_set->id_left_, match_set->id_right_, matches_pruned);
  pro.set_output_val<bapl_keypoint_match_set_sptr>(0, key_set);

  return true;
}


//: Constructor
//  inputs a simple text file:
// \verbatim
//  <img 1 id> <img 2 id>
//  <number of matches>
//  <key id 1> <key id 2>
//  .
//  .
//  .
// \endverbatim
//  assumes that the key ids are their order in the keypoint vector of the image
bool bapl_load_match_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bapl_keypoint_set_sptr"); // keypoint vector for image 1
  input_types.emplace_back("bapl_keypoint_set_sptr"); // keypoint vector for image 2
  input_types.emplace_back("vcl_string"); // match file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("bapl_keypoint_match_set_sptr"); // out img1 with keys marked same color
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_load_match_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cout << "bapl_load_match_process: The input number should be 3" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  bapl_keypoint_set_sptr input1_sptr = pro.get_input<bapl_keypoint_set_sptr>(i++);
  bapl_keypoint_set_sptr input2_sptr = pro.get_input<bapl_keypoint_set_sptr>(i++);
  std::string input_match_file = pro.get_input<std::string>(i++);
  std::vector<bapl_keypoint_sptr>& keys1 = input1_sptr->keys_;
  std::vector<bapl_keypoint_sptr>& keys2 = input2_sptr->keys_;

  std::vector<bapl_key_match> matches;
  // read the match file
  std::ifstream ifs(input_match_file.c_str());
  if (!ifs) {
    std::cout << "Cannot open: " << input_match_file << std::endl;
    return false;
  }
  int id1, id2; ifs >> id1; ifs >> id2; int cnt; ifs >> cnt;
  std::cout << "Reading " << cnt << " mathces for images: " << id1 << " and " << id2 << std::endl;
  for (int i = 0; i < cnt; i++) {
    int kid1, kid2;
    ifs >> kid1; ifs >> kid2;
    if (kid1 >= (int)keys1.size() || kid2 >= (int)keys2.size()) {
      std::cout << "In bapl_load_match_process() -- Mismatch in keypoint vector size and the match file!\n";
      return false;
    }
    bapl_key_match m(keys1[kid1], keys2[kid2]);
    matches.push_back(m);
  }
  ifs.close();

  bapl_keypoint_match_set_sptr ms = new bapl_keypoint_match_set(id1, id2, matches);
  pro.set_output_val<bapl_keypoint_match_set_sptr>(0, ms);

  return true;
}

//: Constructor
//  outputs a simple text file:
// \verbatim
//  <img 1 id> <img 2 id>
//  <number of matches>
//  <key id 1> <key id 2>
//  .
//  .
//  .
// \endverbatim
//  assumes that the key ids are their order in the keypoint vector of the image
bool bapl_write_match_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bapl_keypoint_match_set_sptr"); // keypoint vector for image 1
  input_types.emplace_back("vcl_string"); // match file name
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_write_match_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cout << "bapl_load_match_process: The input number should be 2" << std::endl;
    return false;
  }
  // get the inputs
  unsigned i=0;
  bapl_keypoint_match_set_sptr input_sptr = pro.get_input<bapl_keypoint_match_set_sptr>(i++);
  std::string input_match_file = pro.get_input<std::string>(i++);

  std::vector<bapl_key_match>& matches = input_sptr->matches_;
  // write the match file
  std::ofstream ofs(input_match_file.c_str());
  ofs << input_sptr->id_left_ << ' ' << input_sptr->id_right_ << '\n'
      << matches.size() << std::endl;
  for (auto & matche : matches)
    ofs << matche.first->id() << ' ' << matche.second->id() << std::endl;
  return true;
}
