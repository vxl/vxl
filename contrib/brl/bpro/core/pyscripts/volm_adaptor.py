# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()


def create_satellite_resouces(roi_kml, leaf_size=0.1,
                              eliminate_same_images=False):
    batch.init_process("volmCreateSatResourcesProcess")
    batch.set_input_string(0, roi_kml)
    batch.set_input_float(1, leaf_size)
    batch.set_input_bool(2, eliminate_same_images)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sat_res = dbvalue(id, type)
    return sat_res


def add_satellite_resources(sat_res, sat_res_folder):
    batch.init_process("volmAddSatelliteResourcesProcess")
    batch.set_input_from_db(0, sat_res)
    batch.set_input_string(1, sat_res_folder)
    batch.run_process()


def save_satellite_resources(sat_res, out_file):
    batch.init_process("volmSaveSatResourcesProcess")
    batch.set_input_from_db(0, sat_res)
    batch.set_input_string(1, out_file)
    batch.run_process()


def map_sdet_to_volm_ids(sdet_color_class_img):
    batch.init_process("volmGenerateClassMapProcess")
    batch.set_input_from_db(0, sdet_color_class_img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def volm_id_color_img(id_img, id_to_color_txt=""):
    batch.init_process("volmGenerateColorClassMapProcess")
    batch.set_input_from_db(0, id_img)
    batch.set_input_string(1, id_to_color_txt)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def update_class_map(class_img, source_img):
    batch.init_process("volmUpdateClassMapProcess")
    batch.set_input_from_db(0, class_img)
    batch.set_input_from_db(1, source_img)
    status = batch.run_process()
    return status


def load_sat_resources(res_file_name):
    batch.init_process("volmLoadSatResourcesProcess")
    batch.set_input_string(0, res_file_name)
    batch.run_process()
    (res2_id, res2_type) = batch.commit_output(0)
    res2 = dbvalue(res2_id, res2_type)
    return res2


def find_resource_pair(res, name, tol=10.0):
    batch.init_process("volmFindResourcePairProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, name)
    batch.set_input_double(2, tol)
    statuscode = batch.run_process()
    (f_id, f_type) = batch.commit_output(0)
    full_path = batch.get_output_string(f_id)
    batch.remove_data(f_id)
    (n_id, n_type) = batch.commit_output(1)
    pair_name = batch.get_output_string(n_id)
    batch.remove_data(n_id)
    (p_id, p_type) = batch.commit_output(2)
    full_path_pair_name = batch.get_output_string(p_id)
    batch.remove_data(p_id)
    return statuscode, full_path, pair_name, full_path_pair_name


def get_full_path(res, name):
    batch.init_process("volmGetFullPathProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, name)
    statuscode = batch.run_process()
    (f_id, f_type) = batch.commit_output(0)
    full_path = batch.get_output_string(f_id)
    batch.remove_data(f_id)
    return full_path

## band_name is PAN or MULTI


def pick_nadir_resource(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, satellite_name, band_name="PAN", non_cloud_folder=""):
    batch.init_process("volmPickNadirResProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_double(1, lower_left_lon)
    batch.set_input_double(2, lower_left_lat)
    batch.set_input_double(3, upper_right_lon)
    batch.set_input_double(4, upper_right_lat)
    batch.set_input_string(5, band_name)
    batch.set_input_string(6, satellite_name)
    batch.set_input_string(7, non_cloud_folder)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sat_path = batch.get_output_string(id)
    return sat_path

# find the PAN/MULTI pair for given rectangular region, also output the
# sorted list of such PAN/MULIT lists


def pick_nadir_resource_pair(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, satellite_name, out_folder, band_name="PAN", non_cloud_folder=""):
    batch.init_process("volmPickNadirResPairProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_double(1, lower_left_lon)
    batch.set_input_double(2, lower_left_lat)
    batch.set_input_double(3, upper_right_lon)
    batch.set_input_double(4, upper_right_lat)
    batch.set_input_string(5, band_name)
    batch.set_input_string(6, satellite_name)
    batch.set_input_string(7, non_cloud_folder)
    batch.set_input_string(8, out_folder)
    statuscode = batch.run_process()
    if statuscode:
        (p_id, p_type) = batch.commit_output(0)
        pan_path = batch.get_output_string(p_id)
        batch.remove_data(p_id)
        (m_id, m_type) = batch.commit_output(1)
        multi_path = batch.get_output_string(m_id)
        batch.remove_data(m_id)
    else:
        pan_path = ""
        multi_path = ""
    return statuscode, pan_path, multi_path

# GSD: ground sampling distance, e.g. pass 1 to eliminate all the images
# which have pixel GSD more than 1 meter; the default is 10 meters, so
# practically returns all the satellite images


def scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, scene_res_file, band="PAN", GSD_threshold=10.0, pick_seeds=0, n_seeds=0):
    batch.init_process("volmQuerySatelliteResourcesProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_double(1, lower_left_lon)
    batch.set_input_double(2, lower_left_lat)
    batch.set_input_double(3, upper_right_lon)
    batch.set_input_double(4, upper_right_lat)
    batch.set_input_string(5, scene_res_file)
    batch.set_input_string(6, band)
    # of 0, it returns all resources that intersect the box, otherwise, it
    # picks n_seeds among these resources
    batch.set_input_bool(7, pick_seeds)
    batch.set_input_int(8, n_seeds)
    batch.set_input_double(9, GSD_threshold)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cnt = batch.get_output_unsigned(id)
    return cnt


def scene_resources2(res, poly_kml, scene_res_file, band="PAN", GSD_threshold=10.0, pick_seeds=0, n_seeds=0):
    batch.init_process("volmQuerySatelliteResourceKmlProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, poly_kml)
    batch.set_input_string(2, scene_res_file)
    batch.set_input_string(3, band)
    batch.set_input_bool(4, pick_seeds)
    batch.set_input_int(5, n_seeds)
    batch.set_input_double(6, GSD_threshold)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cnt = batch.get_output_unsigned(id)
    return cnt


def find_stereo_pairs(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, GSD_threshold, scene_res_file, satellite_name):
    batch.init_process("volmFindSatellitePairsProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_double(1, lower_left_lon)
    batch.set_input_double(2, lower_left_lat)
    batch.set_input_double(3, upper_right_lon)
    batch.set_input_double(4, upper_right_lat)
    batch.set_input_string(5, scene_res_file)
    batch.set_input_string(6, satellite_name)
    batch.set_input_float(7, GSD_threshold)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cnt = batch.get_output_unsigned(id)
    return cnt


def find_stereo_pairs2(res, poly_roi, GSD_threshold, scene_res_file, satellite_name):
    batch.init_process("volmFindSatellitePairsPolyProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, poly_roi)
    batch.set_input_string(2, scene_res_file)
    batch.set_input_string(3, satellite_name)
    batch.set_input_float(4, GSD_threshold)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cnt = batch.get_output_unsigned(id)
    return cnt


def find_seed_sat_resources(res, poly_kml, downsample_factor, sat_res_file):
    batch.init_process("volmFindOverlappingSatResourcesProcess")
    batch.set_input_from_db(0, res)             # satellite resource
    batch.set_input_string(1, poly_kml)         # kml polygon filename
    # factor by which to downsample resource footprints when
    batch.set_input_float(2, downsample_factor)
    # computing the raster (smaller factor takes more time & memory)
    # output file to print the list (this will also save a kml version)
    batch.set_input_string(3, sat_res_file)
    batch.run_process()


def find_intersecting_sat_resources(res, poly_kml, max_intersecting_resources, sat_res_file):
    batch.init_process("volmFindIntersectingSatResourcesProcess")
    batch.set_input_from_db(0, res)         # satellite resource
    batch.set_input_string(1, poly_kml)     # kml polygon filename
    # maximum number of intersecting images to consider, e.g., 5
    batch.set_input_float(2, max_intersecting_resources)
    # be careful with this number as this process computes
    # a rising powerset, i.e., n choose k ... n choose l
    # output file to print the list (this will also save a kml version)
    batch.set_input_string(3, sat_res_file)
    batch.run_process()


def correct_ransac_process(cor_file, output_folder, pixel_radius):
    batch.init_process("volmCorrectRationalCamerasRANSACProcess")
    batch.set_input_string(0, cor_file)
    batch.set_input_string(1, output_folder)
    # pixel radius to count for inliers
    batch.set_input_float(2, pixel_radius)
    batch.run_process()

# this one checks if the camera is already corrected and exists in the output folder
# weights the cameras accordingly


def correct_ransac_process2(res, cor_file, output_folder, pixel_radius, enforce_existing=0):
    batch.init_process("volmCorrectRationalCamerasRANSACProcess2")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, cor_file)
    batch.set_input_string(2, output_folder)
    # pixel radius to count for inliers
    batch.set_input_float(3, pixel_radius)
    # if 1: enforce to have at least 2 existing images
    batch.set_input_int(4, enforce_existing)
    statuscode = batch.run_process()
    return statuscode

# this one perform camera correction with an 3-d initial guessing point
# defined from the overlapped region of all cameras


def correct_ransac_with_initial_process(res, cor_file, dem_folder, output_folder, pixel_radius, enforce_existing=False):
    batch.init_process("volmCorrectRationalCameraRANSACwithIntialProcess")
    batch.set_input_from_db(0, res)
    batch.set_input_string(1, cor_file)
    batch.set_input_string(2, dem_folder)
    batch.set_input_string(3, output_folder)
    batch.set_input_float(4, pixel_radius)
    batch.set_input_bool(5, enforce_existing)
    status = batch.run_process()
    return status

# process to transfer geo_index leaf id to leaf string
# the geo index is loaded from tree_txt


def obtain_leaf_string(tree_txt, out_txt):
    batch.init_process("volmTransferGeoIndexIDToStr")
    batch.set_input_string(0, tree_txt)
    batch.set_input_string(1, out_txt)
    batch.run_process()

# process to transfer geo_index leaf id to leaf string
# the geo index is created from given region and min_size


def obtain_leaf_string2(min_size, in_poly, out_txt):
    batch.init_process("volmTransferGeoIndexIDToStr")
    batch.set_input_float(0, min_size)
    batch.set_input_string(1, in_poly)
    batch.set_input_string(2, out_txt)
    batch.run_process()


def generate_height_map_from_ply(ply_folder, ni, nj):
    batch.init_process("volmGenerateHeightMapFromPlyProcess")
    batch.set_input_string(0, ply_folder)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out = dbvalue(id, type)
    return out

# process to refine the height map obtained from bvxm world
# max_h and min_h are the predominant height for sky and ground mask


def refine_bvxm_height_map(img, max_h, min_h):
    batch.init_process("volmRefineBvxmHeightMapProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_float(1, max_h)
    batch.set_input_float(2, min_h)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
    else:
        out_img = 0
    return out_img

# process to project open street map roads onto a cropped satellite images using
# its local cropped RPC camera, an ortho height map and an ortho camera


def project_osm_to_crop_img(crop_img, crop_cam, ortho_img, ortho_cam, osm_bin_file, band="r", is_road=True, is_region=False, vsol_bin_filename="", kml_file=""):
    batch.init_process("volmMapOSMtoImage")
    batch.set_input_from_db(0, crop_img)
    batch.set_input_from_db(1, crop_cam)
    batch.set_input_from_db(2, ortho_img)
    batch.set_input_from_db(3, ortho_cam)
    batch.set_input_string(4, osm_bin_file)
    batch.set_input_string(5, band)
    batch.set_input_bool(6, is_region)
    batch.set_input_bool(7, is_road)
    batch.set_input_string(8, vsol_bin_filename)
    batch.set_input_string(9, kml_file)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
    else:
        out_img = 0
    return out_img

# process to project open street map roads onto an ortho image using its
# vpgl_geo_camera. no height map is necessary


def project_osm_to_ortho_img(img_byte, ortho_cam, osm_file, output_vsol_binary_name=""):
    batch.init_process("volmMapOSMProcess")
    batch.set_input_from_db(0, img_byte)
    batch.set_input_from_db(1, ortho_cam)
    batch.set_input_string(2, osm_file)
    batch.set_input_string(3, output_vsol_binary_name)
    # process returns true if any osm objects hit the image area in this tile
    hit = batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    return hit, out_img


def project_osm_category_to_ortho_img(img_byte, ortho_cam, osm_file, osm_category_name, output_vsol_binary_name):
    batch.init_process("volmMapOSMontoImageProcess3")
    batch.set_input_from_db(0, img_byte)
    batch.set_input_from_db(1, ortho_cam)
    batch.set_input_string(2, osm_file)
    batch.set_input_string(3, osm_category_name)
    batch.set_input_string(4, output_vsol_binary_name)
    # process returns true if any osm objects hit the image area in this tile
    hit = batch.run_process()
    return hit

# process to project open street map roads onto a cropped satellite images using
# its local cropped RPC camera, an ortho height map and an ortho camera
# pass osm_category_name as in the volm category names in table:
# Dropbox\projects\FINDER\data\OSM\land_category_08_12_2014.txt


def project_osm_to_crop_img2(crop_img, crop_cam, ortho_img, ortho_cam, osm_bin_file, osm_category_name, vsol_bin_filename):
    batch.init_process("volmMapOSMontoImageProcess2")
    batch.set_input_from_db(0, crop_img)
    batch.set_input_from_db(1, crop_cam)
    batch.set_input_from_db(2, ortho_img)
    batch.set_input_from_db(3, ortho_cam)
    batch.set_input_string(4, osm_bin_file)
    batch.set_input_string(5, osm_category_name)
    batch.set_input_string(6, vsol_bin_filename)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_mask = dbvalue(id, type)
        return out_img, out_mask
    else:
        return None, None

# process to project DEM images to a satellite image given the satellite viewpoint
# modified to also input ortho camera (instead of a rational camera) so
# that DEMs or height maps can be projected onto any ortho image - just
# pass the ortho cam for sat_cam


def project_dem_to_sat_img(sat_cam, sat_img, dem_file, lower_left_lon=-1.0, lower_left_lat=-1.0, upper_right_lon=-1.0, upper_right_lat=-1.0, dem_cam=0):
    batch.init_process("volmProjectDEMtoSatImgPorcess")
    batch.set_input_from_db(0, sat_cam)
    batch.set_input_from_db(1, sat_img)
    batch.set_input_string(2, dem_file)
    batch.set_input_from_db(3, dem_cam)
    batch.set_input_double(4, lower_left_lon)
    batch.set_input_double(5, lower_left_lat)
    batch.set_input_double(6, upper_right_lon)
    batch.set_input_double(7, upper_right_lat)
    status = batch.run_process()
    return status

# process to up-sample the projected cropped ASTER DEM image


def upsample_projected_img(input_img, num_neighbors=4, bin_size_col=30, bin_size_row=30):
    batch.init_process("volmUpsampleDemImgProcess")
    batch.set_input_from_db(0,  input_img)
    batch.set_input_unsigned(1, num_neighbors)
    batch.set_input_unsigned(2, bin_size_col)
    batch.set_input_unsigned(3, bin_size_row)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        output_img = dbvalue(id, type)
    else:
        output_img = 0
    return output_img


def generate_height_map_plot(gt_height, height, dif_init, dif_final, dif_increment, gt_fix):
    batch.init_process("volmGenerateHeightMapPlotProcess")
    batch.set_input_from_db(0, gt_height)
    batch.set_input_from_db(1, height)
    batch.set_input_float(2, dif_init)
    batch.set_input_float(3, dif_final)
    batch.set_input_float(4, dif_increment)
    batch.set_input_float(5, gt_fix)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    correct_rate_array = batch.get_bbas_1d_array_float(id)
    (id, type) = batch.commit_output(1)
    height_dif_array = batch.get_bbas_1d_array_float(id)
    (id, type) = batch.commit_output(2)
    out_map = dbvalue(id, type)
    (id, type) = batch.commit_output(3)
    out_map_dif = dbvalue(id, type)
    return correct_rate_array, height_dif_array, out_map, out_map_dif

# combine height map


def combine_height_map(height_map_folder, poly_roi, out_folder, size_in_degree=0.0625, leaf_id=-1):
    batch.init_process("volmCombineHeightMapProcess")
    batch.set_input_string(0, height_map_folder)
    batch.set_input_string(1, poly_roi)
    batch.set_input_string(2, out_folder)
    batch.set_input_float(3, size_in_degree)
    batch.set_input_int(4, leaf_id)
    batch.run_process()

# combine height maps by taking the median pixel values


def combine_height_map2(height_map_folder, threshold):
    batch.init_process("volmCombineHeightMapProcess2")
    batch.set_input_string(0, height_map_folder)
    batch.set_input_float(1, threshold)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_map = dbvalue(id, type)
        return out_map
    else:
        return 0

# combine dem generated from multiple stereo pairs

def combine_dem_pairs(height_map_folder, threshold, out_dir = "", select_pair = False, is_debug = False):
    batch.init_process("volmCombineDEMPairsProcess")
    batch.set_input_string(0, height_map_folder)
    batch.set_input_float(1, threshold)
    batch.set_input_bool(2, select_pair)
    batch.set_input_bool(3, is_debug)
    batch.set_input_string(4, out_dir)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_map = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_conf = dbvalue(id, type)
        return out_map, out_conf
    else:
        return None, None

# process that takes an ortho height map, an ortho classification map (id image) and their ortho camera
# extracts the outlines of the buildings in the classification map
# converts them to geo positions and outputs a .csv file and a kml file


def extract_building_outlines(height_img, class_img, geocam, csv_file_name, kml_file_name):
    batch.init_process("volmExtractBuildinOutlinesProcess")
    batch.set_input_from_db(0, height_img)
    batch.set_input_from_db(1, class_img)
    batch.set_input_from_db(2, geocam)
    batch.set_input_string(3, csv_file_name)
    batch.set_input_string(4, kml_file_name)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    binary_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    binary_img_e = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    binary_img_d = dbvalue(id, type)
    return binary_img, binary_img_e, binary_img_d


def registration_error_analysis(gt_file, cor_file, ori_file, gsd=1.0, cor_vector_file="", ori_vector_file=""):
    batch.init_process("volmRegistrationErrorProcess")
    batch.set_input_string(0, gt_file)
    batch.set_input_string(1, cor_file)
    batch.set_input_string(2, ori_file)
    batch.set_input_double(3, gsd)
    batch.set_input_string(4, cor_vector_file)
    batch.set_input_string(5, ori_vector_file)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cor_average = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        cor_std = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        ori_average = batch.get_output_double(id)
        (id, type) = batch.commit_output(3)
        ori_std = batch.get_output_double(id)
        return cor_average, cor_std, ori_average, ori_std
    else:
        return 0.0, 0.0, 0.0, 0.0


def stereo_h_map_fix(h_img, height_fix=0.0):
    batch.init_process("volmStereoHeightFixProcess")
    batch.set_input_from_db(0, h_img)
    batch.set_input_float(1, height_fix)
    batch.run_process()

# process that find the minimum and maximum elevation from height map, for
# a give 2-d rectangluar region


def find_min_max_elev(ll_lon, ll_lat, ur_lon, ur_lat, dem_folder):
    batch.init_process("volmFindMinMaxHeightPorcess")
    batch.set_input_double(0, ll_lon)
    batch.set_input_double(1, ll_lat)
    batch.set_input_double(2, ur_lon)
    batch.set_input_double(3, ur_lat)
    batch.set_input_string(4, dem_folder)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        min_elev = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        max_elev = batch.get_output_double(id)
        return min_elev, max_elev
    else:
        return 0.0, 0.0

# process that generate normalized height map from multiple height map tiles created by bvxm 3-d reconstruction
# the land cover image is used to define the ground pixel and coverage region


def generate_ndsm(ll_lon, ll_lat, ur_lon, ur_lat, img_size_ni, img_size_nj, geo_index_txt, h_map_folder, grd_map_folder, window_size=20, max_h_limit=254.0):
    batch.init_process("volmNdsmGenearationProcess")
    batch.set_input_double(0, ll_lon)
    batch.set_input_double(1, ll_lat)
    batch.set_input_double(2, ur_lon)
    batch.set_input_double(3, ur_lat)
    batch.set_input_unsigned(4, img_size_ni)
    batch.set_input_unsigned(5, img_size_nj)
    batch.set_input_string(6, geo_index_txt)
    batch.set_input_string(7, h_map_folder)
    batch.set_input_string(8, grd_map_folder)
    batch.set_input_unsigned(9, window_size)
    batch.set_input_float(10, max_h_limit)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_ndsm = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_dsm = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        grd_img = dbvalue(id, type)
        (id, type) = batch.commit_output(3)
        out_cam = dbvalue(id, type)
        return out_ndsm, out_dsm, grd_img, out_cam
    else:
        return None, None, None, None

# process to estimate ground plane from a height map


def dsm_ground_estimation(dsm_image, invalid_pixel=-1.0, window_size=20, sample_size=10):
    batch.init_process("volmDsmGroundEstimationProcess")
    batch.set_input_from_db(0, dsm_image)
    batch.set_input_int(1, sample_size)
    batch.set_input_int(2, window_size)
    batch.set_input_float(3, invalid_pixel)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        grd_img = dbvalue(id, type)
        return grd_img
    else:
        return None


def dsm_ground_estimation_edge(dsm_image, edge_img, invalid_pixel=-1.0, sample_size=10):
    batch.init_process("volmDsmGroundEstimationEdgeProcess")
    batch.set_input_from_db(0, dsm_image)
    batch.set_input_from_db(1, edge_img)
    batch.set_input_int(2, sample_size)
    batch.set_input_float(3, invalid_pixel)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        grd_img = dbvalue(id, type)
        return grd_img
    else:
        return None


def dsm_mgf_ground_filtering(dsm_img, elev_thres, slope_thres, window_size=3.0, pixel_res=1.0):
    batch.init_process("volmDsmGroundFilterMGFProcess")
    batch.set_input_from_db(0, dsm_img)
    batch.set_input_float(1, window_size)
    batch.set_input_float(2, elev_thres)
    batch.set_input_float(3, slope_thres)
    batch.set_input_float(4, pixel_res)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        grd_mask = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        grd_img = dbvalue(id, type)
        return grd_mask, grd_img
    else:
        return None, None

# process to mosaics a set of images that covers the given region


def combine_geotiff_images(ll_lon, ll_lat, ur_lon, ur_lat, in_img_folder, init_value=-1.0):
    batch.init_process("volmCombineHeightMapProcess3")
    batch.set_input_string(0, in_img_folder)
    batch.set_input_double(1, ll_lon)
    batch.set_input_double(2, ll_lat)
    batch.set_input_double(3, ur_lon)
    batch.set_input_double(4, ur_lat)
    batch.set_input_float(5, init_value)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_cam = dbvalue(id, type)
        return out_img, out_cam
    else:
        return None, None

# process to generate building layers from land cover image and height image


def generate_building_layers(land_img, land_cam, height_img, height_cam, land_txt, min_h, max_h):
    batch.init_process("volmBuildingLayerExtractionProcess")
    batch.set_input_from_db(0, land_img)
    batch.set_input_from_db(1, land_cam)
    batch.set_input_from_db(2, height_img)
    batch.set_input_from_db(3, height_cam)
    batch.set_input_string(4, land_txt)
    batch.set_input_float(5, min_h)
    batch.set_input_float(6, max_h)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        mask_img = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        out_cam = dbvalue(id, type)
        return out_img, mask_img, out_cam
    else:
        return None, None, None


def generate_layers(land_img, land_cam, height_img, height_cam, land_txt, min_h, max_h, beta=10.0):
    batch.init_process("volmLayerExtractionProcess")
    batch.set_input_from_db(0, land_img)
    batch.set_input_from_db(1, land_cam)
    batch.set_input_from_db(2, height_img)
    batch.set_input_from_db(3, height_cam)
    batch.set_input_string(4, land_txt)
    batch.set_input_float(5, min_h)
    batch.set_input_float(6, max_h)
    batch.set_input_double(7, beta)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_prob_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(2)
        mask_img = dbvalue(id, type)
        (id, type) = batch.commit_output(3)
        out_cam = dbvalue(id, type)
        return out_prob_img, out_img, mask_img, out_cam
    else:
        return None, None, None, None

# process to convert a polygons in KML to geotiff byte image
# Note that this process will update the input image according to given
# polygon structures


def render_kml_polygon_mask(in_kml, image, ll_lon, ll_lat, ur_lon, ur_lat, mask_value=255):
    batch.init_process("volmRenderKmlPolygonMaskProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_double(1, ll_lon)
    batch.set_input_double(2, ll_lat)
    batch.set_input_double(3, ur_lon)
    batch.set_input_double(4, ur_lat)
    batch.set_input_string(5, in_kml)
    batch.set_input_unsigned(6, mask_value)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_cam = dbvalue(id, type)
        return out_cam
    else:
        return None

# process to generate a kml file from a binary image


def generate_kml_from_image(in_img, in_cam, out_kml, threshold=127, r=0, g=255, b=0):
    batch.init_process("volmGenerateKmlFromBinaryImageProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_from_db(1, in_cam)
    batch.set_input_unsigned(2, threshold)
    batch.set_input_string(3, out_kml)
    batch.set_input_unsigned(4, r)
    batch.set_input_unsigned(5, g)
    batch.set_input_unsigned(6, b)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        n_connected_component = batch.get_output_unsigned(id)
        return n_connected_component
    else:
        return None

# process to downsample a binary layer image


def downsample_binary_layer(in_img, in_mask, in_cam, out_img, out_mask, out_cam):
    batch.init_process("volmDownsampleLayerImageProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_from_db(1, in_mask)
    batch.set_input_from_db(2, in_cam)
    batch.set_input_from_db(3, out_img)
    batch.set_input_from_db(4, out_mask)
    batch.set_input_from_db(5, out_cam)
    status = batch.run_process()
    return status

# process to compute detection rate based ROC


def region_wise_roc_analysis(in_img, in_cam, positive_kml, negative_kml):
    batch.init_process("volmDetectionRateROCProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_from_db(1, in_cam)
    batch.set_input_string(2, positive_kml)
    batch.set_input_string(3, negative_kml)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        thres_out = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(1)
        tp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(2)
        tn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(3)
        fp = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(4)
        fn = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(5)
        tpr = batch.get_bbas_1d_array_float(id)
        (id, type) = batch.commit_output(6)
        fpr = batch.get_bbas_1d_array_float(id)
        return thres_out, tp, tn, fp, fn, tpr, fpr
    else:
        return None, None, None, None, None, None, None
