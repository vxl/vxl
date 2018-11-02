// This is brl/bbas/volm/pro/processes/volm_combine_height_map.cxx
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
//          combine height maps having small size to larger height maps
//
//
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_io_tools.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_area.h>

namespace volm_combine_height_map_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool volm_combine_height_map_process_cons(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process_globals;
  // process takes 4 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // input height map folder
  input_types_[1] = "vcl_string";  // polygon region kml
  input_types_[2] = "vcl_string";  // output folder
  input_types_[3] = "float";       // desired map size (in degree)
  input_types_[4] = "int";         // leaf id
  // process takes 0 outputs
  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_combine_height_map_process(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process_globals;
  // sanity check
  if (!pro.verify_inputs())
    return false;
  // get the input
  unsigned in_i = 0;
  std::string dem_folder = pro.get_input<std::string>(in_i++);
  std::string poly_file = pro.get_input<std::string>(in_i++);
  std::string out_folder = pro.get_input<std::string>(in_i++);
  auto min_size = pro.get_input<float>(in_i++);
  int   leaf_id = pro.get_input<int>(in_i++);

  // get the images
  std::vector<volm_img_info> h_info;
  volm_io_tools::load_imgs(dem_folder, h_info, true, true, false);
  std::cout << h_info.size() << " height maps are loaded from " << dem_folder << std::flush << std::endl;

  // construct geo index based on polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  vgl_box_2d<double> bbox_rect;
  for (auto i : poly[0])
    bbox_rect.add(i);
  // truncate the bbox
  int box_min_lon = std::floor(bbox_rect.min_x());
  int box_min_lat = std::floor(bbox_rect.min_y());
  int box_max_lon =  std::ceil(bbox_rect.max_x());
  int box_max_lat =  std::ceil(bbox_rect.max_y());
  vgl_box_2d<double> bbox_extend(box_min_lon, box_max_lon, box_min_lat, box_max_lat);

  double square_size = (bbox_extend.width() >= bbox_extend.height()) ? bbox_extend.width() : bbox_extend.height();
  vgl_box_2d<double> bbox(bbox_extend.min_point(), square_size, square_size, vgl_box_2d<double>::min_pos);

  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  std::vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves, poly);
  std::cout << "ROI poly has " << poly[0].size() << " vertices and loaded from " << poly_file << std::endl;
  std::cout << "Bounding box for input polygon: " << bbox_rect << " expending to square: " << bbox << std::endl;
  std::cout << leaves.size() << " are created, each leaf has size: " << min_size << std::endl;

  // write out the geo index
  std::string txt_filename = out_folder + "/geo_index.txt";
  std::string kml_filename = out_folder + "/height_map_geo_index.kml";
  if (!vul_file::exists(txt_filename))
    volm_geo_index2::write(root, txt_filename, min_size);
  if (!vul_file::exists(kml_filename)) {
    std::ofstream ofs(kml_filename.c_str());
    bkml_write::open_document(ofs);
    for (auto & leave : leaves) {
      std::stringstream explanation; explanation << "leaf_" << std::setprecision(12) << leave->extent_.min_x() << "_"
                                                           << std::setprecision(12) << leave->extent_.min_y();
      volm_geo_index2::write_to_kml_node(ofs, leave, 0, 0, explanation.str());
    }
    bkml_write::close_document(ofs);
  }

  std::cout << " --------------- Start to create map for leaf " << leaf_id << " -----------------" << std::endl;
  for (unsigned l_idx = 0; l_idx < leaves.size(); l_idx++)
  {
    if (leaf_id > 0 && leaf_id < (int)leaves.size())
      if ( (int)l_idx != leaf_id)
        continue;
    // calculate desired resolution
    volm_geo_index2_node_sptr leaf = leaves[l_idx];
    double lon_min, lat_min, lon_max, lat_max;
    lon_min = leaf->extent_.min_x();  lat_min = leaf->extent_.min_y();
    lon_max = leaf->extent_.max_x();  lat_max = leaf->extent_.max_y();
    double scale_x = lon_max - lon_min;
    double scale_y = lat_max - lat_min;
    vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lat_min, lon_min, 0, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
    double box_lx, box_ly, box_lz;
    lvcs->global_to_local(lon_max, lat_max, 0, vpgl_lvcs::wgs84, box_lx, box_ly, box_lz);
    auto ni = (unsigned)std::ceil(box_lx);
    auto nj = (unsigned)std::ceil(box_ly);
    vgl_box_2d<double> leaf_bbox_wgs = leaf->extent_;
    vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);
    // create a filename
    std::stringstream filename;
    std::string hemisphere = "N";
    std::string direction = "E";
    if (lon_min < 0)  direction = "W";
    if (lat_min < 0)  hemisphere = "S";
    filename << "HeightMap_" << hemisphere << std::setprecision(12) << lat_min << direction << std::setprecision(12) << lon_min
             << "_S" << scale_x << 'x' << scale_y;
    // create geo camera for output image
    vnl_matrix<double> trans_matrix(4,4,0,nullptr);
    trans_matrix[0][0] = scale_x/ni;    trans_matrix[1][1] = -scale_y/nj;
    trans_matrix[0][3] = lon_min;       trans_matrix[1][3] = lat_max;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    cam->set_scale_format(true);

    // create the image
    vil_image_view<float> out_img(ni, nj, 1);
    out_img.fill(-1.0f);
    std::cout << "\tleaf " << l_idx << " has geo boundary " << leaf_bbox_wgs << " --> image size " << out_img.ni() << 'x' << out_img.nj() << std::endl;

    // check height maps that intersect with current leaf
    std::vector<volm_img_info> leaf_h_info;
    for (auto & vit : h_info)
    {
      if (vgl_area(vgl_intersection(vit.bbox, leaf_bbox_wgs)) <= 0.0) {
        continue;
      }
      if (vit.img_r->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
        std::cerr << pro.name() << ": unsupported height map image pixel: " << vit.img_r->pixel_format() << std::endl;
        return false;
      }
      leaf_h_info.push_back(vit);
    }
    if (leaf_h_info.empty()) {
      std::cout << "No height map intersects with current leaf, ignore" << std::endl;
      continue;
    }
    // ingest height map onto the image
    std::cout << leaf_h_info.size() << " height maps intersect with current leaf" << std::endl;
    for (auto & vit : leaf_h_info)
      std::cout << "\t\t" << vit.name << " --> " << vit.bbox << std::endl;

    for (unsigned i = 0; i < ni; i++)
    {
      if (i%200==0)
        std::cout << '.' << std::flush;
      for (unsigned j = 0; j < nj; j++)
      {
        double lon, lat, gz;
        float local_x = i+0.5f;  float local_y = box_ly-j+0.5;
        lvcs->local_to_global(local_x, local_y, 0.0, vpgl_lvcs::wgs84, lon, lat, gz);
        bool is_pixel_found = false;
        for (unsigned h_idx = 0; (h_idx < leaf_h_info.size() && !is_pixel_found); h_idx++) {
          if (!leaf_h_info[h_idx].bbox.contains(lon, lat))
            continue;
          double u, v;
          leaf_h_info[h_idx].cam->global_to_img(lon, lat, gz, u, v);
          auto uu = (unsigned)std::floor(u+0.5);
          auto vv = (unsigned)std::floor(v+0.5);
          if (uu < leaf_h_info[h_idx].ni && vv < leaf_h_info[h_idx].nj) {
            is_pixel_found = true;
            vil_image_view<float> imgc(leaf_h_info[h_idx].img_r);
            out_img(i,j) = imgc(uu,vv);
          }
        }
      }
    }

    // use neighbor pixel to fix gaps
    std::cout << "\nfill the artificial gap" << std::endl;
    vil_image_view<float> out_img_fill(ni,nj);
    out_img_fill.deep_copy(out_img);
    unsigned num_nbrs = 8;
    int r_idx = 1;
    int nbrs8_delta[8][2] = { { r_idx, 0}, { r_idx,-r_idx}, { 0,-r_idx}, {-r_idx,-r_idx},
                              {-r_idx, 0}, {-r_idx, r_idx}, { 0, r_idx}, { r_idx, r_idx} };
    for (unsigned i = 0; i < ni; i++) {
      if (i%200==0) std::cout << '.' << std::flush;
      for (unsigned j = 0; j < nj; j++) {
        if (out_img(i,j) > 0)
          continue;
        float neigh_values = 0;
        unsigned cnt = 0;
        for (unsigned c = 0; c < num_nbrs; c++) {
          int nbr_i = i + nbrs8_delta[c][0];
          int nbr_j = j + nbrs8_delta[c][1];
          if (nbr_i > 0 && nbr_j > 0 && nbr_i < ni && nbr_j < nj) {
            if (out_img(nbr_i, nbr_j) > 0) {
              neigh_values += out_img(nbr_i,nbr_j);
              cnt++;
            }
          }
        }
        if (cnt != 0)  out_img_fill(i,j) = neigh_values/cnt;
      }
    }
    // save the image
    std::string img_filename = out_folder + "/" + filename.str() + ".tif";
    vil_save(out_img_fill, img_filename.c_str());
    std::cout << "\nimage is saved at: " << img_filename << std::endl;
    std::string cam_name = out_folder + "/" + filename.str() + ".tfw";
    cam->save_as_tfw(cam_name);
  }

  return true;
}


// combine multiple height maps by taking the median values of all pixels from multiple maps
namespace volm_combine_height_map_process2_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;

  //: function to obtain the median
  float median(std::vector<float> values);
}

bool volm_combine_height_map_process2_cons(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process2_globals;
  // process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // folder that stores all height maps
  input_types_[1] = "float";       // threshold
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";  // output image
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_combine_height_map_process2(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process2_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Input!!!" << std::endl;
    return false;
  }
  // get the input
  unsigned in_i = 0;
  std::string img_folder = pro.get_input<std::string>(in_i++);
  auto threshold = pro.get_input<float>(in_i++);
  // get all images from the folder
  std::vector<std::string> img_files;
  std::string in_dir = img_folder + "*.tif";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn)
    img_files.emplace_back(fn());
  if (img_files.size() == 0) {
    std::cerr << pro.name() << ": No image at folder: " << img_folder << std::endl;
    return false;
  }
  // load the images
  std::vector<vil_image_view<float> > in_imgs;
  for (auto & img_file : img_files) {
    vil_image_view<float> in_img = vil_load(img_file.c_str());
    in_imgs.push_back(in_img);
  }
  std::cout << in_imgs.size() << " images are loaded" << std::endl;
  std::cout << "invalid pixel threshold: " << threshold << std::endl;
  // verify the image size
  unsigned ni = in_imgs[0].ni();
  unsigned nj = in_imgs[0].nj();
  for (unsigned i = 1; i < in_imgs.size(); i++)
    if (in_imgs[i].ni() != ni || in_imgs[i].nj() != nj) {
      std::cerr << pro.name() << ": image size mismatch!\n";
      return false;
    }

  // obtain the median
  auto* out_img = new vil_image_view<float>(ni, nj);
  out_img->fill(-1.0f);
  for (unsigned i = 0; i < ni; i++)
  {
    for (unsigned j = 0; j < nj; j++)
    {
      std::vector<float> pixel_values;
      for (auto & in_img : in_imgs)
        if ( (in_img(i,j)-threshold)*(in_img(i,j)-threshold) > 1E-5 )
          pixel_values.push_back( in_img(i,j) );
      float median_value = median(pixel_values);
      (*out_img)(i,j) = median_value;
    }
  }

  // generate output
  vil_image_view_base_sptr out_img_sptr = new vil_image_view<float>(out_img);
  pro.set_output_val<vil_image_view_base_sptr>(0, out_img_sptr);
  return true;
}

float volm_combine_height_map_process2_globals::median(std::vector<float> values)
{
  std::sort(values.begin(), values.end());
  int size = values.size();
  int s2 = size / 2;
  return size == 0 ? 0.0 : size%2 ? values[s2] : (values[s2]+values[s2-1])*0.5;
}


// process to mosaics a set of images that covers the given region
// the input images need to be geotiff with geo_camera embedded in there headers
namespace volm_combine_height_map_process3_globals
{
  unsigned n_inputs_ = 6;
  unsigned n_output_ = 2;
}

bool volm_combine_height_map_process3_cons(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process3_globals;
  // this process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";   // input image folder
  input_types_[1] = "double";       // lower left lon
  input_types_[2] = "double";       // lower left lat
  input_types_[3] = "double";       // upper right lon
  input_types_[4] = "double";       // upper right lat
  input_types_[5] = "float";        // initialize the output image with this value
  // this process takes 2 outputs
  std::vector<std::string> output_types_(n_output_);
  output_types_[0] = "vil_image_view_base_sptr";  // output image
  output_types_[1] = "vpgl_camera_double_sptr";   // output geo camera
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool volm_combine_height_map_process3(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process3_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  std::string in_img_folder = pro.get_input<std::string>(in_i++);
  auto            ll_lon = pro.get_input<double>(in_i++);
  auto            ll_lat = pro.get_input<double>(in_i++);
  auto            ur_lon = pro.get_input<double>(in_i++);
  auto            ur_lat = pro.get_input<double>(in_i++);
  auto         init_value = pro.get_input<float>(in_i++);

  // load all images in the height map folder
  std::vector<volm_img_info> h_infos;
  volm_io_tools::load_aster_dem_imgs(in_img_folder, h_infos);
  if (h_infos.empty()) {
    std::cerr << pro.name() << ": can not find any tif image in folder: " << in_img_folder << "!\n";
    return false;
  }
  std::cout << h_infos.size() << " images are loaded from " << in_img_folder << std::endl;

  // initialize an image that has same GSD as height map and same coverage of land cover image
  double scale_x = h_infos[0].cam->trans_matrix()[0][0];
  double scale_y = h_infos[0].cam->trans_matrix()[1][1];
  vnl_matrix<double> trans_matrix(4,4,0.0);
  trans_matrix[0][0] = scale_x;
  trans_matrix[1][1] = scale_y;
  trans_matrix[0][3] = ll_lon;
  trans_matrix[1][3] = ur_lat;
  vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
  vpgl_geo_camera* out_cam = new vpgl_geo_camera(trans_matrix, lvcs_dummy);
  out_cam->set_scale_format(true);
  double o_u, o_v;
  out_cam->global_to_img(ur_lon, ll_lat, 0, o_u, o_v);
  unsigned o_ni = std::ceil(o_u);
  unsigned o_nj = std::ceil(o_v);
  auto* out_img = new vil_image_view<float>(o_ni, o_nj);
  out_img->fill(init_value);

  // obtain the overlapped resource
  vgl_box_2d<double> region_box(ll_lon, ur_lon, ll_lat, ur_lat);
  std::vector<volm_img_info> overlap_infos;
  for (auto & h_info : h_infos)
  {
    if (vgl_area(vgl_intersection(region_box, h_info.bbox)) > 0)
      overlap_infos.push_back(h_info);
  }
  if (overlap_infos.empty()) {
    std::cout << "no image overlaps with given region: " << region_box << ", return an empty image" << std::endl;
    pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
    pro.set_output_val<vpgl_camera_double_sptr>(1, out_cam);
    return true;
  }
  std::cout << overlap_infos.size() << " images overlap with given region: " << region_box << std::endl;
  std::cout << "Start to aggregate the images..." << std::endl;
  for (unsigned i = 0; i < o_ni; i++)
  {
    for (unsigned j = 0; j < o_nj; j++)
    {
      double lon, lat;
      out_cam->img_to_global(i, j, lon, lat);
      bool found = false;
      for (auto vit = overlap_infos.begin(); (vit != overlap_infos.end() && !found); ++vit) {
        vgl_box_2d<double> bbox = vit->bbox;
        bbox.expand_about_centroid(2E-5);
        if (!bbox.contains(lon, lat))
          continue;
        double lon, lat;
        out_cam->img_to_global(i, j, lon, lat);
        double u, v;
        vit->cam->global_to_img(lon, lat, 0.0, u, v);
        auto uu = (unsigned)std::floor(u+0.5);
        auto vv = (unsigned)std::floor(v+0.5);
        if (uu < vit->ni && vv < vit->nj) {
          found = true;
          vil_image_view<float> h_img(vit->img_r);
          (*out_img)(i,j) = h_img(uu, vv);
        }
      }
    }
  }
  // output
  pro.set_output_val<vil_image_view_base_sptr>(0, vil_image_view_base_sptr(out_img));
  pro.set_output_val<vpgl_camera_double_sptr>(1, out_cam);
  return true;
}
