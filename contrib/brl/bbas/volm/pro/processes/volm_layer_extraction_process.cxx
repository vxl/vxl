// This is contrib/brl/bbas/volm/pro/process/volm_layer_extraction_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//     Various processes that refine LandCover images using height values retrieved from certain kinds of height maps, such as DEM, nDSM etc.
// \verbatim
//  Modifications
//    none yet
//  \endverbatim

#include <vul/vul_file.h>
#include <vcl_iomanip.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_gamma.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_convert.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>


namespace volm_layer_extraction_process_globals
{
  const unsigned n_inputs_  = 8;
  const unsigned n_outputs_ = 4;
}

bool volm_layer_extraction_process_cons(bprb_func_process& pro)
{
  using namespace volm_layer_extraction_process_globals;
  // this process takes 8 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input land cover image (Byte image)
  input_types_[1] = "vpgl_camera_double_sptr";   // input land cover image camera (geo camera)
  input_types_[2] = "vil_image_view_base_sptr";  // input height image (float or byte image)
  input_types_[3] = "vpgl_camera_double_sptr";   // input height image camera (geo camera)
  input_types_[4] = "vcl_string";                // a list of ids that contain all desired land tags
  input_types_[5] = "float";                     // minimum height value of desired building
  input_types_[6] = "float";                     // maximum height value of desired building
  input_types_[7] = "double";                     // shape factor for Generalized Gaussian PDF
  // this process takes 3 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
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
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr land_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  land_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  height_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vcl_string land_txt = pro.get_input<vcl_string>(in_i++);
  float min_h = pro.get_input<float>(in_i++);
  float max_h = pro.get_input<float>(in_i++);
  double beta = pro.get_input<double>(in_i++);

  vcl_vector<unsigned char> land_ids;
  vcl_ifstream ifs(land_txt.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << land_txt.c_str() << vcl_endl;
    return 0;
  }
  int tmp_id;
  while (!ifs.eof()) {
    ifs >> tmp_id;
    if (vcl_find(land_ids.begin(), land_ids.end(), (unsigned char)tmp_id) == land_ids.end())
      land_ids.push_back((unsigned char)tmp_id);
  }
  vcl_cout << "following ids are treated as buildings: ";
  for (unsigned i = 0; i < land_ids.size(); i++)
    vcl_cout << (int)land_ids[i] << ", ";
  vcl_cout << vcl_endl;

  vil_image_view<vxl_byte>* l_img = dynamic_cast<vil_image_view<vxl_byte>*>(land_img_sptr.ptr());
  if (!l_img) {
    vcl_cerr << pro.name() << ": Unsupported land cover image pixel format -- " << land_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  // load the images and cameras
  vil_image_view<float>* h_img = dynamic_cast<vil_image_view<float>*>(height_img_sptr.ptr());
  vil_image_view_base_sptr h_img_float;
  if (!h_img) {
    vil_image_view<float> temp(height_img_sptr->ni(), height_img_sptr->nj(), 1);
    vil_image_view<vxl_byte>* h_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(height_img_sptr.ptr());
    if (!h_img_byte) {
      vcl_cerr << pro.name() << ": Unsupported height image pixel format -- " << height_img_sptr->pixel_format() << ", only float and byte are supported!\n";
      return false;
    }
    else {
      vil_convert_cast(*h_img_byte, temp);
    }
    h_img_float = new vil_image_view<float>(temp);  // shallow copy
    h_img = dynamic_cast<vil_image_view<float>*>(h_img_float.ptr());
  }

  vpgl_geo_camera* l_cam = dynamic_cast<vpgl_geo_camera*>(land_cam_sptr.ptr());
  if (!l_cam) {
    vcl_cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  vpgl_geo_camera* h_cam = dynamic_cast<vpgl_geo_camera*>(height_cam_sptr.ptr());
  if (!h_cam) {
    vcl_cerr << pro.name() << ": can not load height image camera!\n";
    return false;
  }
  // initialize output images
  unsigned l_ni, l_nj, h_ni, h_nj;
  l_ni = l_img->ni();  l_nj = l_img->nj();
  h_ni = h_img->ni();  h_nj = h_img->nj();

  vil_image_view<float>* out_prob_img = new vil_image_view<float>(l_ni, l_nj);
  vil_image_view<vxl_byte>* out_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  out_img->fill(0);
  vil_image_view<vxl_byte>* mask_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  mask_img->fill(0);

  // compute GGD function parameters
  if (min_h >= max_h) {
    vcl_cerr << pro.name() << ": minimum height " << min_h << " is larger than maximum height: " << max_h << "!\n";
    return false;
  }
  double mu = (max_h + min_h) * 0.5;
  double alpha = (max_h - min_h) * 0.5;
  double gamma = vnl_gamma(1.0/beta);
  double pre_param = beta / (2*alpha*gamma);

  vcl_cout << "start to generate building layer with height from " << min_h << " to " << max_h << "..." << vcl_flush << vcl_endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      if (vcl_find(land_ids.begin(), land_ids.end(), l_id) == land_ids.end())
        continue;
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u+0.5);
      unsigned vv = (unsigned)vcl_floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      // retrieve height value from height image
      float height = (*h_img)(uu, vv);
      // compute the GGD probability
      double prob = pre_param * vcl_exp( -1 * vcl_pow(vcl_abs((height-mu)/alpha), beta));
      (*out_prob_img)(i,j) = prob;
    }
  }
  vcl_cout << " DONE!" << vcl_endl;
  vcl_cout << "start to generate a mask image..." << vcl_flush << vcl_endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u+0.5);
      unsigned vv = (unsigned)vcl_floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      if ((*h_img)(uu,vv) < 255) {
        (*mask_img)(i,j) = 1;
      }
    }
  }
  vcl_cout << " DONE!" << vcl_endl;
  // start to generate ID images
  double min_thres_prob = pre_param * vcl_exp( -1 * vcl_pow(vcl_abs((min_h-mu)/alpha), beta));
  double max_thres_prob = pre_param * vcl_exp( -1 * vcl_pow(vcl_abs((min_h-mu)/alpha), beta));
  double thres = (min_thres_prob + max_thres_prob)*0.5;
  vcl_cout << "min_thres: " << min_thres_prob << ", max_thres: " << max_thres_prob << vcl_endl;
  vcl_cout << "Start to generate a binary image given threshold: " << thres << "..." << vcl_flush << vcl_endl;

  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      if ((*out_prob_img)(i,j) >= 0.9*thres) {
        (*out_img)(i,j) = 1;
      }
    }
  }
  vcl_cout << " DONE!" << vcl_endl;
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
  const unsigned n_inputs_  = 7;
  const unsigned n_outputs_ = 3;
}

bool volm_building_layer_extraction_process_cons(bprb_func_process& pro)
{
  using namespace volm_building_layer_extraction_process_globals;
  // this process takes 7 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input land cover image (Byte image)
  input_types_[1] = "vpgl_camera_double_sptr";   // input land cover image camera (geo camera)
  input_types_[2] = "vil_image_view_base_sptr";  // input height image (float or byte image)
  input_types_[3] = "vpgl_camera_double_sptr";   // input height image camera (geo camera)
  input_types_[4] = "vcl_string";                // a txt file that contains all building land tags
  input_types_[5] = "float";                     // minimum height value of desired building
  input_types_[6] = "float";                     // maximum height value of desired building
  // this process takes 2 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
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
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr land_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  land_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr height_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  height_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vcl_string land_txt = pro.get_input<vcl_string>(in_i++);
  float min_h = pro.get_input<float>(in_i++);
  float max_h = pro.get_input<float>(in_i++);

  vil_image_view<vxl_byte>* l_img = dynamic_cast<vil_image_view<vxl_byte>*>(land_img_sptr.ptr());
  if (!l_img) {
    vcl_cerr << pro.name() << ": Unsupported land cover image pixel format -- " << land_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }

  vil_image_view<float>* h_img = dynamic_cast<vil_image_view<float>*>(height_img_sptr.ptr());
  vil_image_view_base_sptr h_img_float;
  if (!h_img) {
    vil_image_view<float> temp(height_img_sptr->ni(), height_img_sptr->nj(), 1);
    vil_image_view<vxl_byte>* h_img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(height_img_sptr.ptr());
    if (!h_img_byte) {
      vcl_cerr << pro.name() << ": Unsupported height image pixel format -- " << height_img_sptr->pixel_format() << ", only float and byte are supported!\n";
      return false;
    }
    else {
      vil_convert_cast(*h_img_byte, temp);
    }
    h_img_float = new vil_image_view<float>(temp);  // shallow copy
    h_img = dynamic_cast<vil_image_view<float>*>(h_img_float.ptr());
  }

  vpgl_geo_camera* l_cam = dynamic_cast<vpgl_geo_camera*>(land_cam_sptr.ptr());
  if (!l_cam) {
    vcl_cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  vpgl_geo_camera* h_cam = dynamic_cast<vpgl_geo_camera*>(height_cam_sptr.ptr());
  if (!h_cam) {
    vcl_cerr << pro.name() << ": can not load height image camera!\n";
    return false;
  }
  
  vcl_vector<unsigned char> land_ids;
  vcl_ifstream ifs(land_txt.c_str());
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << land_txt.c_str() << vcl_endl;
    return 0;
  }
  int tmp_id;
  while (!ifs.eof()) {
    ifs >> tmp_id;
    if (vcl_find(land_ids.begin(), land_ids.end(), (unsigned char)tmp_id) == land_ids.end())
      land_ids.push_back((unsigned char)tmp_id);
  }
  vcl_cout << "following ids are treated as buildings: ";
  for (unsigned i = 0; i < land_ids.size(); i++)
    vcl_cout << (int)land_ids[i] << ", ";
  vcl_cout << vcl_endl;

  // output image will have same resolution as input land cover images
  unsigned l_ni, l_nj, h_ni, h_nj;
  l_ni = l_img->ni();  l_nj = l_img->nj();
  h_ni = h_img->ni();  h_nj = h_img->nj();

  vil_image_view<vxl_byte>* out_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  out_img->fill(0);
  vil_image_view<vxl_byte>* mask_img = new vil_image_view<vxl_byte>(l_ni, l_nj);
  mask_img->fill(0);
  vcl_cout << "start to generate building layer with height from " << min_h << " to " << max_h << "..." << vcl_flush << vcl_endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      if (vcl_find(land_ids.begin(), land_ids.end(), l_id) == land_ids.end())
        continue;
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u+0.5);
      unsigned vv = (unsigned)vcl_floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      // retrieve height value from height image
      float height = (*h_img)(uu, vv);
      if (height >= min_h && height < max_h) {
        (*out_img)(i, j) = 1;
      }
    }
  }
  vcl_cout << " DONE!" << vcl_endl;
  vcl_cout << "start to generate a mask image..." << vcl_flush << vcl_endl;
  for (unsigned i = 0; i < l_ni; i++)
  {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < l_nj; j++)
    {
      unsigned char l_id = (*l_img)(i,j);
      double lon, lat;
      l_cam->img_to_global(i, j, lon, lat);
      double u, v;
      h_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u+0.5);
      unsigned vv = (unsigned)vcl_floor(v+0.5);
      if ( uu > h_ni || vv > h_nj )
        continue;
      if ((*h_img)(uu,vv) < 255) {
        (*mask_img)(i,j) = 1;
      }
    }
  }
  vcl_cout << " DONE!" << vcl_endl;

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
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";    // input layer binary image (byte image)
  input_types_[1] = "vpgl_camera_double_sptr";     // geo-camera of the input layer binary
  input_types_[2] = "unsigned";                    // threshold of the image pixel (pixel value larger than given threshold will be considered)
  input_types_[3] = "vcl_string";                  // output polygon kml file
  input_types_[4] = "unsigned";                    // r
  input_types_[5] = "unsigned";                    // g
  input_types_[6] = "unsigned";                    // b
  // this process takes 1 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "unsigned";                   // number of polygon geometries generate from image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_generate_kml_from_binary_image_process(bprb_func_process& pro)
{
  using namespace volm_generate_kml_from_binary_image_process_globals;
  if (!pro.verify_inputs()) {
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  unsigned  threshold = pro.get_input<unsigned>(in_i++);
  vcl_string out_file = pro.get_input<vcl_string>(in_i++);
  unsigned r = pro.get_input<unsigned>(in_i++);
  unsigned g = pro.get_input<unsigned>(in_i++);
  unsigned b = pro.get_input<unsigned>(in_i++);

  vil_image_view<vxl_byte>* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    vcl_cerr << pro.name() << ": Unsupported land cover image pixel format -- " << in_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  vpgl_geo_camera* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    vcl_cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }
  // generate the connected component
  volm_candidate_list cand_list(*in_img, threshold);
  vgl_polygon<int> img_polys = cand_list.cand_poly();
  // transfer from image domain to world domain
  unsigned n_sheet = img_polys.num_sheets();
  vcl_vector<vgl_polygon<double> > polys;
  for (unsigned sidx = 0; sidx < n_sheet; sidx++)
  {
    vcl_vector<vgl_point_2d<int> > sheet = img_polys[sidx];
    vgl_polygon<double> single_poly;
    single_poly.new_sheet();
    for (unsigned vidx = 0; vidx < sheet.size(); vidx++) {
      double lon, lat;
      in_cam->img_to_global(sheet[vidx].x(), sheet[vidx].y(), lon, lat);
      single_poly.push_back(lon, lat);
    }
    polys.push_back(single_poly);
  }
  // write to kml
  vcl_ofstream ofs(out_file.c_str());
  if (!ofs.is_open()) {
    vcl_cerr << pro.name() << ": Failed to write " << out_file << "\n!";
    return false;
  }
  bkml_write::open_document(ofs);
  for (unsigned i = 0; i < polys.size(); i++) {
    bkml_write::write_polygon(ofs, polys[i], "polygon", "", 1.0, 3.0, 0.45, r, g, b);
  }
  bkml_write::close_document(ofs);
  ofs.close();

  // output
  pro.set_output_val<unsigned>(0, polys.size());
  return true;
}

//: process to downsample the generate land layer image
namespace volm_downsample_binary_layer_process_globals
{
  unsigned n_inputs_  = 6;
  unsigned n_outputs_ = 0;
}

bool volm_downsample_binary_layer_process_cons(bprb_func_process& pro)
{
  using namespace volm_downsample_binary_layer_process_globals;
  // this process takes 6 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";  // input layer image (byte image)
  input_types_[1] = "vil_image_view_base_sptr";  // input layer mask (byte image)
  input_types_[2] = "vpgl_camera_double_sptr";   // geo camera of the input image
  input_types_[3] = "vil_image_view_base_sptr";  // down-sample image (byte image)
  input_types_[4] = "vil_image_view_base_sptr";  // down-sample mask (byte image)
  input_types_[5] = "vpgl_camera_double_sptr";   // geo camera of down-sample image
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_downsample_binary_layer_process(bprb_func_process& pro)
{
  using namespace volm_downsample_binary_layer_process_globals;
  if (!pro.verify_inputs()) {
    vcl_cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  vil_image_view_base_sptr in_img_sptr   = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr in_mask_sptr  = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  in_cam_sptr   = pro.get_input<vpgl_camera_double_sptr>(in_i++);
  vil_image_view_base_sptr out_img_sptr  = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vil_image_view_base_sptr out_mask_sptr = pro.get_input<vil_image_view_base_sptr>(in_i++);
  vpgl_camera_double_sptr  out_cam_sptr  = pro.get_input<vpgl_camera_double_sptr>(in_i++);

  vil_image_view<vxl_byte>* in_img = dynamic_cast<vil_image_view<vxl_byte>*>(in_img_sptr.ptr());
  if (!in_img) {
    vcl_cerr << pro.name() << ": Unsupported land cover image pixel format -- " << in_img_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  vil_image_view<vxl_byte>* in_mask = dynamic_cast<vil_image_view<vxl_byte>*>(in_mask_sptr.ptr());
  if (!in_mask) {
    vcl_cerr << pro.name() << ": Unsupported land cover mask image pixel format -- " << in_mask_sptr->pixel_format() << ", only Byte is supported!\n";
    return false;
  }
  vpgl_geo_camera* in_cam = dynamic_cast<vpgl_geo_camera*>(in_cam_sptr.ptr());
  if (!in_cam) {
    vcl_cerr << pro.name() << ": can not load land cover image camera!\n";
    return false;
  }

  vil_image_view<vxl_byte>* out_img = dynamic_cast<vil_image_view<vxl_byte>*>(out_img_sptr.ptr());
  if (!out_img) {
    vcl_cerr << pro.name() << ": Unsupported down sample image pixel format -- " << out_img_sptr->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  vil_image_view<vxl_byte>* out_mask = dynamic_cast<vil_image_view<vxl_byte>*>(out_mask_sptr.ptr());
  if (!out_mask) {
    vcl_cerr << pro.name() << ": Unsupported down sample mask image pixel format -- " << out_mask_sptr->pixel_format() << ", only byte is supported!\n";
    return false;
  }
  vpgl_geo_camera* out_cam = dynamic_cast<vpgl_geo_camera*>(out_cam_sptr.ptr());
  if (!out_cam) {
    vcl_cerr << pro.name() << ": can not load down sample image camera!\n";
    return false;
  }

  // start to fill the image
  unsigned i_ni = in_img->ni();
  unsigned i_nj = in_img->nj();
  unsigned o_ni = out_img->ni();
  unsigned o_nj = out_img->nj();
  vcl_cout << "Start to fill the image..." << vcl_flush << vcl_endl;
  for (unsigned i = 0; i < i_ni; i++) {
    if (i%1000 == 0)
      vcl_cout << i << '.' << vcl_flush;
    for (unsigned j = 0; j < i_nj; j++) {
      double lon, lat;
      in_cam->img_to_global(i, j, lon, lat);
      double u, v;
      out_cam->global_to_img(lon, lat, 0.0, u, v);
      unsigned uu = (unsigned)vcl_floor(u+0.5);
      unsigned vv = (unsigned)vcl_floor(v+0.5);
      if ( uu >= o_ni || vv >= o_nj)
        continue;
      (*out_img)(uu,vv) =  (*in_img)(i,j);
      (*out_mask)(uu,vv) = (*in_mask)(i,j);
    }
  }
  vcl_cout << "DONE!" << vcl_endl;

  return true;
}
