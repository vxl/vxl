from boxm2_register import boxm2_batch, dbvalue
#import math, numpy;
import math

# ;
# camera loading;
# ;


def load_perspective_camera(file_path):
    boxm2_batch.init_process("vpglLoadPerspectiveCameraProcess")
    boxm2_batch.set_input_string(0, file_path)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_affine_camera(file_path, viewing_dist=1000, look_dir=(0, 0, -1)):
    """ load an affine camera from a text file containing the projection matrix
        The viewing rays will be flipped such that the dot product with look_dir is positive
    """
    boxm2_batch.init_process("vpglLoadAffineCameraProcess")
    boxm2_batch.set_input_string(0, file_path)
    boxm2_batch.set_input_double(1, viewing_dist)
    boxm2_batch.set_input_double(2, look_dir[0])
    boxm2_batch.set_input_double(3, look_dir[1])
    boxm2_batch.set_input_double(4, look_dir[2])
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_projective_camera(file_path):
    boxm2_batch.init_process("vpglLoadProjCameraProcess")
    boxm2_batch.set_input_string(0, file_path)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam
#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z), up = (x,y,z);


def create_perspective_camera(scale, ppoint, center, look_pt, up=[0, 1, 0]):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess")
    boxm2_batch.set_input_double(0, scale[0])
    boxm2_batch.set_input_double(1, ppoint[0])
    boxm2_batch.set_input_double(2, scale[1])
    boxm2_batch.set_input_double(3, ppoint[1])
    boxm2_batch.set_input_double(4, center[0])
    boxm2_batch.set_input_double(5, center[1])
    boxm2_batch.set_input_double(6, center[2])
    boxm2_batch.set_input_double(7, look_pt[0])
    boxm2_batch.set_input_double(8, look_pt[1])
    boxm2_batch.set_input_double(9, look_pt[2])
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def create_perspective_camera_from_kml(ni, nj, right_fov, top_fov, altitude, heading, tilt, roll, cent_x, cent_y):
    boxm2_batch.init_process("vpglCreatePerspCameraFromKMLProcess")
    boxm2_batch.set_input_unsigned(0, ni)
    boxm2_batch.set_input_unsigned(1, nj)
    boxm2_batch.set_input_double(2, right_fov)
    boxm2_batch.set_input_double(3, top_fov)
    boxm2_batch.set_input_double(4, altitude)
    boxm2_batch.set_input_double(5, heading)
    boxm2_batch.set_input_double(6, tilt)
    boxm2_batch.set_input_double(7, roll)
    boxm2_batch.set_input_double(8, cent_x)
    boxm2_batch.set_input_double(9, cent_y)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_perspective_camera_from_kml_file(NI, NJ, kml_file):
    boxm2_batch.init_process("vpglLoadPerspCameraFromKMLFileProcess")
    boxm2_batch.set_input_unsigned(0, NI)
    boxm2_batch.set_input_unsigned(1, NJ)
    boxm2_batch.set_input_string(2, kml_file)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(1)
    longitude = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    latitude = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(3)
    altitude = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    return cam, longitude, latitude, altitude

# resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);


def resample_perspective_camera(cam, size0, size1):
    boxm2_batch.init_process("vpglResamplePerspectiveCameraProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_int(1, size0[0])
    boxm2_batch.set_input_int(2, size0[1])
    boxm2_batch.set_input_int(3, size1[0])
    boxm2_batch.set_input_int(4, size1[1])
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    out = dbvalue(id, type)
    return out
# resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);


def get_perspective_camera_center(cam):
    boxm2_batch.init_process("vpglGetPerspectiveCamCenterProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return x, y, z


def get_backprojected_ray(cam, u, v):
    boxm2_batch.init_process("vpglGetBackprojectRayProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_float(1, u)
    boxm2_batch.set_input_float(2, v)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return x, y, z
# returns cartesian cam center from azimuth (degrees), elevation
# (degrees), radius, look point;


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
    boxm2_batch.init_process("vpglLoadRationalCameraProcess")
    boxm2_batch.set_input_string(0, file_path)
    status = boxm2_batch.run_process()
    cam = None
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        cam = dbvalue(id, type)
    return status, cam


def load_rational_camera_nitf(file_path):
    boxm2_batch.init_process("vpglLoadRationalCameraNITFProcess")
    boxm2_batch.set_input_string(0, file_path)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def load_local_rational_camera(file_path):
    boxm2_batch.init_process("vpglLoadLocalRationalCameraProcess")
    boxm2_batch.set_input_string(0, file_path)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def convert_local_rational_perspective_camera(local_cam):
    boxm2_batch.init_process("vpglConvertLocalRationalToPerspectiveProcess")
    boxm2_batch.set_input_from_db(0, local_cam)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def create_local_rational_camera(rational_cam_fname, lvcs_fname):
    boxm2_batch.init_process('vpglCreateLocalRationalCameraProcess')
    boxm2_batch.set_input_string(0, rational_cam_fname)
    boxm2_batch.set_input_string(1, lvcs_fname)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id, type)
    return cam


def convert_to_local_rational_camera(rational_cam, lvcs):
    boxm2_batch.init_process('vpglConvertToLocalRationalCameraProcess')
    boxm2_batch.set_input_from_db(0, rational_cam)
    boxm2_batch.set_input_from_db(1, lvcs)
    status = boxm2_batch.run_process()
    cam = None
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        cam = dbvalue(id, type)
    return cam

# ;
# camera saving;
# ;


def save_rational_camera(camera, path):
    boxm2_batch.init_process("vpglSaveRationalCameraProcess")
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_string(1, path)
    boxm2_batch.run_process()


def save_perspective_camera(camera, path):
    boxm2_batch.init_process("vpglSavePerspectiveCameraProcess")
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_string(1, path)
    boxm2_batch.run_process()


def save_perspective_camera_vrml(camera, path):
    boxm2_batch.init_process("vpglSavePerspectiveCameraVrmlProcess")
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_string(1, path)
    boxm2_batch.set_input_float(2, 5.0)
    boxm2_batch.run_process()


def save_perspective_cameras_vrml(camerafolder, path):
    boxm2_batch.init_process("vpglSavePerspectiveCamerasVrmlProcess")
    boxm2_batch.set_input_string(0, camerafolder)
    boxm2_batch.set_input_string(1, path)
    boxm2_batch.set_input_float(2, 5.0)
    boxm2_batch.run_process()


def save_proj_camera(camera, path):
    boxm2_batch.init_process("vpglSaveProjectiveCameraProcess")
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_string(1, path)
    boxm2_batch.run_process()

# ;
# perspective go generic conversion;
# ;


def persp2gen(pcam, ni, nj, level=0):
    boxm2_batch.init_process("vpglConvertToGenericCameraProcess")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_unsigned(1, ni)
    boxm2_batch.set_input_unsigned(2, nj)
    boxm2_batch.set_input_unsigned(3, level)
    status = boxm2_batch.run_process()
    gcam = None
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        gcam = dbvalue(id, type)
    return gcam


def persp2genWmargin(pcam, ni, nj, margin, level=0):
    boxm2_batch.init_process("vpglConvertToGenericCameraWithMarginProcess")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_unsigned(1, ni)
    boxm2_batch.set_input_unsigned(2, nj)
    boxm2_batch.set_input_unsigned(3, level)
    boxm2_batch.set_input_int(4, margin)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    gcam = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(1)
    ni = boxm2_batch.get_output_unsigned(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    nj = boxm2_batch.get_output_unsigned(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(3)
    new_pers_cam = dbvalue(id, type)
    return (gcam, ni, nj, new_pers_cam)


def write_generic_to_vrml(cam, out_file_name, level=0):
    boxm2_batch.init_process("vpglWriteGenericCameraProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_string(1, out_file_name)
    boxm2_batch.set_input_unsigned(2, level)
    boxm2_batch.run_process()


def get_generic_cam_ray(cam, u, v):
    boxm2_batch.init_process("vpglGetGenericCamRayProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_unsigned(1, u)
    boxm2_batch.set_input_unsigned(2, v)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    orig_x = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    orig_y = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    orig_z = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(3)
    dir_x = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(4)
    dir_y = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(5)
    dir_z = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    return orig_x, orig_y, orig_z, dir_x, dir_y, dir_z

# gets bounding box from a directory of cameras... (incomplete)_;


def camera_dir_planar_bbox(dir_name):
    boxm2_batch.init_process("vpglGetBoundingBoxProcess")
    boxm2_batch.set_input_string(0, dir_name)
    boxm2_batch.run_process()


def project_point(camera, x, y, z):
    boxm2_batch.init_process('vpglProjectProcess')
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_float(1, x)
    boxm2_batch.set_input_float(2, y)
    boxm2_batch.set_input_float(3, z)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    u = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    v = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return (u, v)

# gets view direction at a point for a perspective camera;


def get_view_at_point(persp_cam, x, y, z):
    boxm2_batch.init_process("vpglGetViewDirectionAtPointProcess")
    boxm2_batch.set_input_from_db(0, persp_cam)
    boxm2_batch.set_input_float(1, x)
    boxm2_batch.set_input_float(2, y)
    boxm2_batch.set_input_float(3, z)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    theta = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    phi = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return theta, phi


def get_xyz_from_depth_image(cam, depth_img):
    boxm2_batch.init_process("vpglGenerateXYZFromDepthImageProcess")
    boxm2_batch.set_input_from_db(0, cam)  # perspective or generic camera
    boxm2_batch.set_input_from_db(1, depth_img)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x_img = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(1)
    y_img = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(2)
    z_img = dbvalue(id, type)

    return x_img, y_img, z_img


def get_3d_from_depth(cam, u, v, t):
    boxm2_batch.init_process("vpglGenerate3dPointFromDepthProcess")
    boxm2_batch.set_input_from_db(0, cam)  # perspective or generic camera
    boxm2_batch.set_input_float(1, u)
    boxm2_batch.set_input_float(2, v)
    boxm2_batch.set_input_float(3, t)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
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
    boxm2_batch.init_process("vpglGenerate3dPointFromCamsProcess")
    boxm2_batch.set_input_unsigned_array(0, camlist)
    boxm2_batch.set_input_int_array(1, ptlist)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return x, y, z

# create a generic camera;


def convert_to_generic_camera(cam_in, ni, nj, level=0):
    boxm2_batch.init_process('vpglConvertToGenericCameraProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_unsigned(1, ni)
    boxm2_batch.set_input_unsigned(2, nj)
    boxm2_batch.set_input_unsigned(3, level)
    status = boxm2_batch.run_process()
    generic_cam = None
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        generic_cam = dbvalue(id, type)
    return generic_cam

# create a generic camera from a local rational with user-specified z range;


def convert_local_rational_to_generic(cam_in, ni, nj, min_z, max_z, level=0):
    boxm2_batch.init_process('vpglConvertLocalRationalToGenericProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_unsigned(1, ni)
    boxm2_batch.set_input_unsigned(2, nj)
    boxm2_batch.set_input_float(3, min_z)
    boxm2_batch.set_input_float(4, max_z)
    boxm2_batch.set_input_unsigned(5, level)
    if not boxm2_batch.run_process():
        return None
    (id, type) = boxm2_batch.commit_output(0)
    generic_cam = dbvalue(id, type)
    return generic_cam

# correct a rational camera;


def correct_rational_camera(cam_in, offset_x, offset_y):
    boxm2_batch.init_process('vpglCorrectRationalCameraProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_double(1, offset_x)
    boxm2_batch.set_input_double(2, offset_y)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    corrected_cam = dbvalue(id, type)
    return corrected_cam


def get_rational_camera_offsets(cam_in):
    boxm2_batch.init_process('vpglGetRationalCameraOffsetsProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    offset_u = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    offset_v = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
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
    boxm2_batch.init_process('vpglConvertToLocalCoordinatesProcess')
    boxm2_batch.set_input_string(0, lvcs_filename)
    boxm2_batch.set_input_float(1, lat)
    boxm2_batch.set_input_float(2, lon)
    boxm2_batch.set_input_float(3, el)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return (x, y, z)

# convert lat,lon,el to local coordinates;


def convert_to_local_coordinates2(lvcs, lat, lon, el):
    boxm2_batch.init_process('vpglConvertToLocalCoordinatesProcess2')
    boxm2_batch.set_input_from_db(0, lvcs)
    boxm2_batch.set_input_float(1, lat)
    boxm2_batch.set_input_float(2, lon)
    boxm2_batch.set_input_float(3, el)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return (x, y, z)
# convert lat,lon,el to local coordinates;


def convert_local_to_global_coordinates(lvcs, x, y, z):
    boxm2_batch.init_process('vpglConvertLocalToGlobalCoordinatesProcess')
    boxm2_batch.set_input_from_db(0, lvcs)
    boxm2_batch.set_input_double(1, x)
    boxm2_batch.set_input_double(2, y)
    boxm2_batch.set_input_double(3, z)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    lat = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    lon = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    el = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    return (lat, lon, el)
# convert lat,lon,el to local coordinates;


def convert_local_to_global_coordinates_array(lvcs, x, y, z):
    boxm2_batch.init_process('vpglConvertLocalToGlobalCoordinatesArrayProcess')
    boxm2_batch.set_input_from_db(0, lvcs)
    boxm2_batch.set_input_double_array(1, x)
    boxm2_batch.set_input_double_array(2, y)
    boxm2_batch.set_input_double_array(3, z)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    lat = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    lon = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(2)
    el = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    return (lat, lon, el)
# convert lat,lon,el to local coordinates;


def create_lvcs(lat, lon, el, csname):
    boxm2_batch.init_process('vpglCreateLVCSProcess')
    boxm2_batch.set_input_double(0, lat)
    boxm2_batch.set_input_double(1, lon)
    boxm2_batch.set_input_double(2, el)
    boxm2_batch.set_input_string(3, csname)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    lvcs = dbvalue(id, type)
    return lvcs

# randomly sample a camera rotated around principle axis;


def perturb_camera(cam_in, angle, rng):
    boxm2_batch.init_process('vpglPerturbPerspCamOrientProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_float(1, angle)
    boxm2_batch.set_input_from_db(2, rng)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    pert_cam = dbvalue(id, type)
    (theta_id, type) = boxm2_batch.commit_output(1)
    (phi_id, type) = boxm2_batch.commit_output(2)
    theta = boxm2_batch.get_output_float(theta_id)
    phi = boxm2_batch.get_output_float(phi_id)
    boxm2_batch.remove_data(theta_id)
    boxm2_batch.remove_data(phi_id)
    return pert_cam, theta, phi


def write_perspective_cam_vrml(vrml_filename, pcam, camera_rad, axis_length, r, g, b):
    boxm2_batch.init_process("bvrmlWritePerspectiveCamProcess")
    boxm2_batch.set_input_string(0, vrml_filename)
    boxm2_batch.set_input_from_db(1, pcam)
    boxm2_batch.set_input_float(2, camera_rad)
    boxm2_batch.set_input_float(3, axis_length)
    boxm2_batch.set_input_float(4, r)
    boxm2_batch.set_input_float(5, g)
    boxm2_batch.set_input_float(6, b)
    boxm2_batch.run_process()

# rotate a camera around principle axis;


def rotate_perspective_camera(cam_in, theta, phi):
    boxm2_batch.init_process('vpglRotatePerspCamProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_float(1, theta)
    boxm2_batch.set_input_float(2, phi)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    rot_cam = dbvalue(id, type)
    return rot_cam


def get_perspective_cam_center(pcam):
    boxm2_batch.init_process("vpglGetPerspectiveCamCenterProcess")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.run_process()
    (x_id, x_type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(x_id)
    boxm2_batch.remove_data(x_id)
    (y_id, type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(y_id)
    boxm2_batch.remove_data(y_id)
    (z_id, type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(z_id)
    boxm2_batch.remove_data(z_id)
    return x, y, z


def create_perspective_camera2(pcam, cent_x, cent_y, cent_z):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess2")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_float(1, cent_x)
    boxm2_batch.set_input_float(2, cent_y)
    boxm2_batch.set_input_float(3, cent_z)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def create_perspective_camera_with_rot(pcam, phi, theta, cent_x, cent_y, cent_z):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess3")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_float(1, phi)
    boxm2_batch.set_input_float(2, theta)
    boxm2_batch.set_input_float(3, cent_x)
    boxm2_batch.set_input_float(4, cent_y)
    boxm2_batch.set_input_float(5, cent_z)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def get_nitf_footprint(nitf_list_filename, out_kml_filename):
    boxm2_batch.init_process('vpglNITFFootprintProcess')
    boxm2_batch.set_input_string(0, nitf_list_filename)
    boxm2_batch.set_input_string(1, out_kml_filename)
    boxm2_batch.run_process()


def get_single_nitf_footprint(nitf_filename, out_kml_filename="", isKml=False, metafolder=""):
    boxm2_batch.init_process('vpglNITFFootprintProcess2')
    boxm2_batch.set_input_string(0, nitf_filename)
    boxm2_batch.set_input_string(1, out_kml_filename)
    boxm2_batch.set_input_bool(2, isKml)
    boxm2_batch.set_input_string(3, metafolder)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    llon = boxm2_batch.get_output_double(id)
    (id, type) = boxm2_batch.commit_output(1)
    llat = boxm2_batch.get_output_double(id)
    (id, type) = boxm2_batch.commit_output(2)
    lele = boxm2_batch.get_output_double(id)
    (id, type) = boxm2_batch.commit_output(3)
    rlon = boxm2_batch.get_output_double(id)
    (id, type) = boxm2_batch.commit_output(4)
    rlat = boxm2_batch.get_output_double(id)
    (id, type) = boxm2_batch.commit_output(5)
    rele = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
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
    boxm2_batch.remove_data(lvcs.id)
    return gsd_i, gsd_j


def get_geocam_footprint(geocam, geotiff_filename, out_kml_filename, init_finish=True):
    boxm2_batch.init_process('vpglGeoFootprintProcess')
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_string(1, geotiff_filename)
    boxm2_batch.set_input_string(2, out_kml_filename)
    boxm2_batch.set_input_bool(3, init_finish)
    boxm2_batch.run_process()


def get_geocam_footprint(geocam, geotiff_filename, out_kml_filename="", write_kml=False):
    boxm2_batch.init_process("vpglGeoFootprintProcess2")
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_string(1, geotiff_filename)
    boxm2_batch.set_input_string(2, out_kml_filename)
    boxm2_batch.set_input_string(3, write_kml)
    status = boxm2_batch.run_process()
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        ll_lon = boxm2_batch.get_output_double(id)
        (id, type) = boxm2_batch.commit_output(1)
        ll_lat = boxm2_batch.get_output_double(id)
        (id, type) = boxm2_batch.commit_output(2)
        ur_lon = boxm2_batch.get_output_double(id)
        (id, type) = boxm2_batch.commit_output(3)
        ur_lat = boxm2_batch.get_output_double(id)
        return ll_lon, ll_lat, ur_lon, ur_lat
    else:
        return 0.0, 0.0, 0.0, 0.0


def load_geotiff_cam(tfw_filename, lvcs=0, utm_zone=0, utm_hemisphere=0):
    boxm2_batch.init_process("vpglLoadGeoCameraProcess")
    boxm2_batch.set_input_string(0, tfw_filename)
    if lvcs != 0:
        boxm2_batch.set_input_from_db(1, lvcs)
    boxm2_batch.set_input_int(2, utm_zone)
    boxm2_batch.set_input_unsigned(3, utm_hemisphere)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def save_geocam_to_tfw(cam, tfw_filename):
    boxm2_batch.init_process("vpglSaveGeoCameraTFWProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_string(1, tfw_filename)
    boxm2_batch.run_process()


def load_geotiff_cam2(filename, ni, nj):
    boxm2_batch.init_process("vpglLoadGeoCameraProcess2")
    boxm2_batch.set_input_string(0, filename)
    boxm2_batch.set_input_unsigned(1, ni)
    boxm2_batch.set_input_unsigned(2, nj)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def load_geotiff_from_header(filename, lvcs=0):
    boxm2_batch.init_process("vpglLoadGeotiffCamFromHeaderProcess")
    boxm2_batch.set_input_string(0, filename)
    if lvcs != 0:
        boxm2_batch.set_input_from_db(1, lvcs)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def translate_geo_camera(geocam, x, y):
    boxm2_batch.init_process("vpglTranslateGeoCameraProcess")
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_double(1, x)
    boxm2_batch.set_input_double(2, y)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def geo2generic(geocam, ni, nj, scene_height, level):
    boxm2_batch.init_process("vpglConvertGeoCameraToGenericProcess")
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_int(1, ni)
    boxm2_batch.set_input_int(2, nj)
    boxm2_batch.set_input_double(3, scene_height)
    boxm2_batch.set_input_int(4, level)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def geo2generic_nonnadir(geocam, ni, nj, scene_height, dir_x, dir_y, dir_z, level):
    boxm2_batch.init_process("vpglConvertNonNadirGeoCameraToGenericProcess")
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_int(1, ni)
    boxm2_batch.set_input_int(2, nj)
    boxm2_batch.set_input_double(3, scene_height)
    boxm2_batch.set_input_int(4, level)
    boxm2_batch.set_input_double(5, dir_x)
    boxm2_batch.set_input_double(6, dir_y)
    boxm2_batch.set_input_double(7, dir_z)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def save_lvcs(lvcs, lvcs_filename):
    boxm2_batch.init_process("vpglSaveLVCSProcess")
    boxm2_batch.set_input_from_db(0, lvcs)
    boxm2_batch.set_input_string(1, lvcs_filename)
    return boxm2_batch.run_process()


def create_and_save_lvcs(lat, lon, elev, cs_name, lvcs_filename):
    boxm2_batch.init_process("vpglCreateAndSaveLVCSProcess")
    boxm2_batch.set_input_float(0, lat)
    boxm2_batch.set_input_float(1, lon)
    boxm2_batch.set_input_float(2, elev)
    boxm2_batch.set_input_string(3, cs_name)
    boxm2_batch.set_input_string(4, lvcs_filename)
    return bvxm_batch.run_process()


def load_lvcs(lvcs_filename):
    boxm2_batch.init_process("vpglLoadLVCSProcess")
    boxm2_batch.set_input_string(0, lvcs_filename)
    boxm2_batch.run_process()
    (lvcs_id, lvcs_type) = boxm2_batch.commit_output(0)
    lvcs = dbvalue(lvcs_id, lvcs_type)
    return lvcs


def geo_cam_global_to_img(geocam, lon, lat):
    boxm2_batch.init_process("vpglGeoGlobalToImgProcess")
    boxm2_batch.set_input_from_db(0, geocam)
    boxm2_batch.set_input_double(1, lon)
    boxm2_batch.set_input_double(2, lat)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    u = boxm2_batch.get_output_int(id)
    boxm2_batch.remove_data(id)
    (id, type) = boxm2_batch.commit_output(1)
    v = boxm2_batch.get_output_int(id)
    boxm2_batch.remove_data(id)
    return u, v


def convert_perspective_to_nvm(cams_dir, imgs_dir, output_nvm):
    boxm2_batch.init_process("vpglExportCamerasToNvmProcess")
    boxm2_batch.set_input_string(0, cams_dir)
    boxm2_batch.set_input_string(1, imgs_dir)
    boxm2_batch.set_input_string(2, output_nvm)
    return boxm2_batch.run_process()


def interpolate_perspective_cameras(cam0, cam1, ncams, outdir):
    boxm2_batch.init_process("vpglInterpolatePerspectiveCamerasProcess")
    boxm2_batch.set_input_from_db(0, cam0)
    boxm2_batch.set_input_from_db(1, cam1)
    boxm2_batch.set_input_unsigned(2, ncams)
    boxm2_batch.set_input_string(3, outdir)
    return boxm2_batch.run_process()


def compute_affine_from_local_rational(cropped_cam, min_x, min_y, min_z, max_x, max_y, max_z, n_points=100):
    boxm2_batch.init_process("vpglComputeAffineFromRationalProcess")
    boxm2_batch.set_input_from_db(0, cropped_cam)
    boxm2_batch.set_input_double(1, min_x)
    boxm2_batch.set_input_double(2, min_y)
    boxm2_batch.set_input_double(3, min_z)
    boxm2_batch.set_input_double(4, max_x)
    boxm2_batch.set_input_double(5, max_y)
    boxm2_batch.set_input_double(6, max_z)
    boxm2_batch.set_input_unsigned(7, n_points)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    out_cam = dbvalue(id, type)
    return out_cam

# use the affine cameras of the images to compute an affine fundamental matrix and rectify them (flatten epipolar lines to scan lines and align them)
# use the 3-d box that the cameras see to compute correspondences for
# minimally distortive alignment


def affine_rectify_images(img1, affine_cam1, img2, affine_cam2, min_x, min_y, min_z, max_x, max_y, max_z, n_points=100):
    boxm2_batch.init_process("vpglAffineRectifyImagesProcess")
    boxm2_batch.set_input_from_db(0, img1)
    boxm2_batch.set_input_from_db(1, affine_cam1)
    boxm2_batch.set_input_from_db(2, img2)
    boxm2_batch.set_input_from_db(3, affine_cam2)
    boxm2_batch.set_input_double(4, min_x)
    boxm2_batch.set_input_double(5, min_y)
    boxm2_batch.set_input_double(6, min_z)
    boxm2_batch.set_input_double(7, max_x)
    boxm2_batch.set_input_double(8, max_y)
    boxm2_batch.set_input_double(9, max_z)
    boxm2_batch.set_input_unsigned(10, n_points)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    out_img1 = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(1)
    out_cam1 = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(2)
    out_img2 = dbvalue(id, type)
    (id, type) = boxm2_batch.commit_output(3)
    out_cam2 = dbvalue(id, type)
    return out_img1, out_cam1, out_img2, out_cam2


def compute_camera_to_world_homography(cam, plane, inverse=False):
    boxm2_batch.init_process("vpglComputeImageToWorldHomographyProcess")
    boxm2_batch.set_input_from_db(0, cam)
    boxm2_batch.set_input_float_array(1, plane)
    boxm2_batch.set_input_bool(2, inverse)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    homg2d = boxm2_batch.get_bbas_1d_array_float(id)
    boxm2_batch.remove_data(id)
    return homg2d

# use the 3-d box to crop an image using image camera, given certain uncertainty value in meter unit
# note that the input 3-d box is in unit of wgs84 geo coordinates


def crop_image_using_3d_box(img_res, camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev, uncertainty, lvcs=0):
    boxm2_batch.init_process("vpglCropImgUsing3DboxProcess")
    boxm2_batch.set_input_from_db(0, img_res)
    boxm2_batch.set_input_from_db(1, camera)
    boxm2_batch.set_input_double(2, lower_left_lon)
    boxm2_batch.set_input_double(3, lower_left_lat)
    boxm2_batch.set_input_double(4, lower_left_elev)
    boxm2_batch.set_input_double(5, upper_right_lon)
    boxm2_batch.set_input_double(6, upper_right_lat)
    boxm2_batch.set_input_double(7, upper_right_elev)
    boxm2_batch.set_input_double(8, uncertainty)
    if lvcs:
        boxm2_batch.set_input_from_db(9, lvcs)
    status = boxm2_batch.run_process()
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = boxm2_batch.commit_output(1)
        i0 = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(2)
        j0 = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(3)
        ni = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(4)
        nj = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        return status, local_cam, i0, j0, ni, nj
    else:
        return status, dbvalue(0, ""), 0, 0, 0, 0
# use the 3-d box to offset the local camera using image camera, given certain uncertainty value in meter unit
# note that the input 3-d box is in unit of wgs84 geo coordinates


def offset_cam_using_3d_box(camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev, uncertainty, lvcs=None):
    boxm2_batch.init_process("vpglOffsetCamUsing3DboxProcess")
    boxm2_batch.set_input_from_db(0, camera)
    boxm2_batch.set_input_double(1, lower_left_lon)
    boxm2_batch.set_input_double(2, lower_left_lat)
    boxm2_batch.set_input_double(3, lower_left_elev)
    boxm2_batch.set_input_double(4, upper_right_lon)
    boxm2_batch.set_input_double(5, upper_right_lat)
    boxm2_batch.set_input_double(6, upper_right_elev)
    boxm2_batch.set_input_double(7, uncertainty)
    if lvcs:
        boxm2_batch.set_input_from_db(8, lvcs)
    status = boxm2_batch.run_process()
    if status:
        (id, type) = boxm2_batch.commit_output(0)
        local_cam = dbvalue(id, type)
        (id, type) = boxm2_batch.commit_output(1)
        i0 = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(2)
        j0 = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(3)
        ni = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        (id, type) = boxm2_batch.commit_output(4)
        nj = boxm2_batch.get_output_unsigned(id)
        boxm2_batch.remove_data(id)
        return status, local_cam, i0, j0, ni, nj
    else:
        return status, dbvalue(0, ""), 0, 0, 0, 0

# rotate a camera around principle axis


def perspective_camera_distance(cam1, cam2):
    boxm2_batch.init_process('vpglPerspCamDistanceProcess')
    boxm2_batch.set_input_from_db(0, cam1)
    boxm2_batch.set_input_from_db(1, cam2)
    boxm2_batch.run_process()
    (dist_id, type) = boxm2_batch.commit_output(0)
    dist = boxm2_batch.get_output_float(dist_id)
    return dist


def correct_cam_rotation(img, pcam, exp_img, cone_half_angle, n_steps, refine=True):
    boxm2_batch.init_process('icamCorrectCamRotationProcess')
    boxm2_batch.set_input_from_db(0, img)
    boxm2_batch.set_input_from_db(1, pcam)
    boxm2_batch.set_input_from_db(2, exp_img)
    boxm2_batch.set_input_float(3, cone_half_angle)
    boxm2_batch.set_input_unsigned(4, n_steps)
    boxm2_batch.set_input_bool(5, refine)
    boxm2_batch.run_process()
    (m_id, m_type) = boxm2_batch.commit_output(0)
    mapped_img = dbvalue(m_id, m_type)
    (c_id, c_type) = boxm2_batch.commit_output(1)
    corr_cam = dbvalue(c_id, c_type)
    return mapped_img, corr_cam


def compute_direction_covariance(pcam, std_dev_angle, out_file):
    boxm2_batch.init_process("vpglComputePerspCamPACovarianceProcess")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_float(1, std_dev_angle)
    boxm2_batch.set_input_string(2, out_file)
    boxm2_batch.run_process()

# uniformly sample a camera rotated around principle axis


def perturb_camera_uniform(cam_in, angle, rng):
    boxm2_batch.init_process('vpglPerturbUniformPerspCamOrientProcess')
    boxm2_batch.set_input_from_db(0, cam_in)
    boxm2_batch.set_input_float(1, angle)
    boxm2_batch.set_input_from_db(2, rng)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    pert_cam = dbvalue(id, type)
    (theta_id, type) = boxm2_batch.commit_output(1)
    (phi_id, type) = boxm2_batch.commit_output(2)
    theta = boxm2_batch.get_output_float(theta_id)
    phi = boxm2_batch.get_output_float(phi_id)
    return pert_cam, theta, phi


def create_perspective_camera_with_motion_dir(pcam, cent_x, cent_y, cent_z, cent2_x, cent2_y, cent2_z):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess4")
    boxm2_batch.set_input_from_db(0, pcam)
    boxm2_batch.set_input_float(1, cent_x)
    boxm2_batch.set_input_float(2, cent_y)
    boxm2_batch.set_input_float(3, cent_z)
    boxm2_batch.set_input_float(4, cent2_x)
    boxm2_batch.set_input_float(5, cent2_y)
    boxm2_batch.set_input_float(6, cent2_z)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id, c_type)
    return cam


def pers_cam_from_photo_overlay(lvcs, heading, tilt, roll, lat, lon, alt, fov_hor, fov_ver, ni, nj):
    boxm2_batch.init_process("vpglPerspCameraFromPhotoOverlayProcess")
    boxm2_batch.set_input_from_db(0, lvcs)
    boxm2_batch.set_input_float(1, heading)
    boxm2_batch.set_input_float(2, tilt)
    boxm2_batch.set_input_float(3, roll)
    boxm2_batch.set_input_float(4, lat)
    boxm2_batch.set_input_float(5, lon)
    boxm2_batch.set_input_float(6, alt)
    boxm2_batch.set_input_float(7, fov_hor)
    boxm2_batch.set_input_float(8, fov_ver)
    boxm2_batch.set_input_unsigned(9, ni)
    boxm2_batch.set_input_unsigned(10, nj)
    boxm2_batch.run_process()
    (c_id, c_type) = boxm2_batch.commit_output(0)
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

    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess5")
    boxm2_batch.set_input_double_array(0, k)
    boxm2_batch.set_input_double_array(1, r)
    boxm2_batch.set_input_double_array(2, t)
    boxm2_batch.run_process()
    (db_id, db_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(db_id, db_type)
    return cam

## input two sets of points that correspond to each other in two different coordinate systems
## compute the similarity transformation that maps space of pts0 to space of pts1, the size of pts0 and pts1 better match!
## outputs a 4 by 4 similarity matrix as a vector of size 16
## construct the matrix as follows
## 0  1  2  3
## 4  5  6  7
## 8  9  10 11
## 12 13 14 15
def compute_transformation(pts0_xs, pts0_ys, pts0_zs, 
                           pts1_xs, pts1_ys, pts1_zs, 
                           input_cam_folder, output_cam_folder):
    boxm2_batch.init_process("vpglTransformSpaceProcess");
    boxm2_batch.set_input_double_array(0, pts0_xs)
    boxm2_batch.set_input_double_array(1, pts0_ys)
    boxm2_batch.set_input_double_array(2, pts0_zs)
    boxm2_batch.set_input_double_array(3, pts1_xs)
    boxm2_batch.set_input_double_array(4, pts1_ys)
    boxm2_batch.set_input_double_array(5, pts1_zs)
    boxm2_batch.set_input_string(6, input_cam_folder)
    boxm2_batch.set_input_string(7, output_cam_folder)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    matrix_as_array = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    (id,type) = boxm2_batch.commit_output(1)
    scale = boxm2_batch.get_output_double(id)
    boxm2_batch.remove_data(id)
    return matrix_as_array, scale

## inputs a 4 by 4 similarity matrix as a vector of size 16
## construct the matrix as follows
## 0  1  2  3
## 4  5  6  7
## 8  9  10 11
## 12 13 14 15
def compute_transformed_box(min_pt, max_pt, matrix_as_array):
    boxm2_batch.init_process("vpglTransformBoxProcess");
    boxm2_batch.set_input_double_array(0, min_pt)
    boxm2_batch.set_input_double_array(1, max_pt)
    boxm2_batch.set_input_double_array(2, matrix_as_array)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    out_min_pt = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    (id,type) = boxm2_batch.commit_output(1)
    out_max_pt = boxm2_batch.get_output_double_array(id)
    boxm2_batch.remove_data(id)
    return out_min_pt, out_max_pt

