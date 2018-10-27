// This is brl/bseg/bapl/pro/processes/bapl_extract_keypoints_process.cxx

//:
// \file
// \brief A class to extract keypoints and descriptors from an image
//
// \author Ozge Can Ozcanli
// \date September 15, 2010
//
// \verbatim
//  Modifications
//    none
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <bapl/bapl_keypoint.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_keypoint_extractor.h>
#include <bapl/bapl_keypoint_set.h>
#include <bapl/bapl_keypoint_set_sptr.h>
#include <ipts/ipts_draw.h>

#include <vil/vil_new.h>
#include <vil/vil_convert.h>
#include <vul/vul_timer.h>

//: Constructor
bool bapl_extract_keypoints_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  input_types.emplace_back("vcl_string"); // path and name of output keypoint file (a list of keypoint descriptors in a simple txt file)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output color image with keypoints marked
  output_types.emplace_back("bapl_keypoint_set_sptr");    // output a set of keypoints
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_extract_keypoints_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cout << "bapl_extract_keypoints_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image(input_image_sptr);

  vil_image_resource_sptr grey_image_sptr;
  if (input_image.nplanes() == 3)
    grey_image_sptr = vil_new_image_resource_of_view( *vil_convert_to_grey_using_rgb_weighting ( input_image_sptr ) );
  else
    grey_image_sptr = vil_new_image_resource_of_view( input_image );

  std::string key_path = pro.get_input<std::string>(i++);

  vul_timer t;
  std::vector< bapl_keypoint_sptr > keypoints;
  std::cout << "Finding Keypoints" << std::endl;
  bapl_keypoint_extractor( grey_image_sptr, keypoints);

  std::cout << "Drawing Keypoints" << std::endl;

  vil_image_view<vxl_byte> output_img;
  output_img.deep_copy(input_image);
  for (const auto & keypoint : keypoints){
    bapl_lowe_keypoint_sptr kp;
    kp.vertical_cast(keypoint);
    int ii = int(kp->location_i()+0.5); int jj = int(kp->location_j()+0.5);
    if (ii >= 0 && jj >= 0 && ii < (int)output_img.ni() && jj < (int)output_img.nj())
      //if (kp->scale()>1.1)
        ipts_draw_cross(output_img, ii,jj,int(kp->scale()+0.5), vxl_byte(255) );
  }

  vil_image_view_base_sptr output_img_sptr = new vil_image_view<vxl_byte>(output_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_img_sptr);

  bapl_keypoint_set_sptr key_set = new bapl_keypoint_set(keypoints);
  pro.set_output_val<bapl_keypoint_set_sptr>(1, key_set);

  // write keypoints to file
  std::ofstream ofs(key_path.c_str());
  if (!ofs.is_open()) {
    std::cerr << "Failed to open file " << key_path.c_str() << std::endl;
    return false;
  }
  ofs << keypoints.size() << " ";
  int len = 128; ofs << len << std::endl;
  for (const auto & keypoint : keypoints) {
    bapl_lowe_keypoint_sptr kp;
    kp.vertical_cast(keypoint);
    ofs << kp->location_j() << " " << kp->location_i() << " "; // i <-> y, j <-> x
    ofs << kp->scale() << " " << kp->orientation() << std::endl;
    vnl_vector_fixed<double, 128> desc = kp->descriptor();
    for (int j = 0; j < len; j++) {
      ofs << desc[j] << " ";
    }
    ofs << std::endl;
  }
  ofs.close();
  std::cout << "Extracted keypoints in " << t.real()/(1000.0*60.0) << " mins.\n";

  return true;
}

//: Constructor
bool bapl_draw_keypoints_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vil_image_view_base_sptr"); // input image
  input_types.emplace_back("bapl_keypoint_set_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vil_image_view_base_sptr");  // output color image with keypoints marked
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_draw_keypoints_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cout << "bapl_draw_keypoints_process: The input number should be 2" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  vil_image_view_base_sptr input_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> input_image(input_image_sptr);

  bapl_keypoint_set_sptr key_set = pro.get_input<bapl_keypoint_set_sptr>(i++);
  std::vector< bapl_keypoint_sptr >& keypoints = key_set->keys_;

  vil_image_view<vxl_byte> output_img(input_image.ni(), input_image.nj(), 3);
  vil_image_view<vxl_byte> output_img_r = vil_plane(output_img, 0);
  vil_image_view<vxl_byte> output_img_g = vil_plane(output_img, 1);
  vil_image_view<vxl_byte> output_img_b = vil_plane(output_img, 2);
  if (input_image.nplanes() == 3)
    output_img.deep_copy(input_image);
  else {
    output_img_r.deep_copy(input_image);
    output_img_g.deep_copy(input_image);
    output_img_b.deep_copy(input_image);
  }

  for (const auto & keypoint : keypoints){
    bapl_lowe_keypoint_sptr kp;
    kp.vertical_cast(keypoint);
    int ii = int(kp->location_i()+0.5); int jj = int(kp->location_j()+0.5);
    if (ii >= 0 && jj >= 0 && ii < (int)output_img.ni() && jj < (int)output_img.nj())
      //if (kp->scale()>1.1)
        ipts_draw_cross(output_img_r, ii,jj,int(kp->scale()+0.5), vxl_byte(255) );

  }

  vil_image_view_base_sptr output_img_sptr = new vil_image_view<vxl_byte>(output_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, output_img_sptr);
  return true;
}


//: Constructor
bool bapl_load_keypoints_process_cons(bprb_func_process& pro)
{
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // path and name of input keypoint file (a list of keypoint descriptors in a simple txt file)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;
  std::vector<std::string> output_types;
  output_types.emplace_back("bapl_keypoint_set_sptr");    // output a set of keypoints
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

bool bapl_load_keypoints_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    std::cout << "bapl_load_keypoints_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  unsigned i=0;
  std::string key_path = pro.get_input<std::string>(i++);

  std::vector< bapl_keypoint_sptr > keypoints;

  std::cout << "Reading Keypoints from file: " << key_path.c_str() << std::endl;
  std::ifstream ifs(key_path.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << key_path.c_str() << std::endl;
    return false;
  }
  int n; ifs >> n; int len; ifs >> len;
  std::cout << "Found " << n << " keypoints.\n";
  for (int i = 0; i < n; i++) {
    bapl_lowe_keypoint_sptr kp = read_from_file(ifs, len);
    keypoints.push_back(kp);
  }
  ifs.close();
  //: set ids of the keypoints as the order they're read from the file
  for (unsigned i = 0; i < keypoints.size(); i++) {
    keypoints[i]->set_id(i);
  }

  bapl_keypoint_set_sptr key_set = new bapl_keypoint_set(keypoints);
  pro.set_output_val<bapl_keypoint_set_sptr>(0, key_set);
  return true;
}
