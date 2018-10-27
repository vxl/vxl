// This is contrib/brl/bbas/volm/pro/process/volm_layer_extraction_process.cxx
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdio>
#include <bprb/bprb_func_process.h>
//:
// \file
//     Various processes that refine LandCover images using height values retrieved from certain kinds of height maps, such as DEM, nDSM etc.
// \verbatim
//  Modifications
//    none yet
//  \endverbatim

#include <vul/vul_file.h>
//#include <vcl_compiler.h>
#include <vnl/vnl_gamma.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/algo/vil_threshold.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_convert.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <bbas_pro/bbas_1d_array_float.h>

namespace volm_layer_extraction_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 4;
}

bool volm_layer_extraction_process_cons(bprb_func_process& pro)
{
  using namespace volm_layer_extraction_process_globals;
  // this process takes 8 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input land cover image (Byte image)
  input_types_[1] = "vpgl_camera_double_sptr";   // input land cover image camera (geo camera)
  input_types_[2] = "vil_image_view_base_sptr";  // input height image (float or byte image)
  input_types_[3] = "vpgl_camera_double_sptr";   // input height image camera (geo camera)
  input_types_[4] = "vcl_string";                // a list of ids that contain all desired land tags
  input_types_[5] = "float";                     // minimum height value of desired building
  input_types_[6] = "float";                     // maximum height value of desired building
  input_types_[7] = "double";                     // shape factor for Generalized Gaussian PDF
  // this process takes 3 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output probability image
  output_types_[1] = "vil_image_view_base_sptr";  // output binary image
  output_types_[2] = "vil_image_view_base_sptr";  // a binary mask that shows pixels where both land cover image and height image are available
  output_types_[3] = "vpgl_camera_double_sptr";   // output camera
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_layer_extraction_process(bprb_func_process& pro)
{
  using namespace volm_layer_extraction_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr land_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  land_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  height_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  std::string land_txt = pro.get_input<std::string>(in_i++);
  auto min_h = pro.get_input<float>(in_i++);
  auto max_h = pro.get_input<float>(in_i++);
  auto beta = pro.get_input<double>(in_i++);

  std::vector<unsigned char> land_ids;
  std::ifstream ifs(land_txt.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << land_txt.c_str() << std::endl;
    return false;
  }
  int tmp_id;
  while (!ifs.eof()) {
    ifs >> tmp_id;
    if (std::find(land_ids.begin(), land_ids.end(), (unsigned char)tmp_id) == land_ids.end())
      land_ids.push_back((unsigned char)tmp_id);
  }
  std::cout << "following ids are treated as buildings: ";
  for (unsigned char land_id : land_ids)
    std::cout << (int)land_id << ", ";
  std::cout << std::endl;

  auto* l_img = dynamic_cast<vil_image_view<vxl_byte>*>(land_img_sptr.ptr());
  if (!l_img) {
    std::cerr << pro.name() << ": Unsupported land cover image pixel format -- " << land_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  // load the images and cameras
  auto* h_img = dynamic_cast<vil_image_view<float>*>(height_img_sptr.ptr());
  vil_image_view_base_sptr h_img_float;
  if (!h_img) {
    vil_image_view<float> temp(height_img_sptr->ni(), height_img_sptr->nj(), 1);
    auto* h_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(height_img_sptr.ptr());
    if (!h_img_byte) {
      std::cerr << pro.name() << ": Unsupported height image pixel format -- " << height_img_sptr->pixel_format() << ", only float and byte are supported!\n";
      return false;
    }
    else {
      vil_convert_cast(*h_img_byte, temp);
    }
    h_img_float = new vil_image_view<float>(temp);  // shallow copy
    h_img = dynamic_cast<vil_image_view<float>*>(h_img_float.ptr());
  }

  auto* l_cam = dynamic_cast<vpgl_geo_camera*>(land_cam_sptr.ptr());
  if (!l_cam) {
    std::cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  auto* h_cam = dynamic_cast<vpgl_geo_camera*>(height_cam_sptr.ptr());
  if (!h_cam) {
    std::cerr << pro.name() << ": can not load height image camera!\n";
    return false;
  }
  // initialize output images
  unsigned l_ni, l_nj, h_ni, h_nj;
  l_ni = l_img->ni();  l_nj = l_img->nj();
  h_ni = h_img->ni();  h_nj = h_img->nj();

  auto* out_prob_img = new vil_image_view<float>(l_ni, l_nj);
  out_prob_img->fill(0);
  auto* out_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  out_img->fill(0);
  auto* mask_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  mask_img->fill(0);

  // compute GGD function parameters
  if (min_h >= max_h) {
    std::cerr << pro.name() << ": minimum height " << min_h << " is larger than maximum height: " << max_h << "!\n";
    return false;
  }
  double mu = (max_h + min_h) * 0.5;
  double alpha = (max_h - min_h) * 0.5;
  double gamma = vnl_gamma(1.0/beta);
  double pre_param = beta / (2*alpha*gamma);

  std::cout << "start to generate building layer with height from " << min_h << " to " << max_h << "..." << std::flush << std::endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      if (std::find(land_ids.begin(), land_ids.end(), l_id) == land_ids.end())
        continue;
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      auto uu = (unsigned)std::floor(u+0.5);
      auto vv = (unsigned)std::floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      // retrieve height value from height image
      float height = (*h_img)(uu, vv);
      if (height == 255)
        continue;
      // compute the GGD probability
      double prob = pre_param * std::exp( -1 * std::pow(std::abs((height-mu)/alpha), beta));
      (*out_prob_img)(i,j) = prob;
    }
  }
  std::cout << " DONE!" << std::endl;
  std::cout << "start to generate a mask image..." << std::flush << std::endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      auto uu = (unsigned)std::floor(u+0.5);
      auto vv = (unsigned)std::floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      if ((*h_img)(uu,vv) < 255) {
        (*mask_img)(i,j) = 1;
      }
    }
  }
  std::cout << " DONE!" << std::endl;
  // start to generate ID images
  double min_thres_prob = pre_param * std::exp( -1 * std::pow(std::abs((min_h-mu)/alpha), beta));
  double max_thres_prob = pre_param * std::exp( -1 * std::pow(std::abs((max_h-mu)/alpha), beta));
  double thres = (min_thres_prob + max_thres_prob)*0.5;
  std::cout << "min_thres: " << min_thres_prob << ", max_thres: " << max_thres_prob << std::endl;
  std::cout << "Start to generate a binary image given threshold: " << thres << "..." << std::flush << std::endl;

  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      if ((*out_prob_img)(i,j) >= 0.9*thres) {
        (*out_img)(i,j) = 1;
      }
    }
  }
  std::cout << " DONE!" << std::endl;
  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_prob_img));
  pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(out_img));
  pro.set_output_val<vil_image_view_base_sptr>(2, vil_image_view_base_sptr(mask_img));
  pro.set_output_val<vpgl_camera_double_sptr>(3, land_cam_sptr);
  return true;
}

//: process to generate various building layers from LandCover images and height images
namespace volm_building_layer_extraction_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 3;
}

bool volm_building_layer_extraction_process_cons(bprb_func_process& pro)
{
  using namespace volm_building_layer_extraction_process_globals;
  // this process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input land cover image (Byte image)
  input_types_[1] = "vpgl_camera_double_sptr";   // input land cover image camera (geo camera)
  input_types_[2] = "vil_image_view_base_sptr";  // input height image (float or byte image)
  input_types_[3] = "vpgl_camera_double_sptr";   // input height image camera (geo camera)
  input_types_[4] = "vcl_string";                // a txt file that contains all building land tags
  input_types_[5] = "float";                     // minimum height value of desired building
  input_types_[6] = "float";                     // maximum height value of desired building
  // this process takes 2 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output binary image
  output_types_[1] = "vil_image_view_base_sptr";  // a binary mask that shows pixels where both land cover image and height image are available
  output_types_[2] = "vpgl_camera_double_sptr";   // output camera

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_building_layer_extraction_process(bprb_func_process& pro)
{
  using namespace volm_building_layer_extraction_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr land_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  land_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  height_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  std::string land_txt = pro.get_input<std::string>(in_i++);
  auto min_h = pro.get_input<float>(in_i++);
  auto max_h = pro.get_input<float>(in_i++);

  auto* l_img = dynamic_cast<vil_image_view<vxl_byte>*>(land_img_sptr.ptr());
  if (!l_img) {
    std::cerr << pro.name() << ": Unsupported land cover image pixel format -- " << land_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }

  auto* h_img = dynamic_cast<vil_image_view<float>*>(height_img_sptr.ptr());
  vil_image_view_base_sptr h_img_float;
  if (!h_img) {
    vil_image_view<float> temp(height_img_sptr->ni(), height_img_sptr->nj(), 1);
    auto* h_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(height_img_sptr.ptr());
    if (!h_img_byte) {
      std::cerr << pro.name() << ": Unsupported height image pixel format -- " << height_img_sptr->pixel_format() << ", only float and byte are supported!\n";
      return false;
    }
    else {
      vil_convert_cast(*h_img_byte, temp);
    }
    h_img_float = new vil_image_view<float>(temp);  // shallow copy
    h_img = dynamic_cast<vil_image_view<float>*>(h_img_float.ptr());
  }

  auto* l_cam = dynamic_cast<vpgl_geo_camera*>(land_cam_sptr.ptr());
  if (!l_cam) {
    std::cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  auto* h_cam = dynamic_cast<vpgl_geo_camera*>(height_cam_sptr.ptr());
  if (!h_cam) {
    std::cerr << pro.name() << ": can not load height image camera!\n";
    return false;
  }

  std::vector<unsigned char> land_ids;
  std::ifstream ifs(land_txt.c_str());
  if (!ifs.is_open()) {
    std::cerr << "Failed to open file " << land_txt.c_str() << std::endl;
    return false;
  }
  int tmp_id;
  while (!ifs.eof()) {
    ifs >> tmp_id;
    if (std::find(land_ids.begin(), land_ids.end(), (unsigned char)tmp_id) == land_ids.end())
      land_ids.push_back((unsigned char)tmp_id);
  }
  std::cout << "following ids are treated as buildings: ";
  for (unsigned char land_id : land_ids)
    std::cout << (int)land_id << ", ";
  std::cout << std::endl;

  // output image will have same resolution as input land cover images
  unsigned l_ni, l_nj, h_ni, h_nj;
  l_ni = l_img->ni();  l_nj = l_img->nj();
  h_ni = h_img->ni();  h_nj = h_img->nj();

  auto* out_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  out_img->fill(0);
  auto* mask_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  mask_img->fill(0);
  std::cout << "start to generate building layer with height from " << min_h << " to " << max_h << "..." << std::flush << std::endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      if (std::find(land_ids.begin(), land_ids.end(), l_id) == land_ids.end())
        continue;
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      auto uu = (unsigned)std::floor(u+0.5);
      auto vv = (unsigned)std::floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      // retrieve height value from height image
      float height = (*h_img)(uu, vv);
      if (height >= min_h && height < max_h) {
        (*out_img)(i, j) = 1;
      }
    }
  }
  std::cout << " DONE!" << std::endl;
  std::cout << "start to generate a mask image..." << std::flush << std::endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      auto uu = (unsigned)std::floor(u+0.5);
      auto vv = (unsigned)std::floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      if ((*h_img)(uu,vv) < 255) {
        (*mask_img)(i,j) = 1;
      }
    }
  }
  std::cout << " DONE!" << std::endl;

  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
  pro.set_output_val<vil_image_view_base_sptr>(1, vil_image_view_base_sptr(mask_img));
  pro.set_output_val<vpgl_camera_double_sptr>(2, land_cam_sptr);
  return true;
}

//: process to generate kml file from extract binary layer images
// Note that this process uses volm_candidate_list to transfer image connected pixels to polygon
#include <volm/volm_candidate_list.h>

namespace volm_generate_kml_from_binary_image_process_globals
{
  unsigned n_inputs_ = 7;
  unsigned n_outputs_ = 1;
}

bool volm_generate_kml_from_binary_image_process_cons(bprb_func_process& pro)
{
  using namespace volm_generate_kml_from_binary_image_process_globals;
  // this process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";    // input layer binary image (byte image)
  input_types_[1] = "vpgl_camera_double_sptr";     // geo-camera of the input layer binary
  input_types_[2] = "unsigned";                    // threshold of the image pixel (pixel value larger than given threshold will be considered)
  input_types_[3] = "vcl_string";                  // output polygon kml file
  input_types_[4] = "unsigned";                    // r
  input_types_[5] = "unsigned";                    // g
  input_types_[6] = "unsigned";                    // b
  // this process takes 1 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";                   // number of polygon geometries generate from image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_generate_kml_from_binary_image_process(bprb_func_process& pro)
{
  using namespace volm_generate_kml_from_binary_image_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  auto  threshold = pro.get_input<unsigned>(in_i++);
  std::string out_file = pro.get_input<std::string>(in_i++);
  auto r = pro.get_input<unsigned>(in_i++);
  auto g = pro.get_input<unsigned>(in_i++);
  auto b = pro.get_input<unsigned>(in_i++);

  auto* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported land cover image pixel format -- " << in_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  auto* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    std::cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  // generate the connected component
  volm_candidate_list cand_list(*in_img, threshold);
  vgl_polygon<int> img_polys = cand_list.cand_poly();
  // transfer from image domain to world domain
  unsigned n_sheet = img_polys.num_sheets();
  std::vector<vgl_polygon<double> > polys;
  for (unsigned sidx = 0; sidx < n_sheet; sidx++)
  {
    std::vector<vgl_point_2d<int> > sheet = img_polys[sidx];
    vgl_polygon<double> single_poly;
    single_poly.new_sheet();
    for (auto & vidx : sheet) {
      double lon, lat;
      in_cam->img_to_global(vidx.x(), vidx.y(), lon, lat);
      single_poly.push_back(lon, lat);
    }
    polys.push_back(single_poly);
  }
  // write to kml
  std::ofstream ofs(out_file.c_str());
  if (!ofs.is_open()) {
    std::cerr << pro.name() << ": Failed to write " << out_file << "!\n";
    return false;
  }
  bkml_write::open_document(ofs);
  for (const auto & poly : polys) {
    bkml_write::write_polygon(ofs, poly, "polygon", "", 1.0, 3.0, 0.45, r, g, b);
  }
  bkml_write::close_document(ofs);
  ofs.close();

  // output
  pro.set_output_val<unsigned>(0, static_cast<unsigned>(polys.size()));
  return true;
}

//: process to downsample the generate land layer image
namespace volm_downsample_binary_layer_process_globals
{
  unsigned n_inputs_ = 6;
  unsigned n_outputs_ = 0;
}

bool volm_downsample_binary_layer_process_cons(bprb_func_process& pro)
{
  using namespace volm_downsample_binary_layer_process_globals;
  // this process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input layer image (byte image)
  input_types_[1] = "vil_image_view_base_sptr";  // input layer mask (byte image)
  input_types_[2] = "vpgl_camera_double_sptr";   // geo camera of the input image
  input_types_[3] = "vil_image_view_base_sptr";  // down-sample image (byte image)
  input_types_[4] = "vil_image_view_base_sptr";  // down-sample mask (byte image)
  input_types_[5] = "vpgl_camera_double_sptr";   // geo camera of down-sample image
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_downsample_binary_layer_process(bprb_func_process& pro)
{
  using namespace volm_downsample_binary_layer_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr in_mask_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr out_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr out_mask_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  out_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);

  auto* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    std::cerr << pro.name() << ": Unsupported land cover image pixel format -- " << in_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  auto* in_mask = dynamic_cast<vil_image_view<vxl_byte>*>(in_mask_sptr.ptr());
  if (!in_mask) {
    std::cerr << pro.name() << ": Unsupported land cover mask image pixel format -- " << in_mask_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  auto* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    std::cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }

  auto* out_img = dynamic_cast<vil_image_view<vxl_byte>*>(out_img_sptr.ptr());
  if (!out_img) {
    std::cerr << pro.name() << ": Unsupported down sample image pixel format -- " << out_img_sptr->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  auto* out_mask = dynamic_cast<vil_image_view<vxl_byte>*>(out_mask_sptr.ptr());
  if (!out_mask) {
    std::cerr << pro.name() << ": Unsupported down sample mask image pixel format -- " << out_mask_sptr->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  auto* out_cam = dynamic_cast<vpgl_geo_camera*>(out_cam_sptr.ptr());
  if (!out_cam) {
    std::cerr << pro.name() << ": can not load down sample image camera!\n";
    return false;
  }

  // start to fill the image
  unsigned i_ni = in_img->ni();
  unsigned i_nj = in_img->nj();
  unsigned o_ni = out_img->ni();
  unsigned o_nj = out_img->nj();
  std::cout << "Start to fill the image..." << std::flush << std::endl;
  for (unsigned i = 0; i < i_ni; i++) {
    if (i%1000 == 0)
      std::cout << i << '.' << std::flush;
    for (unsigned j = 0; j < i_nj; j++) {
      double lon, lat;
      in_cam->img_to_global(i, j, lon, lat);
      double u, v;
      out_cam->global_to_img(lon, lat, 0.0, u, v);
      auto uu = (unsigned)std::floor(u+0.5);
      auto vv = (unsigned)std::floor(v+0.5);
      if ( uu >= o_ni || vv >= o_nj)
        continue;
      (*out_img)(uu,vv) =  (*in_img)(i,j);
      (*out_mask)(uu,vv) = (*in_mask)(i,j);
    }
  }
  std::cout << "DONE!" << std::endl;

  return true;
}

//: process to compute detection rate and generate detection-rate base roc curve
namespace volm_detection_rate_roc_process_globals
{
  unsigned n_inputs_ = 4;
  unsigned n_outputs_ = 7;
}

bool volm_detection_rate_roc_process_cons(bprb_func_process& pro)
{
  using namespace volm_detection_rate_roc_process_globals;
  // this process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input probability image
  input_types_[1] = "vpgl_camera_double_sptr";   // input geocamera
  input_types_[2] = "vcl_string";                // positive ground truth kml file
  input_types_[3] = "vcl_string";                // negative ground truth kml file
  // this process generates 4 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_float_sptr";  // threshold array
  output_types_[1] = "bbas_1d_array_float_sptr";  // tp
  output_types_[2] = "bbas_1d_array_float_sptr";  // tn
  output_types_[3] = "bbas_1d_array_float_sptr";  // fp
  output_types_[4] = "bbas_1d_array_float_sptr";  // fn
  output_types_[5] = "bbas_1d_array_float_sptr";  // tpr
  output_types_[6] = "bbas_1d_array_float_sptr";  // fpr
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_detection_rate_roc_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  std::string              pos_poly_kml = pro.get_input<std::string>(in_i++);
  std::string              neg_poly_kml = pro.get_input<std::string>(in_i++);

  // get camera
  auto* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    std::cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  if (!vul_file::exists(pos_poly_kml)) {
    std::cerr << pro.name() << ": can not find input positive ground truth kml file -- " << pos_poly_kml << "!\n";
    return false;
  }
  if (!vul_file::exists(neg_poly_kml)) {
    std::cerr << pro.name() << ": can not find input negative ground truth kml file -- " << neg_poly_kml << "!\n";
    return false;
  }
  vgl_polygon<double> pos_poly = bkml_parser::parse_polygon(pos_poly_kml);
  vgl_polygon<double> neg_poly = bkml_parser::parse_polygon(neg_poly_kml);

  // convert image to [0,1] float
  vil_image_view<float>* detection_map;
  if (auto* detection_map_unchar = dynamic_cast<vil_image_view<unsigned char>*>(in_img_sptr.ptr())) {
    detection_map = new vil_image_view<float>(detection_map_unchar->ni(), detection_map_unchar->nj());
    vil_convert_stretch_range_limited<unsigned char>(*detection_map_unchar, *detection_map, 0, 255, 0.0f, 1.0f);
  }
  else if (dynamic_cast<vil_image_view<float>*>(in_img_sptr.ptr())) {
    detection_map = dynamic_cast<vil_image_view<float>*>(in_img_sptr.ptr());
  }
  else {
    std::cerr << pro.name() << ": input detection image can not be converted to float image!\n";
    return false;
  }

  std::cout << "There are " << pos_poly.num_sheets() << " positive regions and " << neg_poly.num_sheets() << " negative regions." << std::endl;
  auto n_pos_condition = static_cast<unsigned>(pos_poly.num_sheets());
  auto n_neg_condition = static_cast<unsigned>(neg_poly.num_sheets());

  // convert polygon from world domain to image domain
  std::vector<vgl_polygon<double> > pos_img_poly;
  std::vector<vgl_polygon<double> > neg_img_poly;
  std::vector<vgl_box_2d<int> > pos_bbox;
  std::vector<vgl_box_2d<int> > neg_bbox;
  for (unsigned i = 0; i < n_pos_condition; i++) {
    vgl_polygon<double> pos_single_sheet;
    pos_single_sheet.new_sheet();
    vgl_box_2d<int> bbox;
    auto n_verts = static_cast<unsigned>(pos_poly[i].size());
    for (unsigned pidx = 0; pidx < n_verts; pidx++) {
      vgl_point_2d<double> wp = pos_poly[i][pidx];
      double u, v;
      in_cam->global_to_img(wp.x(), wp.y(), 0.0, u, v);
      vgl_point_2d<int> img_pt((int)std::floor(u+0.5), (int)std::floor(v+0.5));
      bbox.add(img_pt);
      pos_single_sheet.push_back(u,v);
    }
    pos_bbox.push_back(bbox);
    pos_img_poly.push_back(pos_single_sheet);
  }
  for (unsigned i = 0; i < n_neg_condition; i++) {
    vgl_polygon<double> neg_single_sheet;
    neg_single_sheet.new_sheet();
    vgl_box_2d<int> bbox;
    auto n_verts = static_cast<unsigned>(neg_poly[i].size());
    for (unsigned pidx = 0; pidx < n_verts; pidx++) {
      vgl_point_2d<double> wp = neg_poly[i][pidx];
      double u, v;
      in_cam->global_to_img(wp.x(), wp.y(), 0.0, u, v);
      vgl_point_2d<int> img_pt((int)std::floor(u+0.5), (int)std::floor(v+0.5));
      bbox.add(img_pt);
      neg_single_sheet.push_back(u, v);
    }
    neg_bbox.push_back(bbox);
    neg_img_poly.push_back(neg_single_sheet);
  }

  // create threshold based on image range
  float min_val, max_val;
  vil_math_value_range(*detection_map, min_val,max_val);
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
    tp->data_array[i] = 0.0f;
    tn->data_array[i] = 0.0f;
    fp->data_array[i] = 0.0f;
    fn->data_array[i] = 0.0f;
    tpr->data_array[i] = 0.0f;
    fpr->data_array[i] = 0.0f;
  }

  // count
  unsigned ni = detection_map->ni();
  unsigned nj = detection_map->nj();
  for (unsigned t_idx = 0; t_idx < n_thres; t_idx++)
  {
    // convert detection map to binary image
    vil_image_view<bool> pos_img(ni, nj);
    vil_threshold_above(*detection_map, pos_img, thresholds[t_idx]);
    for (unsigned pos_idx = 0; pos_idx < n_pos_condition; pos_idx++) {
      vgl_box_2d<int> bbox = pos_bbox[pos_idx];
      vgl_polygon<double> poly = pos_img_poly[pos_idx];
      int min_i, min_j, max_i, max_j;
      min_i = bbox.min_x();  min_j = bbox.min_y();
      max_i = bbox.max_x();  max_j = bbox.max_y();
      bool found = false;
      for (int i = min_i; ( i <= max_i && !found); i++) {
        for (int j = min_j; ( j <= max_j && !found); j++) {
          if ( i < 0 || i >= ni || j < 0 || j >= nj)
            continue;
          if (pos_img(i,j) && poly.contains(i,j)) {  // there is a positive pixel inside the positive polygon region -- true positive
            found = true;
            tp->data_array[t_idx] += 1;
          }
        }
      }
      if (!found)                                    // there is no positive pixel inside the current positive polygon region -- false negative
        fn->data_array[t_idx] += 1;
    }
    for (unsigned neg_idx = 0; neg_idx < n_pos_condition; neg_idx++) {
      vgl_box_2d<int> bbox = neg_bbox[neg_idx];
      vgl_polygon<double> poly = neg_img_poly[neg_idx];
      int min_i, min_j, max_i, max_j;
      min_i = bbox.min_x();  min_j = bbox.min_y();
      max_i = bbox.max_x();  max_j = bbox.max_y();
      bool found = false;
      for (int i = min_i; ( i <= max_i && !found); i++) {
        for (int j = min_j; ( j <= max_j && !found); j++) {
          if ( i < 0 || i >= ni || j < 0 || j >= nj)
            continue;
          if (pos_img(i,j) && poly.contains(i,j)) {  // there is a positive pixel inside the negative polygon region -- false positive
            found = true;
            fp->data_array[t_idx] += 1;
          }
        }
      }
      if (!found)                                    // there is no positive pixel inside the current negative polygon region -- true negative
        tn->data_array[t_idx] += 1;
    }
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
