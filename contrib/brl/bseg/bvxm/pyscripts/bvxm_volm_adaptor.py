from bvxm_register import bvxm_batch, dbvalue;
import math;

def map_sdet_to_volm_ids(sdet_color_class_img):
  bvxm_batch.init_process("volmGenerateClassMapProcess");
  bvxm_batch.set_input_from_db(0,sdet_color_class_img);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return out_img

def volm_id_color_img(id_img, id_to_color_txt=""):
  bvxm_batch.init_process("volmGenerateColorClassMapProcess")
  bvxm_batch.set_input_from_db(0,id_img);
  bvxm_batch.set_input_string(1,id_to_color_txt)
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return out_img;

def update_class_map(class_img, source_img):
  bvxm_batch.init_process("volmUpdateClassMapProcess");
  bvxm_batch.set_input_from_db(0, class_img);
  bvxm_batch.set_input_from_db(1, source_img);
  status = bvxm_batch.run_process()
  return status;

def load_sat_resources(res_file_name):
  bvxm_batch.init_process("volmLoadSatResourcesProcess");
  bvxm_batch.set_input_string(0,res_file_name);
  bvxm_batch.run_process();
  (res2_id, res2_type) = bvxm_batch.commit_output(0);
  res2 = dbvalue(res2_id, res2_type);
  return res2

def find_resource_pair(res, name):
  bvxm_batch.init_process("volmFindResourcePairProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_string(1,name);
  statuscode=bvxm_batch.run_process();
  (f_id, f_type) = bvxm_batch.commit_output(0);
  full_path = bvxm_batch.get_output_string(f_id);
  bvxm_batch.remove_data(f_id);
  (n_id, n_type) = bvxm_batch.commit_output(1);
  pair_name = bvxm_batch.get_output_string(n_id);
  bvxm_batch.remove_data(n_id);
  (p_id, p_type) = bvxm_batch.commit_output(2);
  full_path_pair_name = bvxm_batch.get_output_string(p_id);
  bvxm_batch.remove_data(p_id);
  return statuscode, full_path, pair_name, full_path_pair_name

def get_full_path(res, name):
  bvxm_batch.init_process("volmGetFullPathProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_string(1,name);
  statuscode=bvxm_batch.run_process();
  (f_id, f_type) = bvxm_batch.commit_output(0);
  full_path = bvxm_batch.get_output_string(f_id);
  bvxm_batch.remove_data(f_id);
  return full_path

## band_name is PAN or MULTI
def pick_nadir_resource(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, satellite_name, band_name="PAN", non_cloud_folder=""):
  bvxm_batch.init_process("volmPickNadirResProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_double(1, lower_left_lon);
  bvxm_batch.set_input_double(2, lower_left_lat);
  bvxm_batch.set_input_double(3, upper_right_lon);
  bvxm_batch.set_input_double(4, upper_right_lat);
  bvxm_batch.set_input_string(5, band_name);
  bvxm_batch.set_input_string(6, satellite_name);
  bvxm_batch.set_input_string(7, non_cloud_folder);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  sat_path = bvxm_batch.get_output_string(id);
  return sat_path;

## find the PAN/MULTI pair for given rectangular region, also output the sorted list of such PAN/MULIT lists
def pick_nadir_resource_pair(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, satellite_name, out_folder, band_name="PAN", non_cloud_folder=""):
  bvxm_batch.init_process("volmPickNadirResPairProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_double(1, lower_left_lon);
  bvxm_batch.set_input_double(2, lower_left_lat);
  bvxm_batch.set_input_double(3, upper_right_lon);
  bvxm_batch.set_input_double(4, upper_right_lat);
  bvxm_batch.set_input_string(5, band_name);
  bvxm_batch.set_input_string(6, satellite_name);
  bvxm_batch.set_input_string(7, non_cloud_folder);
  bvxm_batch.set_input_string(8, out_folder)
  statuscode = bvxm_batch.run_process();
  if statuscode:
    (p_id, p_type) = bvxm_batch.commit_output(0);
    pan_path = bvxm_batch.get_output_string(p_id);
    bvxm_batch.remove_data(p_id);
    (m_id, m_type) = bvxm_batch.commit_output(1);
    multi_path = bvxm_batch.get_output_string(m_id);
    bvxm_batch.remove_data(m_id);
  else:
    pan_path = "";
    multi_path = "";
  return statuscode, pan_path, multi_path

## GSD: ground sampling distance, e.g. pass 1 to eliminate all the images which have pixel GSD more than 1 meter; the default is 10 meters, so practically returns all the satellite images
def scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, scene_res_file, band="PAN",GSD_threshold=10.0, pick_seeds=0, n_seeds=0):
  bvxm_batch.init_process("volmQuerySatelliteResourcesProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_double(1, lower_left_lon);
  bvxm_batch.set_input_double(2, lower_left_lat);
  bvxm_batch.set_input_double(3, upper_right_lon);
  bvxm_batch.set_input_double(4, upper_right_lat);
  bvxm_batch.set_input_string(5, scene_res_file);
  bvxm_batch.set_input_string(6, band);
  bvxm_batch.set_input_bool(7, pick_seeds);  ## of 0, it returns all resources that intersect the box, otherwise, it picks n_seeds among these resources
  bvxm_batch.set_input_int(8, n_seeds);
  bvxm_batch.set_input_double(9, GSD_threshold);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  cnt = bvxm_batch.get_output_unsigned(id);
  return cnt;

def scene_resources2(res, poly_kml, scene_res_file, band="PAN", GSD_threshold=10.0, pick_seeds=0, n_seeds=0):
  bvxm_batch.init_process("volmQuerySatelliteResourceKmlProcess");
  bvxm_batch.set_input_from_db(0, res);
  bvxm_batch.set_input_string(1, poly_kml);
  bvxm_batch.set_input_string(2, scene_res_file);
  bvxm_batch.set_input_string(3, band);
  bvxm_batch.set_input_bool(4, pick_seeds);
  bvxm_batch.set_input_int(5, n_seeds);
  bvxm_batch.set_input_double(6, GSD_threshold);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0)
  cnt = bvxm_batch.get_output_unsigned(id);
  return cnt;

def find_stereo_pairs(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, GSD_threshold, scene_res_file, satellite_name):
  bvxm_batch.init_process("volmFindSatellitePairsProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_double(1, lower_left_lon);
  bvxm_batch.set_input_double(2, lower_left_lat);
  bvxm_batch.set_input_double(3, upper_right_lon);
  bvxm_batch.set_input_double(4, upper_right_lat);
  bvxm_batch.set_input_string(5, scene_res_file);
  bvxm_batch.set_input_string(6, satellite_name);
  bvxm_batch.set_input_float(7, GSD_threshold);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  cnt = bvxm_batch.get_output_unsigned(id);
  return cnt;

def correct_ransac_process(res, cor_file, output_folder, pixel_radius):
  bvxm_batch.init_process("volmCorrectRationalCamerasRANSACProcess");
  bvxm_batch.set_input_from_db(0, res);
  bvxm_batch.set_input_string(1, cor_file);
  bvxm_batch.set_input_string(2, output_folder);
  bvxm_batch.set_input_float(3, pixel_radius);  ## pixel radius to count for inliers
  bvxm_batch.run_process();

## this one checks if the camera is already corrected and exists in the output folder
## weights the cameras accordingly
def correct_ransac_process2(res, cor_file, output_folder, pixel_radius):
  bvxm_batch.init_process("volmCorrectRationalCamerasRANSACProcess2");
  bvxm_batch.set_input_from_db(0, res);
  bvxm_batch.set_input_string(1, cor_file);
  bvxm_batch.set_input_string(2, output_folder);
  bvxm_batch.set_input_float(3, pixel_radius);  ## pixel radius to count for inliers
  statuscode = bvxm_batch.run_process();
  return statuscode;

## process to transfer geo_index leaf id to leaf string
## the geo index is loaded from tree_txt
def obtain_leaf_string(tree_txt, out_txt):
  bvxm_batch.init_process("volmTransferGeoIndexIDToStr")
  bvxm_batch.set_input_string(0, tree_txt)
  bvxm_batch.set_input_string(1, out_txt)
  bvxm_batch.run_process()

## process to transfer geo_index leaf id to leaf string
## the geo index is created from given region and min_size
def obtain_leaf_string2(min_size, in_poly, out_txt):
  bvxm_batch.init_process("volmTransferGeoIndexIDToStr")
  bvxm_batch.set_input_float(0, min_size)
  bvxm_batch.set_input_string(1, in_poly)
  bvxm_batch.set_input_string(2, out_txt)
  bvxm_batch.run_process()

def generate_height_map_from_ply(ply_folder, ni, nj):
  bvxm_batch.init_process("volmGenerateHeightMapFromPlyProcess")
  bvxm_batch.set_input_string(0, ply_folder);
  bvxm_batch.set_input_unsigned(1, ni);
  bvxm_batch.set_input_unsigned(2, nj);
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0);
  out = dbvalue(id, type);
  return out

## process to refine the height map obtained from bvxm world
## max_h and min_h are the predominant height for sky and ground mask
def refine_bvxm_height_map(img, max_h, min_h):
  bvxm_batch.init_process("volmRefineBvxmHeightMapProcess")
  bvxm_batch.set_input_from_db(0, img)
  bvxm_batch.set_input_float(1, max_h)
  bvxm_batch.set_input_float(2, min_h)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    out_img = dbvalue(id, type);
  else:
    out_img = 0;
  return out_img;

## process to project open street map roads onto a cropped satellite images using
## its local cropped RPC camera, an ortho height map and an ortho camera
def project_osm_to_crop_img(crop_img, crop_cam, ortho_img, ortho_cam, osm_bin_file, band = "r", is_road=True, is_region=False):
  bvxm_batch.init_process("volmMapOSMtoImage");
  bvxm_batch.set_input_from_db(0, crop_img);
  bvxm_batch.set_input_from_db(1, crop_cam);
  bvxm_batch.set_input_from_db(2, ortho_img);
  bvxm_batch.set_input_from_db(3, ortho_cam);
  bvxm_batch.set_input_string(4, osm_bin_file);
  bvxm_batch.set_input_string(5, band);
  bvxm_batch.set_input_bool(6,is_region)
  bvxm_batch.set_input_bool(7,is_road)
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    out_img = dbvalue(id, type);
  else:
    out_img = 0;
  return out_img;

## process to project DEM images to a satellite image given the satellite viewpoint
def project_dem_to_sat_img(sat_cam, sat_img, dem_file, dem_cam=0):
  bvxm_batch.init_process("volmProjectDEMtoSatImgPorcess");
  bvxm_batch.set_input_from_db(0, sat_cam);
  bvxm_batch.set_input_from_db(1, sat_img);
  bvxm_batch.set_input_string(2, dem_file);
  bvxm_batch.set_input_from_db(3, dem_cam);
  status = bvxm_batch.run_process();
  return status;

## process to up-sample the projected cropped ASTER DEM image
def upsample_projected_img(input_img, num_neighbors=4):
  bvxm_batch.init_process("volmUpsampleDemImgProcess");
  bvxm_batch.set_input_from_db(0,  input_img);
  bvxm_batch.set_input_unsigned(1, num_neighbors);
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    output_img = dbvalue(id, type);
  else:
    output_img = 0;
  return output_img;

def generate_height_map_plot(gt_height, height, dif_init, dif_final, dif_increment):
  bvxm_batch.init_process("volmGenerateHeightMapPlotProcess");
  bvxm_batch.set_input_from_db(0, gt_height);
  bvxm_batch.set_input_from_db(1, height);
  bvxm_batch.set_input_float(2, dif_init);
  bvxm_batch.set_input_float(3, dif_final);
  bvxm_batch.set_input_float(4, dif_increment);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  correct_rate_array = bvxm_batch.get_bbas_1d_array_float(id);
  (id, type) = bvxm_batch.commit_output(1);
  height_dif_array = bvxm_batch.get_bbas_1d_array_float(id);
  (id, type) = bvxm_batch.commit_output(2);
  out_map = dbvalue(id, type);
  return correct_rate_array, height_dif_array, out_map
