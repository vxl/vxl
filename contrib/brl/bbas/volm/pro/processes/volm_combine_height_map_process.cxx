// This is brl/bbas/volm/pro/processes/volm_combine_height_map.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//          combine height maps having small size to larger height maps
//
//
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>
#include <volm/volm_geo_index2.h>
#include <volm/volm_loc_hyp_sptr.h>
#include <volm/volm_io_tools.h>
#include <vgl/vgl_intersection.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>

namespace volm_combine_height_map_process_globals
{
  const unsigned n_inputs_  = 5;
  const unsigned n_outputs_ = 0;
}

bool volm_combine_height_map_process_cons(bprb_func_process& pro)
{
  using namespace volm_combine_height_map_process_globals;
  // process takes 4 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // input height map folder
  input_types_[1] = "vcl_string";  // polygon region kml
  input_types_[2] = "vcl_string";  // output folder
  input_types_[3] = "float";       // desired map size (in degree)
  input_types_[4] = "int";         // leaf id
  // process takes 0 outputs
  vcl_vector<vcl_string> output_types_(n_outputs_);

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
  vcl_string dem_folder = pro.get_input<vcl_string>(in_i++);
  vcl_string poly_file  = pro.get_input<vcl_string>(in_i++);
  vcl_string out_folder = pro.get_input<vcl_string>(in_i++);
  float min_size = pro.get_input<float>(in_i++);
  int   leaf_id  = pro.get_input<int>(in_i++);

  // get the images
  vcl_vector<volm_img_info> h_info;
  volm_io_tools::load_imgs(dem_folder, h_info, true, true, false);
  vcl_cout << h_info.size() << " height maps are loaded from " << dem_folder << vcl_flush << vcl_endl;

  // construct geo index based on polygon
  vgl_polygon<double> poly = bkml_parser::parse_polygon(poly_file);
  vgl_box_2d<double> bbox_rect;
  for (unsigned i = 0; i < poly[0].size(); i++)
    bbox_rect.add(poly[0][i]);
  // truncate the bbox
  int box_min_lon = vcl_floor(bbox_rect.min_x());
  int box_min_lat = vcl_floor(bbox_rect.min_y());
  int box_max_lon =  vcl_ceil(bbox_rect.max_x());
  int box_max_lat =  vcl_ceil(bbox_rect.max_y());
  vgl_box_2d<double> bbox_extend(box_min_lon, box_max_lon, box_min_lat, box_max_lat);

  double square_size = (bbox_extend.width() >= bbox_extend.height()) ? bbox_extend.width() : bbox_extend.height();
  vgl_box_2d<double> bbox(bbox_extend.min_point(), square_size, square_size, vgl_box_2d<double>::min_pos);

  volm_geo_index2_node_sptr root = volm_geo_index2::construct_tree<volm_loc_hyp_sptr>(bbox, min_size, poly);
  vcl_vector<volm_geo_index2_node_sptr> leaves;
  volm_geo_index2::get_leaves(root, leaves, poly);
  vcl_cout << "ROI poly has " << poly[0].size() << " vertices and loaded from " << poly_file << vcl_endl;
  vcl_cout << "Bounding box for input polygon: " << bbox_rect << " expending to square: " << bbox << vcl_endl;
  vcl_cout << leaves.size() << " are created, each leaf has size: " << min_size << vcl_endl;

  // write out the geo index
  vcl_string txt_filename = out_folder + "/geo_index.txt";
  vcl_string kml_filename = out_folder + "/height_map_geo_index.kml";
  if (!vul_file::exists(txt_filename))
    volm_geo_index2::write(root, txt_filename, min_size);
  if (!vul_file::exists(kml_filename)) {
    vcl_ofstream ofs(kml_filename.c_str());
    bkml_write::open_document(ofs);
    for (unsigned i = 0; i < leaves.size(); i++) {
      vcl_stringstream explanation; explanation << "leaf_" << vcl_setprecision(12) << leaves[i]->extent_.min_x() << "_"
                                                           << vcl_setprecision(12) << leaves[i]->extent_.min_y();
      volm_geo_index2::write_to_kml_node(ofs, leaves[i], 0, 0, explanation.str());
    }
    bkml_write::close_document(ofs);
  }

  vcl_cout << " --------------- Start to create map for leaf " << leaf_id << " -----------------" << vcl_endl;
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
    unsigned ni = (unsigned)vcl_ceil(box_lx);
    unsigned nj = (unsigned)vcl_ceil(box_ly);
    vgl_box_2d<double> leaf_bbox_wgs = leaf->extent_;
    vgl_box_2d<double> leaf_bbox(0.0, box_lx, 0.0, box_ly);
    // create a filename
    vcl_stringstream filename;
    vcl_string hemisphere = "N";
    vcl_string direction  = "E";
    if (lon_min < 0)  direction  = "W";
    if (lat_min < 0)  hemisphere = "S";
    filename << "HeightMap_" << hemisphere << vcl_setprecision(12) << lat_min << direction << vcl_setprecision(12) << lon_min
             << "_S" << scale_x << 'x' << scale_y;
    // create geo camera for output image
    vnl_matrix<double> trans_matrix(4,4,0,0);
    trans_matrix[0][0] = scale_x/ni;    trans_matrix[1][1] = -scale_y/nj;
    trans_matrix[0][3] = lon_min;       trans_matrix[1][3] = lat_max;
    vpgl_geo_camera* cam = new vpgl_geo_camera(trans_matrix, lvcs);
    cam->set_scale_format(true);

    // create the image
    vil_image_view<float> out_img(ni, nj, 1);
    out_img.fill(-1.0f);
    vcl_cout << "\tleaf " << l_idx << " has geo boundary " << leaf_bbox_wgs << " --> image size " << out_img.ni() << 'x' << out_img.nj() << vcl_endl;

    // check height maps that intersect with current leaf
    vcl_vector<volm_img_info> leaf_h_info;
    for (vcl_vector<volm_img_info>::iterator vit = h_info.begin();  vit != h_info.end();  ++vit)
    {
      if (vgl_intersection(vit->bbox, leaf_bbox_wgs).area() <= 0.0) {
        continue;
      }
      if (vit->img_r->pixel_format() != VIL_PIXEL_FORMAT_FLOAT) {
        vcl_cerr << pro.name() << ": unsupported height map image pixel: " << vit->img_r->pixel_format() << vcl_endl;
        return false;
      }
      leaf_h_info.push_back(*vit);
    }
    if (leaf_h_info.empty()) {
      vcl_cout << "No height map intersects with current leaf, ignore" << vcl_endl;
      continue;
    }
    // ingest height map onto the image
    vcl_cout << leaf_h_info.size() << " height maps intersect with current leaf" << vcl_endl;
    for (vcl_vector<volm_img_info>::iterator vit = leaf_h_info.begin();  vit != leaf_h_info.end();  ++vit)
      vcl_cout << "\t\t" << vit->name << " --> " << vit->bbox << vcl_endl;

    for (unsigned i = 0; i < ni; i++)
    {
      if (i%200==0)
        vcl_cout << '.' << vcl_flush;
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
          unsigned uu = (unsigned)vcl_floor(u+0.5);
          unsigned vv = (unsigned)vcl_floor(v+0.5);
          if (uu < leaf_h_info[h_idx].ni && vv < leaf_h_info[h_idx].nj) {
            is_pixel_found = true;
            vil_image_view<float> imgc(leaf_h_info[h_idx].img_r);
            out_img(i,j) = imgc(uu,vv);
          }
        }
      }
    }

    // use neighbor pixel to fix gaps
    vcl_cout << "\nfill the artificial gap" << vcl_endl;
    vil_image_view<float> out_img_fill(ni,nj);
    out_img_fill.deep_copy(out_img);
    unsigned num_nbrs = 8;
    int r_idx = 1;
    int nbrs8_delta[8][2] = { { r_idx, 0}, { r_idx,-r_idx}, { 0,-r_idx}, {-r_idx,-r_idx},
                              {-r_idx, 0}, {-r_idx, r_idx}, { 0, r_idx}, { r_idx, r_idx} };
    for (unsigned i = 0; i < ni; i++) {
      if (i%200==0) vcl_cout << '.' << vcl_flush;
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
    vcl_string img_filename = out_folder + "/" + filename.str() + ".tif";
    vil_save(out_img_fill, img_filename.c_str());
    vcl_cout << "\nimage is saved at: " << img_filename << vcl_endl;
    vcl_string cam_name = out_folder + "/" + filename.str() + ".tfw";
    cam->save_as_tfw(cam_name);
  }

  return true;
}