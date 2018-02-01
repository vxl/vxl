# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file
import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()

#############################################################################
# PROVIDES higher level brad python functions to make batch
# code more readable/refactored
#############################################################################


# parse metadata of a sat image, reads sat image, nitf2 header and a
# metadata file in the same folder as the image if found
def read_nitf_metadata(nitf_filename, imd_folder="", verbose = False):
    batch.init_process("bradNITFReadMetadataProcess")
    # requires full path and name
    batch.set_input_string(0, nitf_filename)
    # pass empty if meta is in img folder
    batch.set_input_string(1, imd_folder)
    batch.set_input_bool(2, verbose)
    status = batch.run_process()
    meta = None
    if status:
        (id, type) = batch.commit_output(0)
        meta = dbvalue(id, type)
    return meta

# radiometrically normalize a sat image (cropped) based on its metadata


def radiometrically_calibrate(cropped_image, meta):
    batch.init_process("bradNITFAbsRadiometricCalibrationProcess")
    batch.set_input_from_db(0, cropped_image)
    batch.set_input_from_db(1, meta)
    status = batch.run_process()
    cropped_img_cal = None
    if status:
        (id, type) = batch.commit_output(0)
        cropped_img_cal = dbvalue(id, type)
    return cropped_img_cal

# estimate atmospheric parameters


def estimate_atmospheric_parameters(image, metadata, mean_reflectance=0.0, constrain_parameters = True, average_airlight = True):
    batch.init_process("bradEstimateAtmosphericParametersProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_float(2, mean_reflectance)
    batch.set_input_bool(3, constrain_parameters)
    batch.set_input_bool(4, average_airlight)
    status = batch.run_process()
    atm_params = None
    if status:
        (id, type) = batch.commit_output(0)
        atm_params = dbvalue(id, type)
    return atm_params

# convert radiance values to estimated reflectances


def estimate_reflectance(image, metadata, mean_reflectance = 0.0, average_airlight = True, is_normalize = True):
    batch.init_process("bradEstimateReflectanceProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_float(2, mean_reflectance)
    batch.set_input_bool(3, average_airlight)
    batch.set_input_bool(4, is_normalize)
    status = batch.run_process()
    reflectance_img = None
    if status:
        (id, type) = batch.commit_output(0)
        reflectance_img = dbvalue(id, type)
    return reflectance_img

# convert reflectance image back to digital count


def convert_reflectance_to_digital_count(reflectance_image, metadata, mean_reflectance = 0.0, normalize_0_1=False, max_digital_count=2047, average_airlight = True):
    batch.init_process("bradConvertReflectanceToDigitalCountProcess")
    batch.set_input_from_db(0, reflectance_image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_float(2, mean_reflectance)
    batch.set_input_bool(3, normalize_0_1)
    batch.set_input_unsigned(4, max_digital_count)
    batch.set_input_bool(5, average_airlight)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    output_img = dbvalue(id, type)
    return output_img


def find_sun_dir_bin(metadata, output_file):
    batch.init_process("bradSunDirBinProcess")
    batch.set_input_from_db(0, metadata)
    batch.set_input_string(1, output_file)
    status = batch.run_process()
    bin, angle = None, None
    if status:
        (bin_id, bin_type) = batch.commit_output(0)
        bin = batch.get_output_int(bin_id)
        batch.remove_data(bin_id)
        (angle_id, angle_type) = batch.commit_output(1)
        angle = batch.get_output_float(angle_id)
        batch.remove_data(angle_id)
    return bin, angle

# save atmospheric parameters


def save_atmospheric_parameters(atm_params, filename):
    batch.init_process("bradSaveAtmosphericParametersProcess")
    batch.set_input_from_db(0, atm_params)
    batch.set_input_string(1, filename)
    batch.run_process()
    return

# save image_metadata


def save_image_metadata(mdata, filename):
    batch.init_process("bradSaveImageMetadataProcess")
    batch.set_input_from_db(0, mdata)
    batch.set_input_string(1, filename)
    batch.run_process()
    return

# load image_metadata


def load_image_metadata(filename):
    batch.init_process("bradLoadImageMetadataProcess")
    batch.set_input_string(0, filename)
    status = batch.run_process()
    mdata = None
    if status:
        (id, type) = batch.commit_output(0)
        mdata = dbvalue(id, type)
    return mdata

# load atmospheric parameters


def load_atmospheric_parameters(filename):
    batch.init_process("bradLoadAtmosphericParametersProcess")
    batch.set_input_string(0, filename)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    atm_params = dbvalue(id, type)
    return atm_params

# compute sun angles given location and time


def get_sun_angles_date_time(lat, lon, year, month, day, hour, minute):
    batch.init_process("bradGetSunAnglesDateTimeProcess")
    batch.set_input_float(0, lat)
    batch.set_input_float(1, lon)
    batch.set_input_int(2, year)
    batch.set_input_int(3, month)
    batch.set_input_int(4, day)
    batch.set_input_int(5, hour)
    batch.set_input_int(6, minute)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    sun_el = batch.get_output_float(id)
    batch.remove_data(id)
    return sun_az, sun_el

# set sun angles in metadata


def set_sun_angles(mdata, sun_az, sun_el):
    batch.init_process("bradSetSunAnglesProcess")
    batch.set_input_from_db(0, mdata)
    batch.set_input_float(1, sun_az)
    batch.set_input_float(2, sun_el)
    batch.run_process()

# extract sun azimuth and elevation angles from metadata


def get_sun_angles(mdata):
    batch.init_process("bradGetSunAnglesProcess")
    batch.set_input_from_db(0, mdata)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    sun_el = batch.get_output_float(id)
    batch.remove_data(id)
    return sun_az, sun_el

# create a new image_metadata object


def create_image_metadata(gain=1.0, offset=0.0, view_az=0.0, view_el=90.0, sun_az=0.0, sun_el=90.0, sun_irrad=None):
    batch.init_process("bradCreateImageMetadataProcess")
    batch.set_input_float(0, gain)
    batch.set_input_float(1, offset)
    batch.set_input_float(2, view_az)
    batch.set_input_float(3, view_el)
    batch.set_input_float(4, sun_az)
    batch.set_input_float(5, sun_el)
    if sun_irrad != None:
        batch.set_input_float(6, sun_irrad)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mdata = dbvalue(id, type)
    return mdata

# create a new atmopsheric_parameters object


def create_atmospheric_parameters(airlight=0.0, skylight=0.0, optical_depth=0.0):
    batch.init_process("bradCreateAtmosphericParametersProcess")
    batch.set_input_float(0, airlight)
    batch.set_input_float(1, skylight)
    batch.set_input_float(2, optical_depth)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    atm_params = dbvalue(id, type)
    return atm_params


def estimate_shadow_density(image, metadata, atmospheric_params):
    batch.init_process("bradEstimateShadowsProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_from_db(2, atmospheric_params)
    batch.set_input_bool(3, True)  # True returns prob. density value
    batch.run_process()
    (id, type) = batch.commit_output(0)
    shadow_density = dbvalue(id, type)
    return shadow_density


def estimate_shadows(image, metadata, atmospheric_params):
    batch.init_process("bradEstimateShadowsProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_from_db(2, atmospheric_params)
    batch.set_input_bool(3, False)  # False returns probability value
    batch.run_process()
    (id, type) = batch.commit_output(0)
    shadow_probs = dbvalue(id, type)
    return shadow_probs


def load_eigenspace(filename):
    batch.init_process("bradLoadEigenspaceProcess")
    batch.set_input_string(0, filename)
    batch.run_process()
    (eig_id, eig_type) = batch.commit_output(0)
    eig = dbvalue(eig_id, eig_type)
    return eig


def classify_image(eig, h_no, h_atmos, input_image_filename, tile_ni, tile_nj):
    batch.init_process("bradClassifyImageProcess")
    batch.set_input_from_db(0, eig)
    batch.set_input_from_db(1, h_no)
    batch.set_input_from_db(2, h_atmos)
    batch.set_input_string(3, input_image_filename)
    batch.set_input_unsigned(4, tile_ni)
    batch.set_input_unsigned(5, tile_nj)
    batch.run_process()
    (vid, vtype) = batch.commit_output(0)
    q_img = dbvalue(vid, vtype)
    (vid, vtype) = batch.commit_output(1)
    q_img_orig_size = dbvalue(vid, vtype)
    return q_img, q_img_orig_size


def get_metadata_info(mdata):
    batch.init_process("bradGetMetaDataInfoProcess")
    batch.set_input_from_db(0, mdata)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    sun_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    sun_el = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    year = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    month = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(4)
    day = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(5)
    hour = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(6)
    minutes = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(7)
    seconds = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(8)
    gsd = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(9)
    sat_name = batch.get_output_string(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(10)
    view_az = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(11)
    view_el = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(12)
    band = batch.get_output_string(id)
    batch.remove_data(id)
    return sun_az, sun_el, year, month, day, hour, minutes, seconds, gsd, sat_name, view_az, view_el, band

def get_sat_name(mdata):
    sun_az, sun_el, year, month, day, hour, minutes, seconds, gsd, sat_name = get_metadata_info(mdata)
    return sat_name


def get_view_angles(mdata):
    sun_az, sun_el, year, month, day, hour, minutes, seconds, gsd, sat_name, view_az, view_el, band = get_metadata_info(mdata)
    return view_az, view_el

def compute_appearance_index(filenames):
    batch.init_process("bradComputeAppearanceIndexProcess");
    batch.set_input_string_array(0, filenames);
    batch.run_process();
    (index_id,index_type)=batch.commit_output(0);
    index_array = batch.get_bbas_1d_array_int(index_id);
    targets = [];
    neighbors = [];
    n = len(index_array);
    i = 0;
    not_end = True;
    while(not_end):
        target_id = index_array[i];
        targets.append(target_id);
        i+=1;
        neighbor_set = [];
        neighbor_id = index_array[i];
        while neighbor_id>=0 :
            neighbor_set.append(neighbor_id);
            i+=1;
            neighbor_id = index_array[i];
            if i >= n :
                not_end = False;
        neighbors.append(neighbor_set);
        i+=1; # skip the -1 neighbor terminator
        if i >= n :
            not_end = False;
    return targets, neighbors;


def get_image_coverage(mdata):
    batch.init_process("bradGetImageCoverageProcess")
    batch.set_input_from_db(0, mdata)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        ll_lon = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(1)
        ll_lat = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(2)
        ll_elev = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(3)
        ur_lon = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(4)
        ur_lat = batch.get_output_double(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(5)
        ur_elev = batch.get_output_double(id)
        batch.remove_data(id)
        return ll_lon, ll_lat, ll_elev, ur_lon, ur_lat, ur_elev
    else:
        return 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

def get_cloud_coverage(mdata):
    batch.init_process("bradGetCloudCoverageProcess")
    batch.set_input_from_db(0, mdata)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cloud_coverage = batch.get_output_float(id)
    batch.remove_data(id)
    return cloud_coverage

# create a new image_metadata object


def create_image_metadata(gain=1.0, offset=0.0, view_az=0.0, view_el=90.0, sun_az=0.0, sun_el=90.0, sun_irrad=None):
    batch.init_process("bradCreateImageMetadataProcess")
    batch.set_input_float(0, gain)
    batch.set_input_float(1, offset)
    batch.set_input_float(2, view_az)
    batch.set_input_float(3, view_el)
    batch.set_input_float(4, sun_az)
    batch.set_input_float(5, sun_el)
    if sun_irrad != None:
        batch.set_input_float(6, sun_irrad)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    mdata = dbvalue(id, type)
    return mdata

# create a new atmopsheric_parameters object


def create_atmospheric_parameters(airlight=0.0, skylight=0.0, optical_depth=0.0):
    batch.init_process("bradCreateAtmosphericParametersProcess")
    batch.set_input_float(0, airlight)
    batch.set_input_float(1, skylight)
    batch.set_input_float(2, optical_depth)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    atm_params = dbvalue(id, type)
    return atm_params


def estimate_shadow_density(image, metadata, atmospheric_params):
    batch.init_process("bradEstimateShadowsProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_from_db(2, atmospheric_params)
    batch.set_input_bool(3, True)  # True returns prob. density value
    batch.run_process()
    (id, type) = batch.commit_output(0)
    shadow_density = dbvalue(id, type)
    return shadow_density


def estimate_shadows(image, metadata, atmospheric_params):
    batch.init_process("bradEstimateShadowsProcess")
    batch.set_input_from_db(0, image)
    batch.set_input_from_db(1, metadata)
    batch.set_input_from_db(2, atmospheric_params)
    batch.set_input_bool(3, False)  # False returns probability value
    batch.run_process()
    (id, type) = batch.commit_output(0)
    shadow_probs = dbvalue(id, type)
    return shadow_probs


def load_eigenspace(filename):
    batch.init_process("bradLoadEigenspaceProcess")
    batch.set_input_string(0, filename)
    batch.run_process()
    (eig_id, eig_type) = batch.commit_output(0)
    eig = dbvalue(eig_id, eig_type)
    return eig


def classify_image(eig, h_no, h_atmos, input_image_filename, tile_ni, tile_nj):
    batch.init_process("bradClassifyImageProcess")
    batch.set_input_from_db(0, eig)
    batch.set_input_from_db(1, h_no)
    batch.set_input_from_db(2, h_atmos)
    batch.set_input_string(3, input_image_filename)
    batch.set_input_unsigned(4, tile_ni)
    batch.set_input_unsigned(5, tile_nj)
    batch.run_process()
    (vid, vtype) = batch.commit_output(0)
    q_img = dbvalue(vid, vtype)
    (vid, vtype) = batch.commit_output(1)
    q_img_orig_size = dbvalue(vid, vtype)
    return q_img, q_img_orig_size
