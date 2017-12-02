from bvxm_register import bvxm_batch, dbvalue
import os
#############################################################################
# PROVIDES higher level python functions to make bvxm_batch
# code more readable/refactored
#############################################################################

# loads/creates scene


def create_scene(scene_params_xml):
    bvxm_batch.init_process("bvxmCreateVoxelWorldProcess")
    # "./bvxmCreateVoxelWorldProcess.xml"
    bvxm_batch.set_params_process(scene_params_xml)
    status = bvxm_batch.run_process()
    if status != 1:
        raise Exception('Error creating scene from ' + scene_params_xml)
    (world_id, world_type) = bvxm_batch.commit_output(0)
    world = dbvalue(world_id, world_type)
    return world

# create scene xml


def create_scene_xml(scene_xml, world_dir, lvcs, lvcs_file, dim_x, dim_y, dim_z, voxel_size=1.0, corner_x=0.0, corner_y=0.0, corner_z=0.0,
                     min_ocp_prob=0.001, max_ocp_prob=0.999, max_scale=1):
    bvxm_batch.init_process("bvxmCreateSceneXmlProcess")
    bvxm_batch.set_input_string(0, scene_xml)
    bvxm_batch.set_input_string(1, world_dir)
    bvxm_batch.set_input_float(2,  corner_x)
    bvxm_batch.set_input_float(3,  corner_y)
    bvxm_batch.set_input_float(4,  corner_z)
    bvxm_batch.set_input_unsigned(5, dim_x)
    bvxm_batch.set_input_unsigned(6, dim_y)
    bvxm_batch.set_input_unsigned(7, dim_z)
    bvxm_batch.set_input_float(8, voxel_size)
    bvxm_batch.set_input_from_db(9, lvcs)
    bvxm_batch.set_input_string(10, lvcs_file)
    bvxm_batch.set_input_float(11, min_ocp_prob)
    bvxm_batch.set_input_float(12, max_ocp_prob)
    bvxm_batch.set_input_unsigned(13, max_scale)
    return bvxm_batch.run_process()


def create_scene_large_scale(roi_kml, scene_root, world_dir, dem_folder, world_size=500.0, voxel_size=1.0, height_diff=120.0, height_sub=25.0, land_folder=""):
    bvxm_batch.init_process("bvxmCreateSceneXmlLargeScaleProcess")
    bvxm_batch.set_input_string(0, roi_kml)
    bvxm_batch.set_input_string(1, scene_root)
    bvxm_batch.set_input_string(2, world_dir)
    bvxm_batch.set_input_string(3, dem_folder)
    bvxm_batch.set_input_string(4, land_folder)
    bvxm_batch.set_input_float(5, world_size)
    bvxm_batch.set_input_float(6, voxel_size)
    bvxm_batch.set_input_float(7, height_diff)
    bvxm_batch.set_input_float(8, height_sub)
    status = bvxm_batch.run_process()
    if status:
        (id, type) = bvxm_batch.commit_output(0)
        n_scenes = bvxm_batch.get_output_unsigned(id)
        return n_scenes
    else:
        return 0


def scene_box(scene):
    bvxm_batch.init_process("bvxmSceneBoxProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    lower_left_lon = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    lower_left_lat = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(2)
    lower_left_elev = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(3)
    upper_right_lon = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(4)
    upper_right_lat = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(5)
    upper_right_elev = bvxm_batch.get_output_double(id)
    return lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev

# changed to always draw a filled polygon, default alpha value (a) is set to 0 so that the box is completely transparent though so it will look as an empty polygon
# set a to e.g. 100 for a semi-transparent polygon, set a to 255 for an
# opaque polygon


def write_scene_kml(scene, kml_filename, is_overwrite=True, r=255, g=255, b=255, a=0, name=""):
    bvxm_batch.init_process("bvxmSceneKmlProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.set_input_string(1, kml_filename)
    bvxm_batch.set_input_bool(2, is_overwrite)
    bvxm_batch.set_input_unsigned(3, r)
    bvxm_batch.set_input_unsigned(4, g)
    bvxm_batch.set_input_unsigned(5, b)
    bvxm_batch.set_input_unsigned(6, a)
    bvxm_batch.set_input_string(7, name)
    bvxm_batch.run_process()

# check whether the scene is intersects with a given kml polygon


def check_scene_poly_overlap(scene, kml_file):
    bvxm_batch.init_process("bvxmScenePolyOverlapProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.set_input_string(1, kml_file)
    status = bvxm_batch.run_process()
    return status


def scene_origin(scene):
    bvxm_batch.init_process("bvxmSceneOriginProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    lower_left_lon = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    lower_left_lat = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(2)
    lower_left_z = bvxm_batch.get_output_double(id)
    return lower_left_lon, lower_left_lat, lower_left_z

# return the scene bbox and voxel size in local coordinates


def scene_local_box(scene):
    bvxm_batch.init_process("bvxmSceneLocalBoxProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    lower_left_x = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(1)
    lower_left_y = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(2)
    upper_right_x = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(3)
    upper_right_y = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(4)
    voxel_size = bvxm_batch.get_output_double(id)
    (id, type) = bvxm_batch.commit_output(5)
    lower_left_z = bvxm_batch.get_output_double(id)  # min z
    (id, type) = bvxm_batch.commit_output(6)
    upper_right_z = bvxm_batch.get_output_double(id)  # max z
    return lower_left_x, lower_left_y, upper_right_x, upper_right_y, voxel_size, lower_left_z, upper_right_z

# return the scene world directory


def model_dir(scene):
    bvxm_batch.init_process("bvxmSceneModelDirProcess")
    bvxm_batch.set_input_from_db(0, scene)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    model_dir = bvxm_batch.get_output_string(id)
    return model_dir


def roi_init(image_filename, cam, world, roi_init_params_xml, is_all_bits=False):
    bvxm_batch.init_process("bvxmRoiInitProcess")
    bvxm_batch.set_input_string(0, image_filename)
    bvxm_batch.set_input_from_db(1, cam)
    bvxm_batch.set_input_from_db(2, world)
    bvxm_batch.set_input_bool(3, is_all_bits)
    # "bvxmRoiInitProcess.xml")
    bvxm_batch.set_params_process(roi_init_params_xml)
    statuscode = bvxm_batch.run_process()
    if statuscode:
        (cropped_cam_id, cropped_cam_type) = bvxm_batch.commit_output(0)
        cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type)
        (cropped_image_id, cropped_image_type) = bvxm_batch.commit_output(1)
        cropped_image = dbvalue(cropped_image_id, cropped_image_type)
        (uncertainty_id, uncertainty_type) = bvxm_batch.commit_output(2)
        uncertainty = dbvalue(uncertainty_id, uncertainty_type)
        return statuscode, cropped_cam, cropped_image, uncertainty
    else:
        return statuscode, dbvalue(0, ""), dbvalue(0, ""), dbvalue(0, "")


def bvxm_detect_edges(cropped_image, edge_params_xml):
    bvxm_batch.init_process("bvxmDetectEdgesProcess")
    bvxm_batch.set_input_from_db(0, cropped_image)
    bvxm_batch.set_params_process(edge_params_xml)
    bvxm_batch.run_process()
    (cropped_edge_image_id, cropped_edge_image) = bvxm_batch.commit_output(0)
    cropped_edge_image = dbvalue(cropped_edge_image_id, cropped_edge_image)
    return cropped_edge_image


def update_edges(world, cropped_cam, cropped_edge_image, edge_prob_mask_size=21, edge_prob_mask_sigma=1.0, scale=0):
    bvxm_batch.init_process("bvxmUpdateEdgesProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_from_db(1, cropped_cam)
    bvxm_batch.set_input_from_db(2, cropped_edge_image)
    bvxm_batch.set_input_unsigned(3, 0)
    bvxm_batch.set_input_int(4, edge_prob_mask_size)
    bvxm_batch.set_input_float(5, edge_prob_mask_sigma)
# bvxm_batch.set_params_process(update_params_xml); #
# "./bvxmUpdateEdgesProcess.xml");
    bvxm_batch.run_process()


def rpc_registration(world, cropped_cam, cropped_edge_image, uncertainty, shift_3d_flag=0, scale=0, is_uncertainty_float=0):
    bvxm_batch.init_process("bvxmRpcRegistrationProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_from_db(1, cropped_cam)
    bvxm_batch.set_input_from_db(2, cropped_edge_image)
    bvxm_batch.set_input_bool(3, shift_3d_flag)
    if is_uncertainty_float == 1:
        print "uncertainty = ", uncertainty
        bvxm_batch.set_input_float(4, uncertainty)
    else:
        bvxm_batch.set_input_from_db(4, uncertainty)
    bvxm_batch.set_input_unsigned(5, scale)
    bvxm_batch.run_process()
    (cam_id, cam_type) = bvxm_batch.commit_output(0)
    cam = dbvalue(cam_id, cam_type)
    (expected_edge_image_id, expected_edge_image_type) = bvxm_batch.commit_output(1)
    expected_edge_image = dbvalue(
        expected_edge_image_id, expected_edge_image_type)
    (offset_u_id, offset_u_type) = bvxm_batch.commit_output(2)
    offset_u = bvxm_batch.get_output_double(offset_u_id)
    (offset_v_id, offset_v_type) = bvxm_batch.commit_output(3)
    offset_v = bvxm_batch.get_output_double(offset_v_id)
    return cam, expected_edge_image, offset_u, offset_v


def render_height_map(world):
    print("Rendering height map")
    bvxm_batch.init_process("bvxmHeightmapOrthoProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_d_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    out_h_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(2)
    out_conf_img = dbvalue(id, type)
    return out_h_img, out_d_img, out_conf_img


def render_height_map_with_cam(world, input_cam, ni, nj, is_negate=False):
    bvxm_batch.init_process("bvxmHeightmapProcess")
    bvxm_batch.set_input_from_db(0, input_cam)
    bvxm_batch.set_input_unsigned(1, ni)
    bvxm_batch.set_input_unsigned(2, nj)
    bvxm_batch.set_input_from_db(3, world)
    bvxm_batch.set_input_bool(4, is_negate)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_d_img = dbvalue(id, type)
    return out_d_img


def render_height_map_expected_with_cam(world, input_cam, ni, nj):
    bvxm_batch.init_process("bvxmHeightmapExpectedProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_from_db(1, input_cam)
    bvxm_batch.set_input_unsigned(2, ni)
    bvxm_batch.set_input_unsigned(3, nj)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_h_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    out_var_img = dbvalue(id, type)
    return out_h_img, out_var_img


def render_uncertainty_img(input_cam, ni, nj, world):
    bvxm_batch.init_process("bvxmUncertaintyProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_from_db(1, input_cam)
    bvxm_batch.set_input_unsigned(2, ni)
    bvxm_batch.set_input_unsigned(3, nj)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def render_ortho_edgemap(world, scale=0):
    print("Rendering ortho edge map")
    bvxm_batch.init_process("bvxmEdgemapOrthoProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_unsigned(1, 0)  # scale
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_e_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    out_e_img_byte = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(2)
    out_h_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(3)
    ortho_cam = dbvalue(id, type)
    return out_e_img, out_e_img_byte, out_h_img, ortho_cam


def create_ortho_camera(world, is_utm=False):
    bvxm_batch.init_process("bvxmCreateOrthoCameraProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_bool(1, is_utm)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    ortho_cam = dbvalue(id, type)
    return ortho_cam


def render_exp_image(cam, ni, nj, world, app_model, bin_index=0, scale_index=0):
    bvxm_batch.init_process("bvxmRenderExpectedImageProcess")
    bvxm_batch.set_input_from_db(0, cam)
    bvxm_batch.set_input_unsigned(1, ni)
    bvxm_batch.set_input_unsigned(2, nj)
    bvxm_batch.set_input_from_db(3, world)
    bvxm_batch.set_input_string(4, app_model)
    # set bin index to be 0 for all images
    bvxm_batch.set_input_unsigned(5, bin_index)
    bvxm_batch.set_input_unsigned(6, scale_index)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    out_conf_img = dbvalue(id, type)
    return out_img, out_conf_img


def render_exp_edge_img(cam, ni, nj, world, scale=0):
    bvxm_batch.init_process("bvxmExpectedEdgeImageProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_from_db(1, cam)
    bvxm_batch.set_input_unsigned(2, ni)
    bvxm_batch.set_input_unsigned(3, nj)
    bvxm_batch.set_input_unsigned(4, scale)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    exp_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    exp_img_byte = dbvalue(id, type)
    return exp_img, exp_img_byte


def update_appearance(img, cam, world, app_type, bin_index=0, scale_index=0, use_cache=0):
    bvxm_batch.init_process("bvxmUpdateProcess")
    bvxm_batch.set_input_from_db(0, img)
    bvxm_batch.set_input_from_db(1, cam)
    bvxm_batch.set_input_from_db(2, world)
    bvxm_batch.set_input_string(3, app_type)
    # set bin index to be 0 for all images
    bvxm_batch.set_input_unsigned(4, bin_index)
    bvxm_batch.set_input_unsigned(5, scale_index)
    bvxm_batch.set_input_unsigned(6, use_cache)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    density_img = dbvalue(id, type)
    (id, type) = bvxm_batch.commit_output(1)
    density_mask_img = dbvalue(id, type)
    return density_img, density_mask_img


def save_occupancy_raw(world, filename, app_model, scale=0):
    bvxm_batch.init_process("bvxmSaveOccupancyRawProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_string(1, filename)
    bvxm_batch.set_input_unsigned(2, scale)
    bvxm_batch.set_input_string(3, app_model)
    bvxm_batch.run_process()


def orthorectify(world, ortho_height_map, ortho_height_map_camera, input_image, input_image_camera):
    bvxm_batch.init_process("bvxmOrthorectifyProcess")
    bvxm_batch.set_input_from_db(0, ortho_height_map)
    bvxm_batch.set_input_from_db(1, ortho_height_map_camera)
    bvxm_batch.set_input_from_db(2, input_image)
    bvxm_batch.set_input_from_db(3, input_image_camera)
    bvxm_batch.set_input_from_db(4, world)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_ortho_img = dbvalue(id, type)
    return out_ortho_img

############## some utilities, put here for now ##########


def image_to_vrml_points(out_e_img, out_h_img, output_filename, prob_thres, max_scene_height):
    bvxm_batch.init_process("bvrmlImageToPointsProcess")
    bvxm_batch.set_input_from_db(0, out_e_img)
    bvxm_batch.set_input_from_db(1, out_h_img)
    bvxm_batch.set_input_string(2, output_filename)
    bvxm_batch.set_input_float(3, prob_thres)
    bvxm_batch.set_input_float(4, max_scene_height)
    bvxm_batch.run_process()
    (id, type) = bvxm_batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def name_suffix_for_camera(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat):
    if (lower_left_lat < 0):
        name = "S" + str(abs(lower_left_lat))
    else:
        name = "N" + str(lower_left_lat)
    if (lower_left_lon < 0):
        name = name + "W" + str(abs(lower_left_lon))
    else:
        name = name + "E" + str(lower_left_lon)
    name = name + "_S" + str(abs(upper_right_lat - lower_left_lat)) + \
        "x" + str(abs(upper_right_lon - lower_left_lon))
    return name

# Create x y z images for bvxm_scene from dem


def generate_xyz_from_dem(world, geotiff_dem, geoid_height, geocam=0, fill_in_value=-1.0):
    bvxm_batch.init_process("bvxmDemToXYZProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_string(1, geotiff_dem)
    bvxm_batch.set_input_double(2, geoid_height)
    bvxm_batch.set_input_from_db(3, geocam)
    bvxm_batch.set_input_float(4, fill_in_value)
    result = bvxm_batch.run_process()
    if result:
        (xi_id, xi_type) = bvxm_batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = bvxm_batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = bvxm_batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# Create x y z images for bvxm_scene from multiple dem image in image folder
# Assuming the camera are loaded from geotiff image header


def generate_xyz_from_dem_multi(world, img_folder, geoid_height, fill_in_value=-1.0):
    bvxm_batch.init_process("bvxmDemToXYZProcess2")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_string(1, img_folder)
    bvxm_batch.set_input_double(2, geoid_height)
    bvxm_batch.set_input_float(3, fill_in_value)
    result = bvxm_batch.run_process()
    if result:
        (xi_id, xi_type) = bvxm_batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = bvxm_batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = bvxm_batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img


def create_land_map(world, geo_folder, urban_folder, osm_folder, is_osm_pt, is_osm_region, is_osm_line, is_convert=True):
    bvxm_batch.init_process("bvxmCreateLandMapProcess")
    bvxm_batch.set_input_from_db(0, world)
    bvxm_batch.set_input_string(1, geo_folder)
    bvxm_batch.set_input_string(2, urban_folder)
    bvxm_batch.set_input_string(3, osm_folder)
    bvxm_batch.set_input_bool(4, is_osm_pt)
    bvxm_batch.set_input_bool(5, is_osm_line)
    bvxm_batch.set_input_bool(6, is_osm_region)
    bvxm_batch.set_input_bool(7, is_convert)
    result = bvxm_batch.run_process()
    if result:
        (img_id, img_type) = bvxm_batch.commit_output(0)
        land_img = dbvalue(img_id, img_type)
    else:
        land_img = 0
    return land_img


def scene_lvcs(world):
    bvxm_batch.init_process("bvxmSceneLvcsProcess")
    bvxm_batch.set_input_from_db(0, world)
    status = bvxm_batch.run_process()
    if status:
        (id, type) = bvxm_batch.commit_output(0)
        lvcs = dbvalue(id, type)
        return lvcs
    else:
        return 0
