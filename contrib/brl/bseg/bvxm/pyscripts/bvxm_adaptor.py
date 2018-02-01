import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)
import os
#############################################################################
# PROVIDES higher level python functions to make bvxm_batch
# code more readable/refactored
#############################################################################

# loads/creates scene


def create_scene(scene_params_xml):
    batch.init_process("bvxmCreateVoxelWorldProcess")
    # "./bvxmCreateVoxelWorldProcess.xml"
    batch.set_params_process(scene_params_xml)
    status = batch.run_process()
    if status != 1:
        raise Exception('Error creating scene from ' + scene_params_xml)
    (world_id, world_type) = batch.commit_output(0)
    world = dbvalue(world_id, world_type)
    return world

# create scene xml


def create_scene_xml(scene_xml, world_dir, lvcs, lvcs_file, dim_x, dim_y, dim_z, voxel_size=1.0, corner_x=0.0, corner_y=0.0, corner_z=0.0,
                     min_ocp_prob=0.001, max_ocp_prob=0.999, max_scale=1):
    batch.init_process("bvxmCreateSceneXmlProcess")
    batch.set_input_string(0, scene_xml)
    batch.set_input_string(1, world_dir)
    batch.set_input_float(2,  corner_x)
    batch.set_input_float(3,  corner_y)
    batch.set_input_float(4,  corner_z)
    batch.set_input_unsigned(5, dim_x)
    batch.set_input_unsigned(6, dim_y)
    batch.set_input_unsigned(7, dim_z)
    batch.set_input_float(8, voxel_size)
    batch.set_input_from_db(9, lvcs)
    batch.set_input_string(10, lvcs_file)
    batch.set_input_float(11, min_ocp_prob)
    batch.set_input_float(12, max_ocp_prob)
    batch.set_input_unsigned(13, max_scale)
    return batch.run_process()


def create_scene_large_scale(roi_kml, scene_root, world_dir, dem_folder, world_size=500.0, voxel_size=1.0, height_diff=120.0, height_sub=25.0, land_folder=""):
    batch.init_process("bvxmCreateSceneXmlLargeScaleProcess")
    batch.set_input_string(0, roi_kml)
    batch.set_input_string(1, scene_root)
    batch.set_input_string(2, world_dir)
    batch.set_input_string(3, dem_folder)
    batch.set_input_string(4, land_folder)
    batch.set_input_float(5, world_size)
    batch.set_input_float(6, voxel_size)
    batch.set_input_float(7, height_diff)
    batch.set_input_float(8, height_sub)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        n_scenes = batch.get_output_unsigned(id)
        return n_scenes
    else:
        return 0


def scene_box(scene):
    batch.init_process("bvxmSceneBoxProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lower_left_lon = batch.get_output_double(id)
    (id, type) = batch.commit_output(1)
    lower_left_lat = batch.get_output_double(id)
    (id, type) = batch.commit_output(2)
    lower_left_elev = batch.get_output_double(id)
    (id, type) = batch.commit_output(3)
    upper_right_lon = batch.get_output_double(id)
    (id, type) = batch.commit_output(4)
    upper_right_lat = batch.get_output_double(id)
    (id, type) = batch.commit_output(5)
    upper_right_elev = batch.get_output_double(id)
    return lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev

# changed to always draw a filled polygon, default alpha value (a) is set to 0 so that the box is completely transparent though so it will look as an empty polygon
# set a to e.g. 100 for a semi-transparent polygon, set a to 255 for an
# opaque polygon


def write_scene_kml(scene, kml_filename, is_overwrite=True, r=255, g=255, b=255, a=0, name=""):
    batch.init_process("bvxmSceneKmlProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, kml_filename)
    batch.set_input_bool(2, is_overwrite)
    batch.set_input_unsigned(3, r)
    batch.set_input_unsigned(4, g)
    batch.set_input_unsigned(5, b)
    batch.set_input_unsigned(6, a)
    batch.set_input_string(7, name)
    batch.run_process()

# check whether the scene is intersects with a given kml polygon


def check_scene_poly_overlap(scene, kml_file):
    batch.init_process("bvxmScenePolyOverlapProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1, kml_file)
    status = batch.run_process()
    return status


def scene_origin(scene):
    batch.init_process("bvxmSceneOriginProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lower_left_lon = batch.get_output_double(id)
    (id, type) = batch.commit_output(1)
    lower_left_lat = batch.get_output_double(id)
    (id, type) = batch.commit_output(2)
    lower_left_z = batch.get_output_double(id)
    return lower_left_lon, lower_left_lat, lower_left_z

# return the scene bbox and voxel size in local coordinates


def scene_local_box(scene):
    batch.init_process("bvxmSceneLocalBoxProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lower_left_x = batch.get_output_double(id)
    (id, type) = batch.commit_output(1)
    lower_left_y = batch.get_output_double(id)
    (id, type) = batch.commit_output(2)
    upper_right_x = batch.get_output_double(id)
    (id, type) = batch.commit_output(3)
    upper_right_y = batch.get_output_double(id)
    (id, type) = batch.commit_output(4)
    voxel_size = batch.get_output_double(id)
    (id, type) = batch.commit_output(5)
    lower_left_z = batch.get_output_double(id)  # min z
    (id, type) = batch.commit_output(6)
    upper_right_z = batch.get_output_double(id)  # max z
    return lower_left_x, lower_left_y, upper_right_x, upper_right_y, voxel_size, lower_left_z, upper_right_z

# return the scene world directory


def model_dir(scene):
    batch.init_process("bvxmSceneModelDirProcess")
    batch.set_input_from_db(0, scene)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    model_dir = batch.get_output_string(id)
    return model_dir


def roi_init(image_filename, cam, world, roi_init_params_xml, is_all_bits=False):
    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_filename)
    batch.set_input_from_db(1, cam)
    batch.set_input_from_db(2, world)
    batch.set_input_bool(3, is_all_bits)
    # "bvxmRoiInitProcess.xml")
    batch.set_params_process(roi_init_params_xml)
    statuscode = batch.run_process()
    if statuscode:
        (cropped_cam_id, cropped_cam_type) = batch.commit_output(0)
        cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type)
        (cropped_image_id, cropped_image_type) = batch.commit_output(1)
        cropped_image = dbvalue(cropped_image_id, cropped_image_type)
        (uncertainty_id, uncertainty_type) = batch.commit_output(2)
        uncertainty = dbvalue(uncertainty_id, uncertainty_type)
        return statuscode, cropped_cam, cropped_image, uncertainty
    else:
        return statuscode, dbvalue(0, ""), dbvalue(0, ""), dbvalue(0, "")


def bvxm_detect_edges(cropped_image, edge_params_xml):
    batch.init_process("bvxmDetectEdgesProcess")
    batch.set_input_from_db(0, cropped_image)
    batch.set_params_process(edge_params_xml)
    batch.run_process()
    (cropped_edge_image_id, cropped_edge_image) = batch.commit_output(0)
    cropped_edge_image = dbvalue(cropped_edge_image_id, cropped_edge_image)
    return cropped_edge_image


def update_edges(world, cropped_cam, cropped_edge_image, edge_prob_mask_size=21, edge_prob_mask_sigma=1.0, scale=0):
    batch.init_process("bvxmUpdateEdgesProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_from_db(1, cropped_cam)
    batch.set_input_from_db(2, cropped_edge_image)
    batch.set_input_unsigned(3, 0)
    batch.set_input_int(4, edge_prob_mask_size)
    batch.set_input_float(5, edge_prob_mask_sigma)
# batch.set_params_process(update_params_xml); #
# "./bvxmUpdateEdgesProcess.xml");
    batch.run_process()


def rpc_registration(world, cropped_cam, cropped_edge_image, uncertainty, shift_3d_flag=0, scale=0, is_uncertainty_float=0):
    batch.init_process("bvxmRpcRegistrationProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_from_db(1, cropped_cam)
    batch.set_input_from_db(2, cropped_edge_image)
    batch.set_input_bool(3, shift_3d_flag)
    if is_uncertainty_float == 1:
        print "uncertainty = ", uncertainty
        batch.set_input_float(4, uncertainty)
    else:
        batch.set_input_from_db(4, uncertainty)
    batch.set_input_unsigned(5, scale)
    batch.run_process()
    (cam_id, cam_type) = batch.commit_output(0)
    cam = dbvalue(cam_id, cam_type)
    (expected_edge_image_id, expected_edge_image_type) = batch.commit_output(1)
    expected_edge_image = dbvalue(
        expected_edge_image_id, expected_edge_image_type)
    (offset_u_id, offset_u_type) = batch.commit_output(2)
    offset_u = batch.get_output_double(offset_u_id)
    (offset_v_id, offset_v_type) = batch.commit_output(3)
    offset_v = batch.get_output_double(offset_v_id)
    return cam, expected_edge_image, offset_u, offset_v


def render_height_map(world):
    print("Rendering height map")
    batch.init_process("bvxmHeightmapOrthoProcess")
    batch.set_input_from_db(0, world)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_d_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_h_img = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    out_conf_img = dbvalue(id, type)
    return out_h_img, out_d_img, out_conf_img


def render_height_map_with_cam(world, input_cam, ni, nj, is_negate=False):
    batch.init_process("bvxmHeightmapProcess")
    batch.set_input_from_db(0, input_cam)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_from_db(3, world)
    batch.set_input_bool(4, is_negate)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_d_img = dbvalue(id, type)
    return out_d_img


def render_height_map_expected_with_cam(world, input_cam, ni, nj):
    batch.init_process("bvxmHeightmapExpectedProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_from_db(1, input_cam)
    batch.set_input_unsigned(2, ni)
    batch.set_input_unsigned(3, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_h_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_var_img = dbvalue(id, type)
    return out_h_img, out_var_img


def render_uncertainty_img(input_cam, ni, nj, world):
    batch.init_process("bvxmUncertaintyProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_from_db(1, input_cam)
    batch.set_input_unsigned(2, ni)
    batch.set_input_unsigned(3, nj)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    return out_img


def render_ortho_edgemap(world, scale=0):
    print("Rendering ortho edge map")
    batch.init_process("bvxmEdgemapOrthoProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_unsigned(1, 0)  # scale
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_e_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_e_img_byte = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    out_h_img = dbvalue(id, type)
    (id, type) = batch.commit_output(3)
    ortho_cam = dbvalue(id, type)
    return out_e_img, out_e_img_byte, out_h_img, ortho_cam


def create_ortho_camera(world, is_utm=False):
    batch.init_process("bvxmCreateOrthoCameraProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_bool(1, is_utm)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    ortho_cam = dbvalue(id, type)
    return ortho_cam


def render_exp_image(cam, ni, nj, world, app_model, bin_index=0, scale_index=0):
    batch.init_process("bvxmRenderExpectedImageProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_from_db(3, world)
    batch.set_input_string(4, app_model)
    # set bin index to be 0 for all images
    batch.set_input_unsigned(5, bin_index)
    batch.set_input_unsigned(6, scale_index)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_conf_img = dbvalue(id, type)
    return out_img, out_conf_img


def render_exp_edge_img(cam, ni, nj, world, scale=0):
    batch.init_process("bvxmExpectedEdgeImageProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_from_db(1, cam)
    batch.set_input_unsigned(2, ni)
    batch.set_input_unsigned(3, nj)
    batch.set_input_unsigned(4, scale)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    exp_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    exp_img_byte = dbvalue(id, type)
    return exp_img, exp_img_byte


def update_appearance(img, cam, world, app_type, bin_index=0, scale_index=0, use_cache=0):
    batch.init_process("bvxmUpdateProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, cam)
    batch.set_input_from_db(2, world)
    batch.set_input_string(3, app_type)
    # set bin index to be 0 for all images
    batch.set_input_unsigned(4, bin_index)
    batch.set_input_unsigned(5, scale_index)
    batch.set_input_unsigned(6, use_cache)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    density_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    density_mask_img = dbvalue(id, type)
    return density_img, density_mask_img


def save_occupancy_raw(world, filename, app_model, scale=0):
    batch.init_process("bvxmSaveOccupancyRawProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_string(1, filename)
    batch.set_input_unsigned(2, scale)
    batch.set_input_string(3, app_model)
    batch.run_process()


def orthorectify(world, ortho_height_map, ortho_height_map_camera, input_image, input_image_camera):
    batch.init_process("bvxmOrthorectifyProcess")
    batch.set_input_from_db(0, ortho_height_map)
    batch.set_input_from_db(1, ortho_height_map_camera)
    batch.set_input_from_db(2, input_image)
    batch.set_input_from_db(3, input_image_camera)
    batch.set_input_from_db(4, world)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_ortho_img = dbvalue(id, type)
    return out_ortho_img

############## some utilities, put here for now ##########


def image_to_vrml_points(out_e_img, out_h_img, output_filename, prob_thres, max_scene_height):
    batch.init_process("bvrmlImageToPointsProcess")
    batch.set_input_from_db(0, out_e_img)
    batch.set_input_from_db(1, out_h_img)
    batch.set_input_string(2, output_filename)
    batch.set_input_float(3, prob_thres)
    batch.set_input_float(4, max_scene_height)
    batch.run_process()
    (id, type) = batch.commit_output(0)
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
    batch.init_process("bvxmDemToXYZProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_string(1, geotiff_dem)
    batch.set_input_double(2, geoid_height)
    batch.set_input_from_db(3, geocam)
    batch.set_input_float(4, fill_in_value)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img

# Create x y z images for bvxm_scene from multiple dem image in image folder
# Assuming the camera are loaded from geotiff image header


def generate_xyz_from_dem_multi(world, img_folder, geoid_height, fill_in_value=-1.0):
    batch.init_process("bvxmDemToXYZProcess2")
    batch.set_input_from_db(0, world)
    batch.set_input_string(1, img_folder)
    batch.set_input_double(2, geoid_height)
    batch.set_input_float(3, fill_in_value)
    result = batch.run_process()
    if result:
        (xi_id, xi_type) = batch.commit_output(0)
        x_img = dbvalue(xi_id, xi_type)
        (yi_id, yi_type) = batch.commit_output(1)
        y_img = dbvalue(yi_id, yi_type)
        (zi_id, zi_type) = batch.commit_output(2)
        z_img = dbvalue(zi_id, zi_type)
    else:
        x_img = 0
        y_img = 0
        z_img = 0
    return x_img, y_img, z_img


def create_land_map(world, geo_folder, urban_folder, osm_folder, is_osm_pt, is_osm_region, is_osm_line, is_convert=True):
    batch.init_process("bvxmCreateLandMapProcess")
    batch.set_input_from_db(0, world)
    batch.set_input_string(1, geo_folder)
    batch.set_input_string(2, urban_folder)
    batch.set_input_string(3, osm_folder)
    batch.set_input_bool(4, is_osm_pt)
    batch.set_input_bool(5, is_osm_line)
    batch.set_input_bool(6, is_osm_region)
    batch.set_input_bool(7, is_convert)
    result = batch.run_process()
    if result:
        (img_id, img_type) = batch.commit_output(0)
        land_img = dbvalue(img_id, img_type)
    else:
        land_img = 0
    return land_img


def scene_lvcs(world):
    batch.init_process("bvxmSceneLvcsProcess")
    batch.set_input_from_db(0, world)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        lvcs = dbvalue(id, type)
        return lvcs
    else:
        return 0
