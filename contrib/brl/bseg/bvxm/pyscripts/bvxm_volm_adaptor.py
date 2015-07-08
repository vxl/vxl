from bvxm_register import bvxm_batch, dbvalue;
import math;

def create_satellite_resouces(roi_kml, leaf_size = 0.1, eliminate_same_images = False):
  bvxm_batch.init_process("volmCreateSatResourcesProcess")
  bvxm_batch.set_input_string(0, roi_kml)
  bvxm_batch.set_input_float(1, leaf_size)
  bvxm_batch.set_input_bool(2, eliminate_same_images)
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0)
  sat_res = dbvalue(id, type)
  return sat_res

def add_satellite_resources(sat_res, sat_res_folder):
  bvxm_batch.init_process("volmAddSatelliteResourcesProcess")
  bvxm_batch.set_input_from_db(0, sat_res)
  bvxm_batch.set_input_string(1, sat_res_folder)
  bvxm_batch.run_process()

def save_satellite_resources(sat_res, out_file):
  bvxm_batch.init_process("volmSaveSatResourcesProcess")
  bvxm_batch.set_input_from_db(0,sat_res)
  bvxm_batch.set_input_string(1,out_file)
  bvxm_batch.run_process()

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

def find_resource_pair(res, name, tol = 10.0):
  bvxm_batch.init_process("volmFindResourcePairProcess");
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_string(1,name);
  bvxm_batch.set_input_double(2,tol)
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

def find_stereo_pairs2(res, poly_roi, GSD_threshold, scene_res_file, satellite_name):
  bvxm_batch.init_process("volmFindSatellitePairsPolyProcess")
  bvxm_batch.set_input_from_db(0,res);
  bvxm_batch.set_input_string(1, poly_roi);
  bvxm_batch.set_input_string(2, scene_res_file);
  bvxm_batch.set_input_string(3, satellite_name);
  bvxm_batch.set_input_float(4, GSD_threshold);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  cnt = bvxm_batch.get_output_unsigned(id);
  return cnt;

def find_seed_sat_resources(res, poly_kml, downsample_factor, sat_res_file):
  bvxm_batch.init_process("volmFindOverlappingSatResourcesProcess")
  bvxm_batch.set_input_from_db(0,res);             # satellite resource
  bvxm_batch.set_input_string(1,poly_kml);         # kml polygon filename
  bvxm_batch.set_input_float(2,downsample_factor); # factor by which to downsample resource footprints when 
                                                   # computing the raster (smaller factor takes more time & memory)
  bvxm_batch.set_input_string(3,sat_res_file);     # output file to print the list (this will also save a kml version)
  bvxm_batch.run_process();

def find_intersecting_sat_resources(res, poly_kml, max_intersecting_resources, sat_res_file):
  bvxm_batch.init_process("volmFindIntersectingSatResourcesProcess")
  bvxm_batch.set_input_from_db(0,res);         # satellite resource
  bvxm_batch.set_input_string(1,poly_kml);     # kml polygon filename
  bvxm_batch.set_input_float(2,max_intersecting_resources); # maximum number of intersecting images to consider, e.g., 5;
                                                            # be careful with this number as this process computes
                                                            # a rising powerset, i.e., n choose k ... n choose l
  bvxm_batch.set_input_string(3,sat_res_file); # output file to print the list (this will also save a kml version)
  bvxm_batch.run_process();

def correct_ransac_process( cor_file, output_folder, pixel_radius):
  bvxm_batch.init_process("volmCorrectRationalCamerasRANSACProcess");
  bvxm_batch.set_input_string(0, cor_file);
  bvxm_batch.set_input_string(1, output_folder);
  bvxm_batch.set_input_float(2, pixel_radius);  ## pixel radius to count for inliers
  bvxm_batch.run_process();

## this one checks if the camera is already corrected and exists in the output folder
## weights the cameras accordingly
def correct_ransac_process2(res, cor_file, output_folder, pixel_radius, enforce_existing = 0):
  bvxm_batch.init_process("volmCorrectRationalCamerasRANSACProcess2");
  bvxm_batch.set_input_from_db(0, res);
  bvxm_batch.set_input_string(1, cor_file);
  bvxm_batch.set_input_string(2, output_folder);
  bvxm_batch.set_input_float(3, pixel_radius);  ## pixel radius to count for inliers
  bvxm_batch.set_input_int(4, enforce_existing);  ## if 1: enforce to have at least 2 existing images
  statuscode = bvxm_batch.run_process();
  return statuscode;

## this one perform camera correction with an 3-d initial guessing point defined from the overlapped region of all cameras
def correct_ransac_with_initial_process(res, cor_file, dem_folder, output_folder, pixel_radius, enforce_existing = False):
  bvxm_batch.init_process("volmCorrectRationalCameraRANSACwithIntialProcess")
  bvxm_batch.set_input_from_db(0, res)
  bvxm_batch.set_input_string(1, cor_file)
  bvxm_batch.set_input_string(2, dem_folder)
  bvxm_batch.set_input_string(3, output_folder)
  bvxm_batch.set_input_float(4, pixel_radius)
  bvxm_batch.set_input_bool(5, enforce_existing)
  status = bvxm_batch.run_process()
  return status

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
def project_osm_to_crop_img(crop_img, crop_cam, ortho_img, ortho_cam, osm_bin_file, band = "r", is_road=True, is_region=False, vsol_bin_filename="", kml_file = ""):
  bvxm_batch.init_process("volmMapOSMtoImage");
  bvxm_batch.set_input_from_db(0, crop_img);
  bvxm_batch.set_input_from_db(1, crop_cam);
  bvxm_batch.set_input_from_db(2, ortho_img);
  bvxm_batch.set_input_from_db(3, ortho_cam);
  bvxm_batch.set_input_string(4, osm_bin_file);
  bvxm_batch.set_input_string(5, band);
  bvxm_batch.set_input_bool(6,is_region)
  bvxm_batch.set_input_bool(7,is_road)
  bvxm_batch.set_input_string(8,vsol_bin_filename)
  bvxm_batch.set_input_string(9,kml_file)
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    out_img = dbvalue(id, type);
  else:
    out_img = 0;
  return out_img;

## process to project open street map roads onto an ortho image using its vpgl_geo_camera. no height map is necessary
def project_osm_to_ortho_img(img_byte, ortho_cam, osm_file):
  bvxm_batch.init_process("volmMapOSMProcess");
  bvxm_batch.set_input_from_db(0,img_byte);
  bvxm_batch.set_input_from_db(1,ortho_cam);
  bvxm_batch.set_input_string(2, osm_file);
  hit = bvxm_batch.run_process();                ## process returns true if any osm objects hit the image area in this tile
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return hit, out_img

def project_osm_category_to_ortho_img(img_byte, ortho_cam, osm_file, osm_category_name, output_vsol_binary_name):
  bvxm_batch.init_process("volmMapOSMontoImageProcess3");
  bvxm_batch.set_input_from_db(0,img_byte);
  bvxm_batch.set_input_from_db(1,ortho_cam);
  bvxm_batch.set_input_string(2, osm_file);
  bvxm_batch.set_input_string(3, osm_category_name);
  bvxm_batch.set_input_string(4, output_vsol_binary_name);
  hit = bvxm_batch.run_process();                ## process returns true if any osm objects hit the image area in this tile
  return hit

## process to project open street map roads onto a cropped satellite images using
## its local cropped RPC camera, an ortho height map and an ortho camera
## pass osm_category_name as in the volm category names in table: Dropbox\projects\FINDER\data\OSM\land_category_08_12_2014.txt
def project_osm_to_crop_img2(crop_img, crop_cam, ortho_img, ortho_cam, osm_bin_file, osm_category_name, vsol_bin_filename):
  bvxm_batch.init_process("volmMapOSMontoImageProcess2");
  bvxm_batch.set_input_from_db(0, crop_img);
  bvxm_batch.set_input_from_db(1, crop_cam);
  bvxm_batch.set_input_from_db(2, ortho_img);
  bvxm_batch.set_input_from_db(3, ortho_cam);
  bvxm_batch.set_input_string(4, osm_bin_file);
  bvxm_batch.set_input_string(5, osm_category_name);
  bvxm_batch.set_input_string(6,vsol_bin_filename)
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    out_img = dbvalue(id,type)
  else:
    out_img = 0
  return out_img;

## process to project DEM images to a satellite image given the satellite viewpoint
## modified to also input ortho camera (instead of a rational camera) so that DEMs or height maps can be projected onto any ortho image - just pass the ortho cam for sat_cam
def project_dem_to_sat_img(sat_cam, sat_img, dem_file, lower_left_lon=-1.0, lower_left_lat=-1.0, upper_right_lon=-1.0, upper_right_lat=-1.0, dem_cam=0):
  bvxm_batch.init_process("volmProjectDEMtoSatImgPorcess");
  bvxm_batch.set_input_from_db(0, sat_cam);
  bvxm_batch.set_input_from_db(1, sat_img);
  bvxm_batch.set_input_string(2, dem_file);
  bvxm_batch.set_input_from_db(3, dem_cam);
  bvxm_batch.set_input_double(4, lower_left_lon)
  bvxm_batch.set_input_double(5, lower_left_lat)
  bvxm_batch.set_input_double(6, upper_right_lon)
  bvxm_batch.set_input_double(7, upper_right_lat)
  status = bvxm_batch.run_process();
  return status;

## process to up-sample the projected cropped ASTER DEM image
def upsample_projected_img(input_img, num_neighbors=4, bin_size_col = 30, bin_size_row = 30):
  bvxm_batch.init_process("volmUpsampleDemImgProcess");
  bvxm_batch.set_input_from_db(0,  input_img);
  bvxm_batch.set_input_unsigned(1, num_neighbors);
  bvxm_batch.set_input_unsigned(2, bin_size_col);
  bvxm_batch.set_input_unsigned(3, bin_size_row);
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    output_img = dbvalue(id, type);
  else:
    output_img = 0;
  return output_img;

def generate_height_map_plot(gt_height, height, dif_init, dif_final, dif_increment, gt_fix):
  bvxm_batch.init_process("volmGenerateHeightMapPlotProcess");
  bvxm_batch.set_input_from_db(0, gt_height);
  bvxm_batch.set_input_from_db(1, height);
  bvxm_batch.set_input_float(2, dif_init);
  bvxm_batch.set_input_float(3, dif_final);
  bvxm_batch.set_input_float(4, dif_increment);
  bvxm_batch.set_input_float(5, gt_fix)
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  correct_rate_array = bvxm_batch.get_bbas_1d_array_float(id);
  (id, type) = bvxm_batch.commit_output(1);
  height_dif_array = bvxm_batch.get_bbas_1d_array_float(id);
  (id, type) = bvxm_batch.commit_output(2);
  out_map = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(3);
  out_map_dif = dbvalue(id, type);
  return correct_rate_array, height_dif_array, out_map, out_map_dif

# combine height map
def combine_height_map(height_map_folder, poly_roi, out_folder, size_in_degree = 0.0625, leaf_id = -1):
  bvxm_batch.init_process("volmCombineHeightMapProcess");
  bvxm_batch.set_input_string(0, height_map_folder);
  bvxm_batch.set_input_string(1, poly_roi);
  bvxm_batch.set_input_string(2, out_folder);
  bvxm_batch.set_input_float(3, size_in_degree);
  bvxm_batch.set_input_int(4, leaf_id);
  bvxm_batch.run_process();
  
# combine height maps by taking the median pixel values
def combine_height_map2(height_map_folder, threshold):
  bvxm_batch.init_process("volmCombineHeightMapProcess2");
  bvxm_batch.set_input_string(0, height_map_folder);
  bvxm_batch.set_input_float(1, threshold)
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    out_map = dbvalue(id, type);
    return out_map;
  else:
    return 0;

## process that takes an ortho height map, an ortho classification map (id image) and their ortho camera
##   extracts the outlines of the buildings in the classification map
##   converts them to geo positions and outputs a .csv file and a kml file
def extract_building_outlines(height_img, class_img, geocam, csv_file_name, kml_file_name):
  bvxm_batch.init_process("volmExtractBuildinOutlinesProcess")
  bvxm_batch.set_input_from_db(0, height_img)
  bvxm_batch.set_input_from_db(1, class_img)
  bvxm_batch.set_input_from_db(2, geocam)
  bvxm_batch.set_input_string(3, csv_file_name)
  bvxm_batch.set_input_string(4, kml_file_name)
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0);
  binary_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  binary_img_e = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(2);
  binary_img_d = dbvalue(id, type);
  return binary_img, binary_img_e, binary_img_d

def registration_error_analysis(gt_file, cor_file, ori_file, gsd = 1.0, cor_vector_file = "", ori_vector_file = ""):
  bvxm_batch.init_process("volmRegistrationErrorProcess")
  bvxm_batch.set_input_string(0, gt_file)
  bvxm_batch.set_input_string(1, cor_file)
  bvxm_batch.set_input_string(2, ori_file)
  bvxm_batch.set_input_double(3, gsd)
  bvxm_batch.set_input_string(4, cor_vector_file)
  bvxm_batch.set_input_string(5, ori_vector_file)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    cor_average = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    cor_std = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(2)
    ori_average = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(3)
    ori_std = bvxm_batch.get_output_double(id)
    return cor_average, cor_std, ori_average, ori_std
  else:
    return 0.0, 0.0, 0.0, 0.0

def stereo_h_map_fix(h_img, height_fix = 0.0):
  bvxm_batch.init_process("volmStereoHeightFixProcess")
  bvxm_batch.set_input_from_db(0,h_img)
  bvxm_batch.set_input_float(1, height_fix)
  bvxm_batch.run_process()
  
## process that find the minimum and maximum elevation from height map, for a give 2-d rectangluar region
def find_min_max_elev(ll_lon, ll_lat, ur_lon, ur_lat, dem_folder):
  bvxm_batch.init_process("volmFindMinMaxHeightPorcess")
  bvxm_batch.set_input_double(0, ll_lon)
  bvxm_batch.set_input_double(1, ll_lat)
  bvxm_batch.set_input_double(2, ur_lon)
  bvxm_batch.set_input_double(3, ur_lat)
  bvxm_batch.set_input_string(4, dem_folder)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0)
    min_elev = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    max_elev = bvxm_batch.get_output_double(id)
    return min_elev, max_elev
  else:
    return 0.0, 0.0
