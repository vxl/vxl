# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

#############################################################################
# PROVIDES higher level bbas python functions to make batch
# code more readable/refactored
#############################################################################


def atmospheric_correct(image, sun_z):
    batch.init_process('bbasAtmosphericCorrProcess')
    batch.set_input_from_db(0, image)
    batch.set_input_float(1, sun_z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image_corrected = dbvalue(id, type)
    return image_corrected


def estimate_irradiance(image, sun_z, mean_albedo=1.0):
    batch.init_process('bbasEstimateIrradianceProcess')
    batch.set_input_from_db(0, image)
    batch.set_input_float(1, sun_z)
    batch.set_input_float(2, mean_albedo)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    img_float = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    irrad = batch.get_output_float(id)
    batch.remove_data(id)
    return(img_float, irrad)


def sun_angles(image_path):
    batch.init_process('bbasSunAnglesProcess')
    batch.set_input_string(0, image_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_el = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    sun_az = batch.get_output_float(id)
    batch.remove_data(id)
    return(sun_az, sun_el)


def sun_position(image_path):
    batch.init_process('bbasSunAnglesProcess')
    batch.set_input_string(0, image_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_el = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    sun_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    sun_dist = batch.get_output_float(id)
    batch.remove_data(id)
    return(sun_az, sun_el, sun_dist)


def camera_angles(camera, x, y, z):
    batch.init_process('bbasCameraAnglesProcess')
    batch.set_input_from_db(0, camera)
    batch.set_input_float(1, x)
    batch.set_input_float(2, y)
    batch.set_input_float(3, z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    cam_el = batch.get_output_float(id)
    batch.remove_data(id)
    return (cam_az, cam_el)


def bbox_from_ply(filename):
    minpoint = list()
    maxpoint = list()
    batch.init_process('imeshPlyBboxProcess')
    batch.set_input_string(0, filename)
    batch.run_process()
    for i in (0, 1, 2):
        (id, type) = batch.commit_output(i)
        minpoint.append(batch.get_output_double(id))
        batch.remove_data(id)
    for i in (3, 4, 5):
        (id, type) = batch.commit_output(i)
        maxpoint.append(batch.get_output_double(id))
        batch.remove_data(id)
    return (minpoint, maxpoint)


def estimate_radiance_values(image, sun_el, sun_dist, sensor_el, solar_irrad=None, downwelled_irrad=None, optical_depth=None):
    batch.init_process("bradEstimateRadianceValuesProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_float(1, sun_el)
    batch.set_input_float(2, sun_dist)
    batch.set_input_float(3, sensor_el)
    if solar_irrad != None:
        batch.set_input_float(4, solar_irrad)
    if downwelled_irrad != None:
        batch.set_input_float(5, downwelled_irrad)
    if optical_depth != None:
        batch.set_input_float(6, optical_depth)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    airlight = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    ref_horizontal = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    ref_sun_facing = batch.get_output_float(id)
    batch.remove_data(id)
    return (airlight, ref_horizontal, ref_sun_facing)


def save_sun_index(output_file_name, longitude, latitude, year, hour, minute, radius):
    batch.init_process("bradSaveSunIndexProcess")
    batch.set_input_string(0, output_file_name)
    batch.set_input_float(1, longitude)
    batch.set_input_float(2, latitude)
    batch.set_input_int(3, year)
    batch.set_input_int(4, hour)
    batch.set_input_int(5, minute)
    batch.set_input_int(6, radius)
    status = batch.run_process()
    return status


def sun_dir_bin(meta, illum_bin_filename):
    batch.init_process("bradSunDirBinProcess")
    batch.set_input_from_db(0, meta)
    batch.set_input_string(1, illum_bin_filename)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    bin = batch.get_output_int(id)
    batch.remove_data(id)
    return bin


def prob_as_expected(image, atomicity):
    batch.init_process("bslExpectedImageProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_float(1, atomicity)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    image_prob = dbvalue(id, type)
    return image_prob


def fuse_beliefs(image1, image2, atomicity):
    batch.init_process("bslFusionProcess")
    batch.set_input_from_db(0, image1)
    batch.set_input_from_db(1, image2)
    batch.set_input_float(2, atomicity)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    fused_image = dbvalue(id, type)
    return fused_image

# Removes elements from brdb (list of elements, or just one)


def remove_from_db(dbvals):
    if not isinstance(dbvals, (list, tuple)):
        dbvals = [dbvals]
    for dbval in dbvals:
        batch.init_process("bbasRemoveFromDbProcess")
        batch.set_input_unsigned(0, dbval.id)
        batch.run_process()

# redirects std out to a file


def set_stdout(file):
    batch.set_stdout(file)

# resets std out to terminal


def reset_stdout():
    batch.reset_stdout()


def vrml_initialize(vrml_filename):
    batch.init_process("bvrmlInitializeProcess")
    batch.set_input_string(0, vrml_filename)
    batch.run_process()


def vrml_write_box(vrml_filename, bbox, is_wire, r, g, b):
    batch.init_process("bvrmlWriteBoxProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_double(1, bbox[0])  # minx
    batch.set_input_double(2, bbox[1])  # miny
    batch.set_input_double(3, bbox[2])  # minz
    batch.set_input_double(4, bbox[3])  # maxx
    batch.set_input_double(5, bbox[4])  # maxy
    batch.set_input_double(6, bbox[5])  # maxz
    batch.set_input_bool(7, is_wire)
    batch.set_input_float(8, r)
    batch.set_input_float(9, g)
    batch.set_input_float(10, b)
    batch.run_process()


def vrml_write_origin(vrml_filename, axis_len):
    batch.init_process("bvrmlWriteOriginAndAxesProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_float(1, axis_len)
    batch.run_process()


def vrml_write_point(vrml_filename, coords, radius, r, g, b):
    batch.init_process("bvrmlWritePointProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_float(1, coords[0])  # x
    batch.set_input_float(2, coords[1])  # y
    batch.set_input_float(3, coords[2])  # z
    batch.set_input_float(4, radius)  # radius
    batch.set_input_float(5, r)  # red
    batch.set_input_float(6, g)  # green
    batch.set_input_float(7, b)  # blue
    batch.run_process()


def vrml_write_line(vrml_filename, pt1_coords, pt2_coords, r, g, b):
    batch.init_process("bvrmlWriteLineProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_float(1, pt1_coords[0])  # x
    batch.set_input_float(2, pt1_coords[1])  # y
    batch.set_input_float(3, pt1_coords[2])  # z
    batch.set_input_float(4, pt2_coords[0])  # x
    batch.set_input_float(5, pt2_coords[1])  # y
    batch.set_input_float(6, pt2_coords[2])  # z
    batch.set_input_float(7, r)  # red
    batch.set_input_float(8, g)  # green
    batch.set_input_float(9, b)  # blue
    batch.run_process()


def vrml_filter_ply(vrml_filename, ply_file, points_file, dist_threshold, nearest=False):
    batch.init_process("bvrmlFilteredPlyProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_string(1, ply_file)
    batch.set_input_string(2, points_file)
    batch.set_input_float(3, dist_threshold)
    batch.set_input_bool(4, nearest)
    batch.run_process()


def initialize_rng(seed=0):
    batch.init_process("bstaInitializeRandomSeedProcess")
    batch.set_input_unsigned(0, seed)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    rng = dbvalue(id, type)
    return rng


def load_joint_hist3d(filename):
    batch.init_process("bstaLoadJointHist3dProcess")
    batch.set_input_string(0, filename)
    batch.run_process()
    (h_id, h_type) = batch.commit_output(0)
    h = dbvalue(h_id, h_type)
    return h


def texture_classifier_kernel_margin(dictionary):
    batch.init_process("sdetTextureClassifierKernelMarginProcess")
    batch.set_input_string(0, dictionary)
    batch.run_process()
    (m_id, m_type) = batch.commit_output(0)
    margin = batch.get_output_int(m_id)
    batch.remove_data(m_id)
    return margin


def texture_classifier(tcl, dictionary, img, block_size=64):
    batch.init_process("sdetTextureClassifierProcess")
    batch.set_input_from_db(0, tcl)
    batch.set_input_string(1, dictionary)
    batch.set_input_from_db(2, img)
    batch.set_input_unsigned(3, block_size)    # size of blocks
    batch.run_process()
    (img_id, img_type) = batch.commit_output(0)
    img_classified = dbvalue(img_id, img_type)
    return img_classified


def create_texture_classifier(lambda0, lambda1, n_scales, scale_interval, angle_interval, laplace_radius, gauss_radius, k, n_samples):
    batch.init_process("sdetCreateTextureClassifierProcess")
    batch.set_input_float(0, lambda0)
    batch.set_input_float(1, lambda1)
    batch.set_input_unsigned(2, n_scales)
    batch.set_input_float(3, scale_interval)
    batch.set_input_float(4, angle_interval)
    batch.set_input_float(5, laplace_radius)
    batch.set_input_float(6, gauss_radius)
    batch.set_input_unsigned(7, k)
    batch.set_input_unsigned(8, n_samples)
    batch.run_process()
    (tclsf_id, tclsf_type) = batch.commit_output(0)
    tclsf = dbvalue(tclsf_id, tclsf_type)
    return tclsf


def texture_training(tclsf, image, exp_poly_path, texton_dict_path, name_of_category="mod", compute_textons=True):
    batch.init_process("sdetTextureTrainingProcess")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_bool(1, compute_textons)  # compute textons
    batch.set_input_from_db(2, image)
    batch.set_input_string(3, exp_poly_path)
    batch.set_input_string(4, name_of_category)
    batch.set_input_string(5, texton_dict_path)
    batch.run_process()
    # write out the texton dictionary on finish
    batch.finish_process()


def print_texton_dictionary(tclsf, texton_dict_path, print_mode="inter_dist"):
    batch.init_process("sdetPrintTextonDictProcess")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_string(1, texton_dict_path)
    batch.set_input_string(2, print_mode)
    batch.run_process()


def texture_classifier_exp_img(tclsf, dict_expected, fimage, fexp, block_size=64):
    batch.init_process("sdetExpImgClassifierProcess")
    batch.set_input_from_db(0, tclsf)
    batch.set_input_string(1, dict_expected)
    batch.set_input_from_db(2, fimage)
    batch.set_input_from_db(3, fexp)
    batch.set_input_unsigned(4, 64)
    batch.run_process()
    (text_class_id, text_class_type) = batch.commit_output(0)
    text_class = dbvalue(text_class_id, text_class_type)
    return text_class


def solve_gain_offset(model_image, test_image, test_mask):
    batch.init_process("bripSolveGainOffsetProcess")
    batch.set_input_from_db(0, model_image)
    batch.set_input_from_db(1, test_image)
    batch.set_input_from_db(3, test_mask)
    batch.run_process()
    (map_image_id, map_image_type) = batch.commit_output(0)
    map_image = dbvalue(map_image_id, map_image_type)
    return map_image


def triangulate_site_corrs(site_file, out_file):
    batch.init_process("bwmTriangulateCorrProcess")
    batch.set_input_string(0, site_file)
    batch.set_input_string(1, out_file)
    batch.run_process()


def generate_depth_maps(depth_scene_file, output_folder, output_name_prefix, downsampling_level=0):
    batch.init_process("bpglGenerateDepthMapsProcess")
    batch.set_input_string(0, depth_scene_file)
    batch.set_input_unsigned(1, downsampling_level)
    batch.set_input_string(2, output_folder)
    batch.set_input_string(3, output_name_prefix)
    batch.run_process()
