// This is brl/bseg/bvxm/pro/processes/bvxm_scene_kml_process.cxx
#include "bvxm_scene_kml_process.h"
//:
// \file
#include <vgl/vgl_box_3d.h>
#include <vul/vul_file.h>
#include <vgl/vgl_intersection.h>

// set input and output type
bool bvxm_scene_kml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_kml_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";      // voxel world spec
  input_types_[1] = "vcl_string";                 // kml filename
  input_types_[2] = "bool";                       // option to replace all previous content in kml file
  input_types_[3] = "unsigned";                   // color index r
  input_types_[4] = "unsigned";                   // color index g
  input_types_[5] = "unsigned";                   // color index b
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_scene_kml_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_kml_process_globals;
  // sanity check
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string kml_file = pro.get_input<vcl_string>(i++);
  bool is_overwrite = pro.get_input<bool>(i++);
  unsigned r = pro.get_input<unsigned>(i++);
  unsigned g = pro.get_input<unsigned>(i++);
  unsigned b = pro.get_input<unsigned>(i++);

  // obtain the bounding box for the scene region
  bvxm_world_params_sptr params = voxel_world->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();
  vgl_point_3d<float> corner = params->corner();
  vgl_vector_3d<unsigned> num_voxels = params->num_voxels();
  float voxel_length = params->voxel_length();
  double low_left_lon, low_left_lat, low_left_alt, upp_rght_lon, upp_rght_lat, upp_rght_alt;
  lvcs->local_to_global(corner.x(), corner.y(), corner.z(), vpgl_lvcs::wgs84, low_left_lon, low_left_lat, low_left_alt);
  double dim_x = num_voxels.x()*voxel_length;
  double dim_y = num_voxels.y()*voxel_length;
  double dim_z = num_voxels.z()*voxel_length;
  lvcs->local_to_global(corner.x()+dim_x, corner.y()+dim_y, corner.z()+dim_z, vpgl_lvcs::wgs84, upp_rght_lon, upp_rght_lat, upp_rght_alt);

  vgl_box_3d<double> bbox(low_left_lat, low_left_lon, low_left_alt, upp_rght_lat, upp_rght_lon, upp_rght_alt);

  
  vnl_double_2 ll;  ll[0] = bbox.min_point().x();  ll[1] = bbox.min_point().y();  // low_left
  vnl_double_2 lr;  lr[0] = bbox.max_point().x();  lr[1] = bbox.min_point().y();  // low_right
  vnl_double_2 ur;  ur[0] = bbox.max_point().x();  ur[1] = bbox.max_point().y();  // upp_right
  vnl_double_2 ul;  ul[0] = bbox.min_point().x();  ul[1] = bbox.max_point().y();  // upp_left

  vcl_stringstream box_info;
  box_info << "origin: " << low_left_lon << "x" << low_left_lat;
  vcl_string desc = "bvxm_scene";

  // write to kml file
  if (vul_file::exists(kml_file) && !is_overwrite) {
    // read all previous box first and store them into a polygon
    vgl_polygon<double> poly = bkml_parser::parse_polygon(kml_file);

    // write the previous content with same format
    vcl_ofstream ofs(kml_file.c_str(), vcl_ios_trunc);
    bkml_write::open_document(ofs);
    unsigned num_sheet = poly.num_sheets();
    for (unsigned i = 0; i < num_sheet; i++) {
      vnl_double_2 ul_t(poly[i][0].y(), poly[i][0].x());
      vnl_double_2 ur_t(poly[i][1].y(), poly[i][1].x());
      vnl_double_2 lr_t(poly[i][2].y(), poly[i][2].x());
      vnl_double_2 ll_t(poly[i][3].y(), poly[i][3].x());
      vcl_stringstream box_info_t;
      box_info_t << "origin: " << ll_t[1] << "x" << ll_t[0];
      bkml_write::write_box(ofs, box_info_t.str(), desc, ul_t, ur_t, ll_t, lr_t, (unsigned char)r, (unsigned char)g, (unsigned char)b);
    }
    bkml_write::write_box(ofs, box_info.str(), desc, ul, ur, ll, lr, (unsigned char)r, (unsigned char)g, (unsigned char)b);
    bkml_write::close_document(ofs);
    ofs.close();
  }
  else {
    vcl_ofstream ofs(kml_file.c_str());
    bkml_write::open_document(ofs);
    bkml_write::write_box(ofs, box_info.str(), desc, ul, ur, ll, lr, (unsigned char)r, (unsigned char)g, (unsigned char)b);
    bkml_write::close_document(ofs);
    ofs.close();
  }
  return true;
}

bool bvxm_scene_poly_overlap_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_poly_overlap_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";  // bvxm voxel world spec
  input_types_[1] = "vcl_string";             // input kml polygon
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvxm_scene_poly_overlap_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_poly_overlap_process_globals;
  // sanity check
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  vcl_string kml_file = pro.get_input<vcl_string>(i++);

  if (!vul_file::exists(kml_file)) {
    vcl_cout << pro.name() << " can not find input kml file: " << kml_file << vcl_endl;
    return false;
  }

  // read the polygon from kml
  vgl_polygon<double> poly = bkml_parser::parse_polygon(kml_file);

  // obtain the bounding box for the scene region
  bvxm_world_params_sptr params = voxel_world->get_params();
  vpgl_lvcs_sptr lvcs = params->lvcs();
  vgl_point_3d<float> corner = params->corner();
  vgl_vector_3d<unsigned> num_voxels = params->num_voxels();
  float voxel_length = params->voxel_length();
  double low_left_lon, low_left_lat, low_left_alt, upp_rght_lon, upp_rght_lat, upp_rght_alt;
  lvcs->local_to_global(corner.x(), corner.y(), corner.z(), vpgl_lvcs::wgs84, low_left_lon, low_left_lat, low_left_alt);
  double dim_x = num_voxels.x()*voxel_length;
  double dim_y = num_voxels.y()*voxel_length;
  double dim_z = num_voxels.z()*voxel_length;
  lvcs->local_to_global(corner.x()+dim_x, corner.y()+dim_y, corner.z()+dim_z, vpgl_lvcs::wgs84, upp_rght_lon, upp_rght_lat, upp_rght_alt);

  //vgl_box_2d<double> bbox(low_left_lon, low_left_lat, upp_rght_lon, upp_rght_lat);
  vgl_box_2d<double> bbox(low_left_lon, upp_rght_lon, low_left_lat, upp_rght_lat);
#if 0
  vcl_cout << " lower_left = " << low_left_lon << "," << low_left_lat << vcl_endl;
  vcl_cout << " upper_rght = " << upp_rght_lon << "," << upp_rght_lat << vcl_endl;
  vcl_cout << " bbox = " << bbox << vcl_endl;
  poly.print(vcl_cout);
#endif

  return vgl_intersection(bbox, poly);
}