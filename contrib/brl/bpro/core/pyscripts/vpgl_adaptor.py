# import the batch module and dbvalue from init
# set the global variable, batch, on init before importing this file

import brl_init
dbvalue = brl_init.DummyBatch()
batch = brl_init.DummyBatch()


class VpglException(brl_init.BrlException):
    pass

#############################################################################
# PROVIDES higher level vpgl python functions to make batch
# code more readable/refactored
#############################################################################

import math

# ;
# camera loading;
# ;


def load_perspective_camera(file_path):
    batch.init_process("vpglLoadPerspectiveCameraProcess")
    batch.set_input_string(0, file_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_affine_camera(file_path, viewing_dist=1000, look_dir=(0, 0, -1)):
    """ load an affine camera from a text file containing the projection matrix
        The viewing rays will be flipped such that the dot product with look_dir is positive
    """
    batch.init_process("vpglLoadAffineCameraProcess")
    batch.set_input_string(0, file_path)
    batch.set_input_double(1, viewing_dist)
    batch.set_input_double(2, look_dir[0])
    batch.set_input_double(3, look_dir[1])
    batch.set_input_double(4, look_dir[2])
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_projective_camera(file_path):
    batch.init_process("vpglLoadProjCameraProcess")
    batch.set_input_string(0, file_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam
#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z), up = (x,y,z);


def create_perspective_camera(scale, ppoint, center, look_pt, up=[0, 1, 0]):
    batch.init_process("vpglCreatePerspectiveCameraProcess")
    batch.set_input_double(0, scale[0])
    batch.set_input_double(1, ppoint[0])
    batch.set_input_double(2, scale[1])
    batch.set_input_double(3, ppoint[1])
    batch.set_input_double(4, center[0])
    batch.set_input_double(5, center[1])
    batch.set_input_double(6, center[2])
    batch.set_input_double(7, look_pt[0])
    batch.set_input_double(8, look_pt[1])
    batch.set_input_double(9, look_pt[2])
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def create_perspective_camera_from_kml(ni, nj, right_fov, top_fov, altitude, heading, tilt, roll, cent_x, cent_y):
    batch.init_process("vpglCreatePerspCameraFromKMLProcess")
    batch.set_input_unsigned(0, ni)
    batch.set_input_unsigned(1, nj)
    batch.set_input_double(2, right_fov)
    batch.set_input_double(3, top_fov)
    batch.set_input_double(4, altitude)
    batch.set_input_double(5, heading)
    batch.set_input_double(6, tilt)
    batch.set_input_double(7, roll)
    batch.set_input_double(8, cent_x)
    batch.set_input_double(9, cent_y)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_perspective_camera_from_kml_file(NI, NJ, kml_file):
    batch.init_process("vpglLoadPerspCameraFromKMLFileProcess")
    batch.set_input_unsigned(0, NI)
    batch.set_input_unsigned(1, NJ)
    batch.set_input_string(2, kml_file)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    longitude = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    latitude = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    altitude = batch.get_output_double(id)
    batch.remove_data(id)
    return cam, longitude, latitude, altitude

# resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);


def resample_perspective_camera(cam, size0, size1):
    batch.init_process("vpglResamplePerspectiveCameraProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_int(1, size0[0])
    batch.set_input_int(2, size0[1])
    batch.set_input_int(3, size1[0])
    batch.set_input_int(4, size1[1])
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out = dbvalue(id, type)
    return out
# resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);


def get_perspective_camera_center(cam):
    batch.init_process("vpglGetPerspectiveCamCenterProcess")
    batch.set_input_from_db(0, cam)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)
    return x, y, z


def get_backprojected_ray(cam, u, v):
    batch.init_process("vpglGetBackprojectRayProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_float(1, u)
    batch.set_input_float(2, v)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)
    return x, y, z
# returns cartesian cam center from azimuth (degrees), elevation
# (degrees), radius, look point;


def get_rpc_backprojected_ray(cam, u, v, altitude, initial_lon, initial_lat, initial_alt):
    batch.init_process("vpglGetRpcBackprojectRayProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_double(1, u)
    batch.set_input_double(2, v)
    batch.set_input_double(3, altitude)
    batch.set_input_double(4, initial_lon)
    batch.set_input_double(5, initial_lat)
    batch.set_input_double(6, initial_alt)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_double(id)
    batch.remove_data(id)
    return x, y, z


def get_camera_center(azimuth, elevation, radius, lookPt):
    deg_to_rad = math.pi / 180.0
    el = elevation * deg_to_rad
    az = azimuth * deg_to_rad
    cx = radius * math.sin(el) * math.cos(az)
    cy = radius * math.sin(el) * math.sin(az)
    cz = radius * math.cos(el)
    center = (cx + lookPt[0], cy + lookPt[1], cz + lookPt[2])
    return center

# returns spherical coordinates about sCenter given cartesian point;


def cart2sphere(cartPt, sCenter):
    # offset cart point;
    cartPt = numpy.subtract(cartPt, sCenter)
    rad = math.sqrt(sum(cartPt * cartPt))
    az = math.atan2(cartPt[1], cartPt[0])
    el = math.acos(cartPt[2] / rad)
    return (math.degrees(az), math.degrees(el), rad)


def load_rational_camera(file_path):
    batch.init_process("vpglLoadRationalCameraProcess")
    batch.set_input_string(0, file_path)
    status = batch.run_process()
    cam = None
    if status:
        (id, type) = batch.commit_output(0)
        cam = dbvalue(id, type)
        return cam
    else:
        raise VpglException("Failed to load Rational Camera")


def load_local_rational_camera(file_path):
    batch.init_process("vpglLoadLocalRationalCameraProcess")
    batch.set_input_string(0, file_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_rational_camera_nitf(file_path):
    batch.init_process("vpglLoadRationalCameraNITFProcess")
    batch.set_input_string(0, file_path)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_rational_camera_from_txt(file_path):
    batch.init_process("vpglLoadRationalCameraFromTXTProcess")
    batch.set_input_string(0, file_path)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cam = dbvalue(id, type)
        return cam
    else:
        raise VpglException("Failed to load Rational Camera from txt")


def convert_local_rational_perspective_camera(local_cam):
    batch.init_process("vpglConvertLocalRationalToPerspectiveProcess")
    batch.set_input_from_db(0, local_cam)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def create_local_rational_camera(rational_cam_fname, lvcs_fname):
    batch.init_process('vpglCreateLocalRationalCameraProcess')
    batch.set_input_string(0, rational_cam_fname)
    batch.set_input_string(1, lvcs_fname)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def convert_to_local_rational_camera(rational_cam, lvcs):
    batch.init_process('vpglConvertToLocalRationalCameraProcess')
    batch.set_input_from_db(0, rational_cam)
    batch.set_input_from_db(1, lvcs)
    status = batch.run_process()
    cam = None
    if status:
        (id, type) = batch.commit_output(0)
        cam = dbvalue(id, type)
        return cam
    else:
        raise VpglException("Failed to convert local rational camera")

# ;
# camera saving;
# ;


def save_rational_camera(camera, path):
    batch.init_process("vpglSaveRationalCameraProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_string(1, path)
    batch.run_process()


def save_perspective_camera(camera, path):
    batch.init_process("vpglSavePerspectiveCameraProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_string(1, path)
    batch.run_process()


def save_perspective_camera_vrml(camera, path):
    batch.init_process("vpglSavePerspectiveCameraVrmlProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_string(1, path)
    batch.set_input_float(2, 5.0)
    batch.run_process()


def save_perspective_cameras_vrml(camerafolder, path):
    batch.init_process("vpglSavePerspectiveCamerasVrmlProcess")
    batch.set_input_string(0, camerafolder)
    batch.set_input_string(1, path)
    batch.set_input_float(2, 5.0)
    batch.run_process()


def save_proj_camera(camera, path):
    batch.init_process("vpglSaveProjectiveCameraProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_string(1, path)
    batch.run_process()

# ;
# perspective go generic conversion;
# ;


def persp2gen(pcam, ni, nj, level=0):
    batch.init_process("vpglConvertToGenericCameraProcess")
    batch.set_input_from_db(0, pcam)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_unsigned(3, level)
    status = batch.run_process()
    gcam = None
    if status:
        (id, type) = batch.commit_output(0)
        gcam = dbvalue(id, type)
        return gcam
    else:
        raise VpglException("Failed to convert perspective camera to generic")


def persp2genWmargin(pcam, ni, nj, margin, level=0):
    batch.init_process("vpglConvertToGenericCameraWithMarginProcess")
    batch.set_input_from_db(0, pcam)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_unsigned(3, level)
    batch.set_input_int(4, margin)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    gcam = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    ni = batch.get_output_unsigned(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    nj = batch.get_output_unsigned(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    new_pers_cam = dbvalue(id, type)
    return (gcam, ni, nj, new_pers_cam)


def write_generic_to_vrml(cam, out_file_name, level=0):
    batch.init_process("vpglWriteGenericCameraProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_string(1, out_file_name)
    batch.set_input_unsigned(2, level)
    batch.run_process()


def get_generic_cam_ray(cam, u, v):
    batch.init_process("vpglGetGenericCamRayProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_unsigned(1, u)
    batch.set_input_unsigned(2, v)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    orig_x = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    orig_y = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    orig_z = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(3)
    dir_x = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(4)
    dir_y = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(5)
    dir_z = batch.get_output_double(id)
    batch.remove_data(id)
    return orig_x, orig_y, orig_z, dir_x, dir_y, dir_z

# gets bounding box from a directory of cameras... (incomplete)_;


def camera_dir_planar_bbox(dir_name):
    batch.init_process("vpglGetBoundingBoxProcess")
    batch.set_input_string(0, dir_name)
    batch.run_process()


def project_point(camera, x, y, z):
    batch.init_process('vpglProjectProcess')
    batch.set_input_from_db(0, camera)
    batch.set_input_double(1, x)
    batch.set_input_double(2, y)
    batch.set_input_double(3, z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    u = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    v = batch.get_output_double(id)
    batch.remove_data(id)
    return (u, v)

# gets view direction at a point for a perspective camera;


def get_view_at_point(persp_cam, x, y, z):
    batch.init_process("vpglGetViewDirectionAtPointProcess")
    batch.set_input_from_db(0, persp_cam)
    batch.set_input_float(1, x)
    batch.set_input_float(2, y)
    batch.set_input_float(3, z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    theta = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    phi = batch.get_output_float(id)
    batch.remove_data(id)
    return theta, phi


def get_xyz_from_depth_image(cam, depth_img):
    batch.init_process("vpglGenerateXYZFromDepthImageProcess")
    batch.set_input_from_db(0, cam)  # perspective or generic camera
    batch.set_input_from_db(1, depth_img)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x_img = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    y_img = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    z_img = dbvalue(id, type)

    return x_img, y_img, z_img


def get_3d_from_depth(cam, u, v, t):
    batch.init_process("vpglGenerate3dPointFromDepthProcess")
    batch.set_input_from_db(0, cam)  # perspective or generic camera
    batch.set_input_float(1, u)
    batch.set_input_float(2, v)
    batch.set_input_float(3, t)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)
    return x, y, z

# triangulates a list of cams and a list of points;


def get_3d_from_cams(cams, points):
    assert(len(cams) == len(points) and len(cams) > 1)
    # list of points will just be [u1,v1,u2,v2...];
    ptlist = []
    for p in points:
        ptlist.append(p[0])
        ptlist.append(p[1])
    # list of cam ids (type will be checked in C++);
    camlist = []
    for cam in cams:
        camlist.append(cam.id)
    batch.init_process("vpglGenerate3dPointFromCamsProcess")
    batch.set_input_unsigned_array(0, camlist)
    batch.set_input_int_array(1, ptlist)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)
    return x, y, z

# create a generic camera;


def convert_to_generic_camera(cam_in, ni, nj, level=0):
    batch.init_process('vpglConvertToGenericCameraProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_unsigned(3, level)
    status = batch.run_process()
    generic_cam = None
    if status:
        (id, type) = batch.commit_output(0)
        generic_cam = dbvalue(id, type)
        return generic_cam
    else:
        raise VpglException("Failed to convert to generic camera")

# create a generic camera from a local rational with user-specified z range;


def convert_local_rational_to_generic(cam_in, ni, nj, min_z, max_z, level=0):
    batch.init_process('vpglConvertLocalRationalToGenericProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.set_input_float(3, min_z)
    batch.set_input_float(4, max_z)
    batch.set_input_unsigned(5, level)
    if not batch.run_process():
        return None
    (id, type) = batch.commit_output(0)
    generic_cam = dbvalue(id, type)
    return generic_cam

# correct a rational camera;


def correct_rational_camera(cam_in, offset_x, offset_y):
    batch.init_process('vpglCorrectRationalCameraProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_double(1, offset_x)
    batch.set_input_double(2, offset_y)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    corrected_cam = dbvalue(id, type)
    return corrected_cam


def get_rational_camera_offsets(cam_in):
    batch.init_process('vpglGetRationalCameraOffsetsProcess')
    batch.set_input_from_db(0, cam_in)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    offset_u = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    offset_v = batch.get_output_double(id)
    batch.remove_data(id)
    return (offset_u, offset_v)


def get_correction_offset(cam_orig, cam_corrected):
    offset_u,   offset_v = get_rational_camera_offsets(cam_orig)
    offset_u_c, offset_v_c = get_rational_camera_offsets(cam_corrected)
    diff_u = offset_u_c - offset_u
    diff_v = offset_v_c - offset_v
    return diff_u, diff_v


def find_offset_and_correct_rational_camera(cam_orig, cam_corrected, cam_to_be_corrected):
    offset_u, offset_v = get_rational_camera_offsets(cam_orig)
    offset_u_c, offset_v_c = get_rational_camera_offsets(cam_corrected)
    diff_u = offset_u_c - offset_u
    diff_v = offset_v_c - offset_v
    cam_out = correct_rational_camera(cam_to_be_corrected, diff_u, diff_v)
    return cam_out

# convert lat,lon,el to local coordinates;


def convert_to_local_coordinates(lvcs_filename, lat, lon, el):
    batch.init_process('vpglConvertToLocalCoordinatesProcess')
    batch.set_input_string(0, lvcs_filename)
    batch.set_input_float(1, lat)
    batch.set_input_float(2, lon)
    batch.set_input_float(3, el)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_float(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_float(id)
    batch.remove_data(id)
    return (x, y, z)

# convert lat,lon,el to local coordinates;


def convert_to_local_coordinates2(lvcs, lat, lon, el):
    batch.init_process('vpglConvertToLocalCoordinatesProcess2')
    batch.set_input_from_db(0, lvcs)
    batch.set_input_double(1, lat)
    batch.set_input_double(2, lon)
    batch.set_input_double(3, el)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    x = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    y = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    z = batch.get_output_double(id)
    batch.remove_data(id)
    return (x, y, z)
# convert lat,lon,el to local coordinates;


def convert_local_to_global_coordinates(lvcs, x, y, z):
    batch.init_process('vpglConvertLocalToGlobalCoordinatesProcess')
    batch.set_input_from_db(0, lvcs)
    batch.set_input_double(1, x)
    batch.set_input_double(2, y)
    batch.set_input_double(3, z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lat = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    lon = batch.get_output_double(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    el = batch.get_output_double(id)
    batch.remove_data(id)
    return (lat, lon, el)
# convert lat,lon,el to local coordinates;


def convert_local_to_global_coordinates_array(lvcs, x, y, z):
    batch.init_process('vpglConvertLocalToGlobalCoordinatesArrayProcess')
    batch.set_input_from_db(0, lvcs)
    batch.set_input_double_array(1, x)
    batch.set_input_double_array(2, y)
    batch.set_input_double_array(3, z)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lat = batch.get_output_double_array(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    lon = batch.get_output_double_array(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(2)
    el = batch.get_output_double_array(id)
    batch.remove_data(id)
    return (lat, lon, el)
# convert lat,lon,el to local coordinates;


def create_lvcs(lat, lon, el, csname):
    batch.init_process('vpglCreateLVCSProcess')
    batch.set_input_double(0, lat)
    batch.set_input_double(1, lon)
    batch.set_input_double(2, el)
    batch.set_input_string(3, csname)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    lvcs = dbvalue(id, type)
    return lvcs

# randomly sample a camera rotated around principal axis;


def perturb_camera(cam_in, angle, rng):
    batch.init_process('vpglPerturbPerspCamOrientProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_float(1, angle)
    batch.set_input_from_db(2, rng)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    pert_cam = dbvalue(id, type)
    (theta_id, type) = batch.commit_output(1)
    (phi_id, type) = batch.commit_output(2)
    theta = batch.get_output_float(theta_id)
    phi = batch.get_output_float(phi_id)
    batch.remove_data(theta_id)
    batch.remove_data(phi_id)
    return pert_cam, theta, phi


def write_perspective_cam_vrml(vrml_filename, pcam, camera_rad, axis_length, r, g, b):
    batch.init_process("bvrmlWritePerspectiveCamProcess")
    batch.set_input_string(0, vrml_filename)
    batch.set_input_from_db(1, pcam)
    batch.set_input_float(2, camera_rad)
    batch.set_input_float(3, axis_length)
    batch.set_input_float(4, r)
    batch.set_input_float(5, g)
    batch.set_input_float(6, b)
    batch.run_process()

# rotate a camera around principal axis;


def rotate_perspective_camera(cam_in, theta, phi):
    batch.init_process('vpglRotatePerspCamProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_float(1, theta)
    batch.set_input_float(2, phi)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    rot_cam = dbvalue(id, type)
    return rot_cam


def get_perspective_cam_center(pcam):
    batch.init_process("vpglGetPerspectiveCamCenterProcess")
    batch.set_input_from_db(0, pcam)
    batch.run_process()
    (x_id, x_type) = batch.commit_output(0)
    x = batch.get_output_float(x_id)
    batch.remove_data(x_id)
    (y_id, type) = batch.commit_output(1)
    y = batch.get_output_float(y_id)
    batch.remove_data(y_id)
    (z_id, type) = batch.commit_output(2)
    z = batch.get_output_float(z_id)
    batch.remove_data(z_id)
    return x, y, z


def create_perspective_camera2(pcam, cent_x, cent_y, cent_z):
    batch.init_process("vpglCreatePerspectiveCameraProcess2")
    batch.set_input_from_db(0, pcam)
    batch.set_input_float(1, cent_x)
    batch.set_input_float(2, cent_y)
    batch.set_input_float(3, cent_z)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def create_perspective_camera_with_rot(pcam, phi, theta, cent_x, cent_y, cent_z):
    batch.init_process("vpglCreatePerspectiveCameraProcess3")
    batch.set_input_from_db(0, pcam)
    batch.set_input_float(1, phi)
    batch.set_input_float(2, theta)
    batch.set_input_float(3, cent_x)
    batch.set_input_float(4, cent_y)
    batch.set_input_float(5, cent_z)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def get_nitf_footprint(nitf_list_filename, out_kml_filename):
    batch.init_process('vpglNITFFootprintProcess')
    batch.set_input_string(0, nitf_list_filename)
    batch.set_input_string(1, out_kml_filename)
    batch.run_process()


def get_single_nitf_footprint(nitf_filename, out_kml_filename="", isKml=False, metafolder=""):
    batch.init_process('vpglNITFFootprintProcess2')
    batch.set_input_string(0, nitf_filename)
    batch.set_input_string(1, out_kml_filename)
    batch.set_input_bool(2, isKml)
    batch.set_input_string(3, metafolder)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    llon = batch.get_output_double(id)
    (id, type) = batch.commit_output(1)
    llat = batch.get_output_double(id)
    (id, type) = batch.commit_output(2)
    lele = batch.get_output_double(id)
    (id, type) = batch.commit_output(3)
    rlon = batch.get_output_double(id)
    (id, type) = batch.commit_output(4)
    rlat = batch.get_output_double(id)
    (id, type) = batch.commit_output(5)
    rele = batch.get_output_double(id)
    batch.remove_data(id)
    return llon, llat, lele, rlon, rlat, rele

# use a local lvcs to calculate GSD of nitf image


def calculate_nitf_gsd(rational_cam, lon1, lat1, elev1, distance=1000.0):
    # create a local lvcs
    lvcs = create_lvcs(lat1, lon1, elev1, 'wgs84')
    lat2, lon2, elev2 = convert_local_to_global_coordinates(
        lvcs, distance, distance, 0.0)
    # calculate image pixel difference
    i1, j1 = project_point(rational_cam, lon1, lat1, elev1)
    i2, j2 = project_point(rational_cam, lon2, lat2, elev2)
    gsd_i = distance / (i2 - i1)
    gsd_j = distance / (j2 - j1)
    if (gsd_i < 0.0):
        gsd_i = -1 * gsd_i
    if (gsd_j < 0.0):
        gsd_j = -1 * gsd_j
    batch.remove_data(lvcs.id)
    return gsd_i, gsd_j

def get_geocam_footprint_with_value(geocam, geotiff_filename, out_kml_filename="", write_kml=False):
    batch.init_process("vpglGeoFootprintProcess2")
    batch.set_input_from_db(0, geocam)
    batch.set_input_string(1, geotiff_filename)
    batch.set_input_string(2, out_kml_filename)
    batch.set_input_bool(3, write_kml)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        ll_lon = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        ll_lat = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        ur_lon = batch.get_output_double(id)
        (id, type) = batch.commit_output(3)
        ur_lat = batch.get_output_double(id)
        return ll_lon, ll_lat, ur_lon, ur_lat
    else:
        raise VpglException("Failed to get geographic camera footprint")


def load_geotiff_cam(tfw_filename, lvcs=0, utm_zone=0, utm_hemisphere=0):
    batch.init_process("vpglLoadGeoCameraProcess")
    batch.set_input_string(0, tfw_filename)
    if lvcs != 0:
        batch.set_input_from_db(1, lvcs)
    batch.set_input_int(2, utm_zone)
    batch.set_input_unsigned(3, utm_hemisphere)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def save_geocam_to_tfw(cam, tfw_filename):
    batch.init_process("vpglSaveGeoCameraTFWProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_string(1, tfw_filename)
    batch.run_process()


def load_geotiff_cam2(filename, ni, nj):
    batch.init_process("vpglLoadGeoCameraProcess2")
    batch.set_input_string(0, filename)
    batch.set_input_unsigned(1, ni)
    batch.set_input_unsigned(2, nj)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def load_geotiff_from_header(filename, lvcs=0):
    batch.init_process("vpglLoadGeotiffCamFromHeaderProcess")
    batch.set_input_string(0, filename)
    if lvcs != 0:
        batch.set_input_from_db(1, lvcs)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def translate_geo_camera(geocam, x, y):
    batch.init_process("vpglTranslateGeoCameraProcess")
    batch.set_input_from_db(0, geocam)
    batch.set_input_double(1, x)
    batch.set_input_double(2, y)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def create_geotiff_cam(ll_lon, ll_lat, ur_lon, ur_lat, ni, nj, lvcs=0):
    batch.init_process("vpglCreateGeoCameraProcess")
    batch.set_input_double(0, ll_lon)
    batch.set_input_double(1, ll_lat)
    batch.set_input_double(2, ur_lon)
    batch.set_input_double(3, ur_lat)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    if lvcs != 0:
        batch.set_input_from_db(6, lvcs)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        cam = dbvalue(id, type)
        return cam
    else:
        raise VpglException("Failed to create geotiff camera")


def geo2generic(geocam, ni, nj, scene_height, level):
    batch.init_process("vpglConvertGeoCameraToGenericProcess")
    batch.set_input_from_db(0, geocam)
    batch.set_input_int(1, ni)
    batch.set_input_int(2, nj)
    batch.set_input_double(3, scene_height)
    batch.set_input_int(4, level)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def geo2generic_nonnadir(geocam, ni, nj, scene_height, dir_x, dir_y, dir_z, level):
    batch.init_process("vpglConvertNonNadirGeoCameraToGenericProcess")
    batch.set_input_from_db(0, geocam)
    batch.set_input_int(1, ni)
    batch.set_input_int(2, nj)
    batch.set_input_double(3, scene_height)
    batch.set_input_int(4, level)
    batch.set_input_double(5, dir_x)
    batch.set_input_double(6, dir_y)
    batch.set_input_double(7, dir_z)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def save_lvcs(lvcs, lvcs_filename):
    batch.init_process("vpglSaveLVCSProcess")
    batch.set_input_from_db(0, lvcs)
    batch.set_input_string(1, lvcs_filename)
    return batch.run_process()


def create_and_save_lvcs(lat, lon, elev, cs_name, lvcs_filename):
    batch.init_process("vpglCreateAndSaveLVCSProcess")
    batch.set_input_float(0, lat)
    batch.set_input_float(1, lon)
    batch.set_input_float(2, elev)
    batch.set_input_string(3, cs_name)
    batch.set_input_string(4, lvcs_filename)
    return batch.run_process()


def load_lvcs(lvcs_filename):
    batch.init_process("vpglLoadLVCSProcess")
    batch.set_input_string(0, lvcs_filename)
    batch.run_process()
    (lvcs_id, lvcs_type) = batch.commit_output(0)
    lvcs = dbvalue(lvcs_id, lvcs_type)
    return lvcs


def geo_cam_global_to_img(geocam, lon, lat):
    batch.init_process("vpglGeoGlobalToImgProcess")
    batch.set_input_from_db(0, geocam)
    batch.set_input_double(1, lon)
    batch.set_input_double(2, lat)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    u = batch.get_output_int(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    v = batch.get_output_int(id)
    batch.remove_data(id)
    return u, v


def geo_cam_img_to_global(geocam, i, j):
    batch.init_process("vpglGeoImgToGlobalProcess")
    batch.set_input_from_db(0, geocam)
    batch.set_input_unsigned(1, i)
    batch.set_input_unsigned(2, j)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        lon = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        lat = batch.get_output_double(id)
        return lon, lat
    else:
        raise VpglException("Failed to convert geographic camera image to global")


def convert_perspective_to_nvm(cams_dir, imgs_dir, output_nvm):
    batch.init_process("vpglExportCamerasToNvmProcess")
    batch.set_input_string(0, cams_dir)
    batch.set_input_string(1, imgs_dir)
    batch.set_input_string(2, output_nvm)
    return batch.run_process()


def interpolate_perspective_cameras(cam0, cam1, ncams, outdir):
    batch.init_process("vpglInterpolatePerspectiveCamerasProcess")
    batch.set_input_from_db(0, cam0)
    batch.set_input_from_db(1, cam1)
    batch.set_input_unsigned(2, ncams)
    batch.set_input_string(3, outdir)
    return batch.run_process()


def compute_affine_from_local_rational(cropped_cam, min_x, min_y, min_z, max_x, max_y, max_z, n_points=100):
    batch.init_process("vpglComputeAffineFromRationalProcess")
    batch.set_input_from_db(0, cropped_cam)
    batch.set_input_double(1, min_x)
    batch.set_input_double(2, min_y)
    batch.set_input_double(3, min_z)
    batch.set_input_double(4, max_x)
    batch.set_input_double(5, max_y)
    batch.set_input_double(6, max_z)
    batch.set_input_unsigned(7, n_points)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_cam = dbvalue(id, type)
    return out_cam

# use the affine cameras of the images to compute an affine fundamental matrix and rectify them (flatten epipolar lines to scan lines and align them)
# use the 3-d box that the cameras see to compute correspondences for
# minimally distortive alignment


def affine_rectify_images(img1, affine_cam1, img2, affine_cam2, min_x, min_y, min_z, max_x, max_y, max_z, local_ground_plane_height=5, n_points=100):
    batch.init_process("vpglAffineRectifyImagesProcess")
    batch.set_input_from_db(0, img1)
    batch.set_input_from_db(1, affine_cam1)
    batch.set_input_from_db(2, img2)
    batch.set_input_from_db(3, affine_cam2)
    batch.set_input_double(4, min_x)
    batch.set_input_double(5, min_y)
    batch.set_input_double(6, min_z)
    batch.set_input_double(7, max_x)
    batch.set_input_double(8, max_y)
    batch.set_input_double(9, max_z)
    batch.set_input_unsigned(10, n_points)
    batch.set_input_double(11, local_ground_plane_height)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_cam1 = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    out_img2 = dbvalue(id, type)
    (id, type) = batch.commit_output(3)
    out_cam2 = dbvalue(id, type)
    return out_img1, out_cam1, out_img2, out_cam2

# use the affine cameras of the images to compute an affine fundamental matrix and rectify them (flatten epipolar lines to scan lines and align them)
# use the 3-d box that the cameras see to compute correspondences for
# minimally distortive alignment, use the local rational cameras to find
# the correspondence points


def affine_rectify_images2(img1, affine_cam1, local_rational_cam1, img2, affine_cam2, local_rational_cam2, min_x, min_y, min_z, max_x, max_y, max_z, output_path_H1, output_path_H2, local_ground_plane_height=5, n_points=100):
    batch.init_process("vpglAffineRectifyImagesProcess2")
    batch.set_input_from_db(0, img1)
    batch.set_input_from_db(1, affine_cam1)
    batch.set_input_from_db(2, local_rational_cam1)
    batch.set_input_from_db(3, img2)
    batch.set_input_from_db(4, affine_cam2)
    batch.set_input_from_db(5, local_rational_cam2)
    batch.set_input_double(6, min_x)
    batch.set_input_double(7, min_y)
    batch.set_input_double(8, min_z)
    batch.set_input_double(9, max_x)
    batch.set_input_double(10, max_y)
    batch.set_input_double(11, max_z)
    batch.set_input_unsigned(12, n_points)
    batch.set_input_double(13, local_ground_plane_height)
    batch.set_input_string(14, output_path_H1)
    batch.set_input_string(15, output_path_H2)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    out_cam1 = dbvalue(id, type)
    (id, type) = batch.commit_output(2)
    out_img2 = dbvalue(id, type)
    (id, type) = batch.commit_output(3)
    out_cam2 = dbvalue(id, type)
    return out_img1, out_cam1, out_img2, out_cam2


# use the affine cameras of the images to compute an affine fundamental
# matrix and write the f matrix out


def affine_f_matrix(affine_cam1, affine_cam2, output_path):
    batch.init_process("vpglAffineFMatrixProcess")
    batch.set_input_from_db(0, affine_cam1)
    batch.set_input_from_db(1, affine_cam2)
    batch.set_input_string(2, output_path)
    batch.run_process()


def construct_height_map_from_disparity(img1, img1_disp, min_disparity, local_rational_cam1, img2, local_rational_cam2,
                                        min_x, min_y, min_z, max_x, max_y, max_z, path_H1, path_H2, voxel_size):
    batch.init_process("vpglConstructHeightMapProcess")
    batch.set_input_from_db(0, img1)
    batch.set_input_from_db(1, local_rational_cam1)
    batch.set_input_string(2, img1_disp)
    batch.set_input_float(3, min_disparity)
    batch.set_input_from_db(4, img2)
    batch.set_input_from_db(5, local_rational_cam2)
    batch.set_input_double(6, min_x)
    batch.set_input_double(7, min_y)
    batch.set_input_double(8, min_z)
    batch.set_input_double(9, max_x)
    batch.set_input_double(10, max_y)
    batch.set_input_double(11, max_z)
    batch.set_input_double(12, voxel_size)
    batch.set_input_string(13, path_H1)
    batch.set_input_string(14, path_H2)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_map = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    disparity_map = dbvalue(id, type)
    return out_map, disparity_map


def compute_camera_to_world_homography(cam, plane, inverse=False):
    batch.init_process("vpglComputeImageToWorldHomographyProcess")
    batch.set_input_from_db(0, cam)
    batch.set_input_float_array(1, plane)
    batch.set_input_bool(2, inverse)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    homg2d = batch.get_bbas_1d_array_float(id)
    batch.remove_data(id)
    return homg2d

# use the 3-d box to crop an image using image camera, given certain uncertainty value in meter unit
# note that the input 3-d box is in unit of wgs84 geo coordinates


def crop_image_using_3d_box(img_res_ni, img_res_nj, camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev, uncertainty, lvcs=0):
    batch.init_process("vpglCropImgUsing3DboxProcess")
    batch.set_input_unsigned(0, img_res_ni)
    batch.set_input_unsigned(1, img_res_nj)
    batch.set_input_from_db(2, camera)
    batch.set_input_double(3, lower_left_lon)
    batch.set_input_double(4, lower_left_lat)
    batch.set_input_double(5, lower_left_elev)
    batch.set_input_double(6, upper_right_lon)
    batch.set_input_double(7, upper_right_lat)
    batch.set_input_double(8, upper_right_elev)
    batch.set_input_double(9, uncertainty)
    if lvcs:
        batch.set_input_from_db(10, lvcs)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        i0 = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(2)
        j0 = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(3)
        ni = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(4)
        nj = batch.get_output_unsigned(id)
        batch.remove_data(id)
        return status, local_cam, i0, j0, ni, nj
    else:
        raise VpglException("Failed to crop image using 3D box")


# use the 3-d box to crop an image using image camera, given certain uncertainty value in meter unit
# note that the elevation of 3-d box is obtained from DEM height map
def crop_image_using_3d_box_dem(img_res, camera, ll_lon, ll_lat, ur_lon, ur_lat, dem_folder, extra_height, uncertainty, lvcs=0):
    batch.init_process("vpglCropImgUsing3DboxDemProcess")
    batch.set_input_from_db(0, img_res)
    batch.set_input_from_db(1, camera)
    batch.set_input_double(2, ll_lon)
    batch.set_input_double(3, ll_lat)
    batch.set_input_double(4, ur_lon)
    batch.set_input_double(5, ur_lat)
    batch.set_input_string(6, dem_folder)
    batch.set_input_double(7, extra_height)
    batch.set_input_double(8, uncertainty)
    if lvcs != 0:
        batch.set_input_from_db(9, lvcs)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        i0 = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(2)
        j0 = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(3)
        ni = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(4)
        nj = batch.get_output_unsigned(id)
        return status, local_cam, i0, j0, ni, nj
    else:
        raise VpglException("Failed to crop image using 3D box DEM")

# use the 3-d box to crop an ortho image using its geo camera
# note that the input 3-d box is in unit of wgs84 geo coordinates


def crop_ortho_image_using_3d_box(img_res, camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev):
    batch.init_process("vpglCropOrthoUsing3DboxPRocess")
    batch.set_input_from_db(0, img_res)
    batch.set_input_from_db(1, camera)
    batch.set_input_double(2, lower_left_lon)
    batch.set_input_double(3, lower_left_lat)
    batch.set_input_double(4, lower_left_elev)
    batch.set_input_double(5, upper_right_lon)
    batch.set_input_double(6, upper_right_lat)
    batch.set_input_double(7, upper_right_elev)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        local_geo_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        i0 = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(2)
        j0 = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(3)
        ni = batch.get_output_unsigned(id)
        (id, type) = batch.commit_output(4)
        nj = batch.get_output_unsigned(id)
        return status, local_geo_cam, i0, j0, ni, nj
    else:
        raise VpglException("Failed to crop ortho image using 3D box")


# use the 3-d box to offset the local camera using image camera, given certain uncertainty value in meter unit
# note that the input 3-d box is in unit of wgs84 geo coordinates


def offset_cam_using_3d_box(camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev, uncertainty, lvcs=None):
    batch.init_process("vpglOffsetCamUsing3DboxProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_double(1, lower_left_lon)
    batch.set_input_double(2, lower_left_lat)
    batch.set_input_double(3, lower_left_elev)
    batch.set_input_double(4, upper_right_lon)
    batch.set_input_double(5, upper_right_lat)
    batch.set_input_double(6, upper_right_elev)
    batch.set_input_double(7, uncertainty)
    if lvcs:
        batch.set_input_from_db(8, lvcs)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        i0 = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(2)
        j0 = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(3)
        ni = batch.get_output_unsigned(id)
        batch.remove_data(id)
        (id, type) = batch.commit_output(4)
        nj = batch.get_output_unsigned(id)
        batch.remove_data(id)
        return status, local_cam, i0, j0, ni, nj
    else:
        raise VpglException("Failed to offset camera using 3D box")


# covert (lat, lon) to UTM coordinates
def utm_coords(lon, lat):
    batch.init_process("vpglComputeUTMZoneProcess")
    batch.set_input_double(0, lon)
    batch.set_input_double(1, lat)
    result = batch.run_process()
    if result:
        (id, type) = batch.commit_output(0)
        x = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        y = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        utm_zone = batch.get_output_int(id)
        (id, type) = batch.commit_output(3)
        northing = batch.get_output_int(id)
        return x, y, utm_zone, northing
    else:
        return 0.0, 0.0, 0, 0


# get the world point (wgs84) given the image point and rational camera
# Pass default initial guess point (-1.0, -1.0, -1.0) and plane height
# (-1.0) if initial is unknown at all
def rational_cam_img_to_global(camera, i, j, init_lon=-1.0, init_lat=-1.0, init_elev=-1.0, pl_elev=-1.0, error_tol=0.05):
    batch.init_process("vpglRationalImgToGlobalProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_unsigned(1, i)
    batch.set_input_unsigned(2, j)
    batch.set_input_double(3, init_lon)
    batch.set_input_double(4, init_lat)
    batch.set_input_double(5, init_elev)
    batch.set_input_double(6, pl_elev)
    batch.set_input_double(7, error_tol)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        lon = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        lat = batch.get_output_double(id)
        (id, type) = batch.commit_output(2)
        elev = batch.get_output_double(id)
        return lon, lat, elev
    else:
        raise VpglException("Failed to convert rational camera image to global")


def rational_cam_nadirness(camera, lat, lon, elev):
    batch.init_process("vpglRationalCamNadirnessProcess")
    batch.set_input_from_db(0, camera)
    batch.set_input_double(1, lat)
    batch.set_input_double(2, lon)
    batch.set_input_double(3, elev)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    val = batch.get_output_double(id)
    return val


# calculate GSD from a rational camera
def calculate_nitf_gsd(rational_cam, lon1, lat1, elev1, distance=1000):
    # create a lvcs
    lvcs = create_lvcs(lat1, lon1, elev1, "wgs84")
    lat2, lon2, elev2 = convert_local_to_global_coordinates(
        lvcs, distance, distance, 0.0)
    # calculate image pixel
    i1, j1 = project_point(rational_cam, lon1, lat1, elev1)
    i2, j2 = project_point(rational_cam, lon2, lat2, elev2)
    gsd_i = distance / (i2 - i1)
    gsd_j = distance / (j2 - j1)
    if (gsd_i < 0.0):
        gsd_i = -1 * gsd_i
    if (gsd_j < 0.0):
        gsd_j = -1 * gsd_j
    batch.remove_data(lvcs.id)
    return gsd_i, gsd_j


# geo-register series of rational cameras using their correspondence features
def isfm_rational_camera(trackfile, output_folder, pixel_radius):
    batch.init_process("vpglIsfmRationalCameraProcess")
    batch.set_input_string(0, trackfile)
    batch.set_input_string(1, output_folder)
    # pixel radius to count for inliers
    batch.set_input_float(2, pixel_radius)
    if not batch.run_process():
        return None, -1.0, -1.0
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    error = batch.get_output_float(id)
    (id, type) = batch.commit_output(2)
    inliers = batch.get_output_float(id)
    return cam, error, inliers


# geo-register a rational camera to group of geo-registered cameras
def isfm_rational_camera_seed(track_file, out_folder, dem_folder, ll_lon=0.0, ll_lat=0.0, ur_lon=0.0, ur_lat=0.0, height_diff=20.0, pixel_radius=2.0, enforce_existing=False):
    batch.init_process("vpglIsfmRationalCameraSeedProcess")
    batch.set_input_string(0, track_file)
    batch.set_input_string(1, out_folder)
    batch.set_input_string(2, dem_folder)
    batch.set_input_float(3, ll_lon)
    batch.set_input_float(4, ll_lat)
    batch.set_input_float(5, ur_lon)
    batch.set_input_float(6, ur_lat)
    batch.set_input_double(7, height_diff)
    batch.set_input_float(8, pixel_radius)
    batch.set_input_bool(9, enforce_existing)
    status = batch.run_process()
    return status


def isfm_rational_camera_with_init(track_file, dem_folder, ll_lon=0.0, ll_lat=0.0, ur_lon=0.0, ur_lat=0.0, height_diff=20.0, pixel_radius=2.0):
    batch.init_process("vpglIsfmRationalCameraWithInitialProcess")
    batch.set_input_string(0, track_file)
    batch.set_input_string(1, dem_folder)
    batch.set_input_double(2, ll_lon)
    batch.set_input_double(3, ll_lat)
    batch.set_input_double(4, ur_lon)
    batch.set_input_double(5, ur_lat)
    batch.set_input_double(6, height_diff)
    batch.set_input_float(7, pixel_radius)
    if not batch.run_process():
        return None, -1.0, -1.0
    (id, type) = batch.commit_output(0)
    cam = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    error = batch.get_output_float(id)
    (id, type) = batch.commit_output(2)
    inliers = batch.get_output_float(id)
    return cam, error, inliers


# rotate a camera around principle axis


def perspective_camera_distance(cam1, cam2):
    batch.init_process('vpglPerspCamDistanceProcess')
    batch.set_input_from_db(0, cam1)
    batch.set_input_from_db(1, cam2)
    batch.run_process()
    (dist_id, type) = batch.commit_output(0)
    dist = batch.get_output_float(dist_id)
    return dist


def correct_cam_rotation(img, pcam, exp_img, cone_half_angle, n_steps, refine=True):
    batch.init_process('icamCorrectCamRotationProcess')
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, pcam)
    batch.set_input_from_db(2, exp_img)
    batch.set_input_float(3, cone_half_angle)
    batch.set_input_unsigned(4, n_steps)
    batch.set_input_bool(5, refine)
    batch.run_process()
    (m_id, m_type) = batch.commit_output(0)
    mapped_img = dbvalue(m_id, m_type)
    (c_id, c_type) = batch.commit_output(1)
    corr_cam = dbvalue(c_id, c_type)
    return mapped_img, corr_cam


def compute_direction_covariance(pcam, std_dev_angle, out_file):
    batch.init_process("vpglComputePerspCamPACovarianceProcess")
    batch.set_input_from_db(0, pcam)
    batch.set_input_float(1, std_dev_angle)
    batch.set_input_string(2, out_file)
    batch.run_process()

# uniformly sample a camera rotated around principle axis


def perturb_camera_uniform(cam_in, angle, rng):
    batch.init_process('vpglPerturbUniformPerspCamOrientProcess')
    batch.set_input_from_db(0, cam_in)
    batch.set_input_float(1, angle)
    batch.set_input_from_db(2, rng)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    pert_cam = dbvalue(id, type)
    (theta_id, type) = batch.commit_output(1)
    (phi_id, type) = batch.commit_output(2)
    theta = batch.get_output_float(theta_id)
    phi = batch.get_output_float(phi_id)
    return pert_cam, theta, phi


def create_perspective_camera_with_motion_dir(pcam, cent_x, cent_y, cent_z, cent2_x, cent2_y, cent2_z):
    batch.init_process("vpglCreatePerspectiveCameraProcess4")
    batch.set_input_from_db(0, pcam)
    batch.set_input_float(1, cent_x)
    batch.set_input_float(2, cent_y)
    batch.set_input_float(3, cent_z)
    batch.set_input_float(4, cent2_x)
    batch.set_input_float(5, cent2_y)
    batch.set_input_float(6, cent2_z)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def pers_cam_from_photo_overlay(lvcs, heading, tilt, roll, lat, lon, alt, fov_hor, fov_ver, ni, nj):
    batch.init_process("vpglPerspCameraFromPhotoOverlayProcess")
    batch.set_input_from_db(0, lvcs)
    batch.set_input_float(1, heading)
    batch.set_input_float(2, tilt)
    batch.set_input_float(3, roll)
    batch.set_input_float(4, lat)
    batch.set_input_float(5, lon)
    batch.set_input_float(6, alt)
    batch.set_input_float(7, fov_hor)
    batch.set_input_float(8, fov_ver)
    batch.set_input_unsigned(9, ni)
    batch.set_input_unsigned(10, nj)
    batch.run_process()
    (c_id, c_type) = batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def create_perspective_camera_krt(k, r, t):
    ''' Take a k(3x3), r(3x3), and t(3) numpy array and returns a database object

      k, r, t can also be a flattened list
    '''
    if type(k) != list:
        k = k.flatten().tolist()
    if type(r) != list:
        r = r.flatten().tolist()
    if type(t) != list:
        t = t.flatten().tolist()

    batch.init_process("vpglCreatePerspectiveCameraProcess5")
    batch.set_input_double_array(0, k)
    batch.set_input_double_array(1, r)
    batch.set_input_double_array(2, t)
    batch.run_process()
    (db_id, db_type) = batch.commit_output(0)
    cam = dbvalue(db_id, db_type)
    return cam


# input two sets of points that correspond to each other in two different coordinate systems
# compute the similarity transformation that maps space of pts0 to space of pts1, the size of pts0 and pts1 better match!
# outputs a 4 by 4 similarity matrix as a vector of size 16
# construct the matrix as follows
# 0  1  2  3
# 4  5  6  7
# 8  9  10 11
# 12 13 14 15
def compute_transformation(pts0_xs, pts0_ys, pts0_zs,
                           pts1_xs, pts1_ys, pts1_zs,
                           input_cam_folder, output_cam_folder):
    batch.init_process("vpglTransformSpaceProcess")
    batch.set_input_double_array(0, pts0_xs)
    batch.set_input_double_array(1, pts0_ys)
    batch.set_input_double_array(2, pts0_zs)
    batch.set_input_double_array(3, pts1_xs)
    batch.set_input_double_array(4, pts1_ys)
    batch.set_input_double_array(5, pts1_zs)
    batch.set_input_string(6, input_cam_folder)
    batch.set_input_string(7, output_cam_folder)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    matrix_as_array = batch.get_output_double_array(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    scale = batch.get_output_double(id)
    batch.remove_data(id)
    return matrix_as_array, scale

# inputs a 4 by 4 similarity matrix as a vector of size 16
# construct the matrix as follows
# 0  1  2  3
# 4  5  6  7
# 8  9  10 11
# 12 13 14 15


def compute_transformed_box(min_pt, max_pt, matrix_as_array):
    batch.init_process("vpglTransformBoxProcess")
    batch.set_input_double_array(0, min_pt)
    batch.set_input_double_array(1, max_pt)
    batch.set_input_double_array(2, matrix_as_array)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    out_min_pt = batch.get_output_double_array(id)
    batch.remove_data(id)
    (id, type) = batch.commit_output(1)
    out_max_pt = batch.get_output_double_array(id)
    batch.remove_data(id)
    return out_min_pt, out_max_pt

# get connected component of a geotiff image


def find_connected_component(in_img, in_cam, threshold, out_kml, is_above=True):
    batch.init_process("vpglFindConnectedComponentProcess")
    batch.set_input_from_db(0, in_img)
    batch.set_input_from_db(1, in_cam)
    batch.set_input_float(2, threshold)
    batch.set_input_string(3, out_kml)
    batch.set_input_bool(4, is_above)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        out_img = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        num_regions = batch.get_output_unsigned(id)
        return out_img, num_regions
    else:
        raise VpglException("Failed to find connected components")

# rotate a image north up based on its RPC camera.  The return value is
# rotation angle between -Pi to Pi


def rational_camera_rotate_to_north(in_cam):
    batch.init_process("vpglRationalCamRotationToNorthProcess")
    batch.set_input_from_db(0, in_cam)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        ang_in_deg = batch.get_output_double(id)
        return ang_in_deg
    else:
        raise VpglException("Failed to get north angle from rational camera")

# rotate a image north up based on its RPC camera.  The return value is
# rotation angle between -Pi to Pi


def rational_camera_get_up_vector(in_cam):
    batch.init_process("vpglRationalCamRotationToUpVectorProcess")
    batch.set_input_from_db(0, in_cam)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        u = batch.get_output_double(id)
        (id, type) = batch.commit_output(1)
        v = batch.get_output_double(id)
        return u,v
    else:
        raise VpglException("Failed to get north angle from rational camera")

# create a DEM manager to support DEM utility functions
# note if zmin > zmax, these values will be calculated from the DEM
def create_DEM_manager(dem_img_resc,zmin=0.0,zmax=-1.0):
    batch.init_process("vpglCreateDemManagerProcess")
    batch.set_input_from_db(0, dem_img_resc)
    batch.set_input_double(1, zmin)
    batch.set_input_double(2, zmax)
    status = batch.run_process()
    if status:
        (id0, type) = batch.commit_output(0)
        dem_mgr = dbvalue(id0, type)
        (id1, type) = batch.commit_output(1)
        zmin = batch.get_output_double(id1)
        (id2, type) = batch.commit_output(2)
        zmax = batch.get_output_double(id2)
        return (dem_mgr,zmin,zmax)
    raise VpglException("failed to create a DEM manager")

# Backproject an image point onto the DEM and return a 3-d point
def DEM_backproj(dem_mgr, cam, u, v, err_tol = 1.0):
    batch.init_process("vpglBackprojectDemProcess")
    batch.set_input_from_db(0, dem_mgr)
    batch.set_input_from_db(1, cam)
    batch.set_input_double(2, u)
    batch.set_input_double(3, v)
    batch.set_input_double(4, err_tol)
    status = batch.run_process()
    if status:
        (id0, type) = batch.commit_output(0)
        x = batch.get_output_double(id0)
        (id1, type) = batch.commit_output(1)
        y = batch.get_output_double(id1)
        (id2, type) = batch.commit_output(2)
        z = batch.get_output_double(id2)
        batch.remove_data(id0)
        batch.remove_data(id1)
        batch.remove_data(id2)
        return (x, y, z)
    raise VpglException("failed to backproject onto the DEM")

# convert a geotiff image to a ASCII xyz point cloud file via a LVCS conversion
def dem_to_pts_lvcs(img, cam, lvcs, out_file, is_convert_z = True):
    batch.init_process("vpglConvertGeotiffToPointCloudProcess")
    batch.set_input_from_db(0, img)
    batch.set_input_from_db(1, cam)
    batch.set_input_from_db(2, lvcs)
    batch.set_input_string(3, out_file)
    batch.set_input_bool(4, is_convert_z)
    status = batch.run_process()
    if status:
        (id, type) = batch.commit_output(0)
        n_pts = batch.get_output_unsigned(id)
        return n_pts
    raise VpglException("failed to convert geotiff to point clouds")

# Project a reference image that controled by a rational camera onto target image domain using DEM
def DEM_project_img(dem_mgr, ref_img, ref_cam, tgr_cam, tgr_i0, tgr_j0, tgr_ni, tgr_nj, err_tol = 1.0):
    batch.init_process("vpglDemImageProjectionProcess")
    batch.set_input_from_db(0, ref_img)
    batch.set_input_from_db(1, ref_cam)
    batch.set_input_from_db(2, dem_mgr)
    batch.set_input_from_db(3, tgr_cam)
    batch.set_input_unsigned(4, tgr_i0)
    batch.set_input_unsigned(5, tgr_j0)
    batch.set_input_unsigned(6, tgr_ni)
    batch.set_input_unsigned(7, tgr_nj)
    batch.set_input_double(8, err_tol)
    status = batch.run_process()
    if status:
        (id0, type0) = batch.commit_output(0)
        out_img = dbvalue(id0, type0)
        (id1, type1) = batch.commit_output(1)
        err_cnt = batch.get_output_unsigned(id1)
        return out_img, err_cnt
    raise VpglException("failed to project reference image to target image using DEM manager")
