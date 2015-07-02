from bvxm_register import bvxm_batch, dbvalue;
#import math, numpy;
import math;

###################;
#camera loading;
###################;
def load_perspective_camera(file_path) :
  bvxm_batch.init_process("vpglLoadPerspectiveCameraProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def load_affine_camera(file_path, viewing_dist=1000) :
  bvxm_batch.init_process("vpglLoadAffineCameraProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.set_input_double(1, viewing_dist);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def load_projective_camera(file_path) :
  bvxm_batch.init_process("vpglLoadProjCameraProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z), up = (x,y,z);
def create_perspective_camera( scale, ppoint, center, look_pt,up = [0,1,0] ) :
  bvxm_batch.init_process("vpglCreatePerspectiveCameraProcess");
  bvxm_batch.set_input_double(0, scale[0]);
  bvxm_batch.set_input_double(1, ppoint[0]);
  bvxm_batch.set_input_double(2, scale[1]);
  bvxm_batch.set_input_double(3, ppoint[1]);
  bvxm_batch.set_input_double(4, center[0]);
  bvxm_batch.set_input_double(5, center[1]);
  bvxm_batch.set_input_double(6, center[2]);
  bvxm_batch.set_input_double(7, look_pt[0]);
  bvxm_batch.set_input_double(8, look_pt[1]);
  bvxm_batch.set_input_double(9, look_pt[2]);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;


def create_perspective_camera_from_kml(ni, nj, right_fov, top_fov, altitude, heading, tilt, roll, cent_x, cent_y) :
  bvxm_batch.init_process("vpglCreatePerspCameraFromKMLProcess");
  bvxm_batch.set_input_unsigned(0, ni);
  bvxm_batch.set_input_unsigned(1, nj);
  bvxm_batch.set_input_double(2, right_fov);
  bvxm_batch.set_input_double(3, top_fov);
  bvxm_batch.set_input_double(4, altitude);
  bvxm_batch.set_input_double(5, heading);
  bvxm_batch.set_input_double(6, tilt);
  bvxm_batch.set_input_double(7, roll);
  bvxm_batch.set_input_double(8, cent_x);
  bvxm_batch.set_input_double(9, cent_y);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_perspective_camera_from_kml_file(NI, NJ, kml_file) :
  bvxm_batch.init_process("vpglLoadPerspCameraFromKMLFileProcess");
  bvxm_batch.set_input_unsigned(0, NI);
  bvxm_batch.set_input_unsigned(1, NJ);
  bvxm_batch.set_input_string(2, kml_file);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  (id,type) = bvxm_batch.commit_output(1);
  longitude = bvxm_batch.get_output_double(id);
  (id,type) = bvxm_batch.commit_output(2);
  latitude = bvxm_batch.get_output_double(id);
  (id,type) = bvxm_batch.commit_output(3);
  altitude = bvxm_batch.get_output_double(id);
  return cam, longitude, latitude, altitude;

#resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);
def resample_perspective_camera( cam, size0, size1 ):
  bvxm_batch.init_process("vpglResamplePerspectiveCameraProcess");
  bvxm_batch.set_input_from_db(0, cam);
  bvxm_batch.set_input_int(1, size0[0]);
  bvxm_batch.set_input_int(2, size0[1]);
  bvxm_batch.set_input_int(3, size1[0]);
  bvxm_batch.set_input_int(4, size1[1]);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  out = dbvalue(id,type);
  return out;
#resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1);
def get_perspective_camera_center( cam):
  bvxm_batch.init_process("vpglGetPerspectiveCamCenterProcess");
  bvxm_batch.set_input_from_db(0, cam);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  x=bvxm_batch.get_output_float(id);
  (id, type) = bvxm_batch.commit_output(1);
  y=bvxm_batch.get_output_float(id);
  (id, type) = bvxm_batch.commit_output(2);
  z=bvxm_batch.get_output_float(id);
  return x,y,z;

# returns cartesian cam center from azimuth (degrees), elevation (degrees), radius, look point;
def get_camera_center( azimuth, elevation, radius, lookPt) :
  deg_to_rad = math.pi/180.0;
  el = elevation*deg_to_rad;
  az = azimuth*deg_to_rad;
  cx = radius*math.sin(el)*math.cos(az);
  cy = radius*math.sin(el)*math.sin(az);
  cz = radius*math.cos(el);
  center = (cx + lookPt[0], cy + lookPt[1], cz + lookPt[2]);
  return center;

#returns spherical coordinates about sCenter given cartesian point;
def cart2sphere(cartPt, sCenter):
  #offset cart point;
  cartPt = numpy.subtract(cartPt, sCenter);
  rad = math.sqrt( sum(cartPt*cartPt) );
  az = math.atan2(cartPt[1],cartPt[0]);
  el = math.acos(cartPt[2]/rad);
  return (math.degrees(az), math.degrees(el), rad);

def load_rational_camera(file_path) :
  bvxm_batch.init_process("vpglLoadRationalCameraProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_rational_camera_nitf(file_path) :
  bvxm_batch.init_process("vpglLoadRationalCameraNITFProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_local_rational_camera(file_path) :
  bvxm_batch.init_process("vpglLoadLocalRationalCameraProcess");
  bvxm_batch.set_input_string(0, file_path);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def convert_local_rational_perspective_camera(local_cam) :
  bvxm_batch.init_process("vpglConvertLocalRationalToPerspectiveProcess");
  bvxm_batch.set_input_from_db(0, local_cam);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def create_local_rational_camera(rational_cam_fname, lvcs_fname):
    bvxm_batch.init_process('vpglCreateLocalRationalCameraProcess');
    bvxm_batch.set_input_string(0,rational_cam_fname);
    bvxm_batch.set_input_string(1,lvcs_fname);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    cam = dbvalue(id,type);
    return cam;

###################;
#camera saving;
###################;
def save_rational_camera(camera,path) :
  bvxm_batch.init_process("vpglSaveRationalCameraProcess");
  bvxm_batch.set_input_from_db(0,camera);
  bvxm_batch.set_input_string(1,path);
  bvxm_batch.run_process();
def save_perspective_camera(camera,path) :
  bvxm_batch.init_process("vpglSavePerspectiveCameraProcess");
  bvxm_batch.set_input_from_db(0,camera);
  bvxm_batch.set_input_string(1,path);
  bvxm_batch.run_process();
def save_perspective_camera_vrml(camera,path) :
  bvxm_batch.init_process("vpglSavePerspectiveCameraVrmlProcess");
  bvxm_batch.set_input_from_db(0,camera);
  bvxm_batch.set_input_string(1,path);
  bvxm_batch.set_input_float(2,5.0);
  bvxm_batch.run_process();
def save_perspective_cameras_vrml(camerafolder,path) :
  bvxm_batch.init_process("vpglSavePerspectiveCamerasVrmlProcess");
  bvxm_batch.set_input_string(0,camerafolder);
  bvxm_batch.set_input_string(1,path);
  bvxm_batch.set_input_float(2,5.0);
  bvxm_batch.run_process();
def save_proj_camera(camera, path):
  bvxm_batch.init_process("vpglSaveProjectiveCameraProcess");
  bvxm_batch.set_input_from_db(0, camera);
  bvxm_batch.set_input_string(1, path);
  bvxm_batch.run_process();

#################################################;
# perspective go generic conversion;
#################################################;
def persp2gen(pcam, ni, nj, level=0) :
  bvxm_batch.init_process("vpglConvertToGenericCameraProcess");
  bvxm_batch.set_input_from_db(0, pcam);
  bvxm_batch.set_input_unsigned(1, ni);
  bvxm_batch.set_input_unsigned(2, nj);
  bvxm_batch.set_input_unsigned(3, level);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  gcam = dbvalue(id,type);
  return gcam;

def persp2genWmargin(pcam, ni, nj, margin, level=0) :
  bvxm_batch.init_process("vpglConvertToGenericCameraWithMarginProcess");
  bvxm_batch.set_input_from_db(0, pcam);
  bvxm_batch.set_input_unsigned(1, ni);
  bvxm_batch.set_input_unsigned(2, nj);
  bvxm_batch.set_input_unsigned(3, level);
  bvxm_batch.set_input_int(4, margin);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  gcam = dbvalue(id,type);
  (id,type) = bvxm_batch.commit_output(1);
  ni = bvxm_batch.get_output_unsigned(id);
  (id,type) = bvxm_batch.commit_output(2);
  nj = bvxm_batch.get_output_unsigned(id);
  (id,type) = bvxm_batch.commit_output(3);
  new_pers_cam = dbvalue(id,type);
  return (gcam, ni, nj, new_pers_cam);

def write_generic_to_vrml(cam, out_file_name, level=0):
  bvxm_batch.init_process("vpglWriteGenericCameraProcess");
  bvxm_batch.set_input_from_db(0, cam);
  bvxm_batch.set_input_string(1, out_file_name);
  bvxm_batch.set_input_unsigned(2, level);
  bvxm_batch.run_process();
  
#gets bounding box from a directory of cameras... (incomplete)_;
def camera_dir_planar_bbox(dir_name) :
  bvxm_batch.init_process("vpglGetBoundingBoxProcess");
  bvxm_batch.set_input_string(0, dir_name);
  bvxm_batch.run_process();

def project_point(camera,x,y,z):
    bvxm_batch.init_process('vpglProjectProcess');
    bvxm_batch.set_input_from_db(0,camera);
    bvxm_batch.set_input_float(1,x);
    bvxm_batch.set_input_float(2,y);
    bvxm_batch.set_input_float(3,z);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    u = bvxm_batch.get_output_float(id);
    (id,type) = bvxm_batch.commit_output(1);
    v = bvxm_batch.get_output_float(id);
    return (u,v);

# gets view direction at a point for a perspective camera;
def get_view_at_point(persp_cam,x,y,z):
  bvxm_batch.init_process("vpglGetViewDirectionAtPointProcess");
  bvxm_batch.set_input_from_db(0,persp_cam);
  bvxm_batch.set_input_float(1,x);
  bvxm_batch.set_input_float(2,y);
  bvxm_batch.set_input_float(3,z);
  bvxm_batch.run_process();
  (id,type) = bvxm_batch.commit_output(0);
  theta=bvxm_batch.get_output_float(id);
  (id,type) = bvxm_batch.commit_output(1);
  phi=bvxm_batch.get_output_float(id);
  return theta, phi;

def get_3d_from_depth(persp_cam,u,v,t) :
  bvxm_batch.init_process("vpglGenerate3dPointFromDepthProcess");
  bvxm_batch.set_input_from_db(0,persp_cam);
  bvxm_batch.set_input_float(1,u);
  bvxm_batch.set_input_float(2,v);
  bvxm_batch.set_input_float(3,t);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  x=bvxm_batch.get_output_float(id);
  (id, type) = bvxm_batch.commit_output(1);
  y=bvxm_batch.get_output_float(id);
  (id, type) = bvxm_batch.commit_output(2);
  z=bvxm_batch.get_output_float(id);
  return x,y,z;

# triangulates a list of cams and a list of points;
def get_3d_from_cams( cams, points ):
    assert( len(cams) == len(points) and len(cams) > 1 );
    #list of points will just be [u1,v1,u2,v2...];
    ptlist = [];
    for p in points:
        ptlist.append(p[0]);
        ptlist.append(p[1]);
    #list of cam ids (type will be checked in C++);
    camlist = [];
    for cam in cams:
        camlist.append(cam.id);
    bvxm_batch.init_process("vpglGenerate3dPointFromCamsProcess");
    bvxm_batch.set_input_unsigned_array(0, camlist);
    bvxm_batch.set_input_int_array(1, ptlist);
    bvxm_batch.run_process();
    (id, type) = bvxm_batch.commit_output(0);
    x=bvxm_batch.get_output_float(id);
    (id, type) = bvxm_batch.commit_output(1);
    y=bvxm_batch.get_output_float(id);
    (id, type) = bvxm_batch.commit_output(2);
    z=bvxm_batch.get_output_float(id);
    return x,y,z;

# create a generic camera;
def convert_to_generic_camera(cam_in, ni, nj, level=0):
    bvxm_batch.init_process('vpglConvertToGenericCameraProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.set_input_unsigned(1,ni);
    bvxm_batch.set_input_unsigned(2,nj);
    bvxm_batch.set_input_unsigned(3,level);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    generic_cam = dbvalue(id,type);
    return generic_cam;

# create a generic camera from a local rational with user-specified z range;
def convert_local_rational_to_generic(cam_in, ni, nj, min_z, max_z, level=0):
    bvxm_batch.init_process('vpglConvertLocalRationalToGenericProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.set_input_unsigned(1,ni);
    bvxm_batch.set_input_unsigned(2,nj);
    bvxm_batch.set_input_float(3,min_z);
    bvxm_batch.set_input_float(4,max_z);
    bvxm_batch.set_input_unsigned(5,level);
    if not bvxm_batch.run_process():
        return None;
    (id,type) = bvxm_batch.commit_output(0);
    generic_cam = dbvalue(id,type);
    return generic_cam;

# correct a rational camera;
def correct_rational_camera(cam_in, offset_x, offset_y):
    bvxm_batch.init_process('vpglCorrectRationalCameraProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.set_input_double(1,offset_x);
    bvxm_batch.set_input_double(2,offset_y);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    corrected_cam = dbvalue(id,type);
    return corrected_cam;

def get_rational_camera_offsets(cam_in):
    bvxm_batch.init_process('vpglGetRationalCameraOffsetsProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    offset_u = bvxm_batch.get_output_double(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(1);
    offset_v = bvxm_batch.get_output_double(id);
    bvxm_batch.remove_data(id);
    return (offset_u,offset_v);

def get_correction_offset(cam_orig, cam_corrected):
  offset_u,   offset_v   = get_rational_camera_offsets(cam_orig);
  offset_u_c, offset_v_c = get_rational_camera_offsets(cam_corrected);
  diff_u = offset_u_c - offset_u;
  diff_v = offset_v_c - offset_v;
  return diff_u, diff_v;

def find_offset_and_correct_rational_camera(cam_orig, cam_corrected, cam_to_be_corrected):
  offset_u, offset_v = get_rational_camera_offsets(cam_orig);
  offset_u_c, offset_v_c = get_rational_camera_offsets(cam_corrected);
  diff_u = offset_u_c - offset_u;
  diff_v = offset_v_c - offset_v;
  print " diff_u = %.1f, diff_v = %.1f " % (diff_u, diff_v)
  cam_out = correct_rational_camera(cam_to_be_corrected, diff_u, diff_v);
  return cam_out

# convert lat,lon,el to local coordinates;
def convert_to_local_coordinates(lvcs_filename,lat,lon,el):
    bvxm_batch.init_process('vpglConvertToLocalCoordinatesProcess');
    bvxm_batch.set_input_string(0,lvcs_filename);
    bvxm_batch.set_input_float(1,lat);
    bvxm_batch.set_input_float(2,lon);
    bvxm_batch.set_input_float(3,el);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    x = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(1);
    y = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(2);
    z = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    return (x,y,z);

# convert lat,lon,el to local coordinates;
def convert_to_local_coordinates2(lvcs,lat,lon,el):
    bvxm_batch.init_process('vpglConvertToLocalCoordinatesProcess2');
    bvxm_batch.set_input_from_db(0,lvcs);
    bvxm_batch.set_input_float(1,lat);
    bvxm_batch.set_input_float(2,lon);
    bvxm_batch.set_input_float(3,el);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    x = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(1);
    y = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(2);
    z = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    return (x,y,z);
# convert lat,lon,el to local coordinates;
def convert_local_to_global_coordinates(lvcs,x,y,z):
    bvxm_batch.init_process('vpglConvertLocalToGlobalCoordinatesProcess');
    bvxm_batch.set_input_from_db(0,lvcs);
    bvxm_batch.set_input_float(1,x);
    bvxm_batch.set_input_float(2,y);
    bvxm_batch.set_input_float(3,z);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    lat = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(1);
    lon = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    (id,type) = bvxm_batch.commit_output(2);
    el = bvxm_batch.get_output_float(id);
    bvxm_batch.remove_data(id);
    return (lat,lon,el);
# convert lat,lon,el to local coordinates;
def create_lvcs(lat,lon,el,csname):
    bvxm_batch.init_process('vpglCreateLVCSProcess');
    bvxm_batch.set_input_float(0,lat);
    bvxm_batch.set_input_float(1,lon);
    bvxm_batch.set_input_float(2,el);
    bvxm_batch.set_input_string(3,csname);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    lvcs = dbvalue(id,type);
    return lvcs;

# randomly sample a camera rotated around principle axis;
def perturb_camera(cam_in, angle, rng):
    bvxm_batch.init_process('vpglPerturbPerspCamOrientProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.set_input_float(1,angle);
    bvxm_batch.set_input_from_db(2,rng);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    pert_cam = dbvalue(id,type);
    (theta_id,type) = bvxm_batch.commit_output(1);
    (phi_id,type) = bvxm_batch.commit_output(2);
    theta = bvxm_batch.get_output_float(theta_id);
    phi = bvxm_batch.get_output_float(phi_id);
    return pert_cam, theta, phi;

def write_perspective_cam_vrml(vrml_filename, pcam, camera_rad, axis_length, r, g, b):
    bvxm_batch.init_process("bvrmlWritePerspectiveCamProcess");
    bvxm_batch.set_input_string(0,vrml_filename);
    bvxm_batch.set_input_from_db(1,pcam);
    bvxm_batch.set_input_float(2,camera_rad);
    bvxm_batch.set_input_float(3,axis_length);
    bvxm_batch.set_input_float(4,r);
    bvxm_batch.set_input_float(5,g);
    bvxm_batch.set_input_float(6,b);
    bvxm_batch.run_process();

# rotate a camera around principle axis;
def rotate_perspective_camera(cam_in, theta, phi):
    bvxm_batch.init_process('vpglRotatePerspCamProcess');
    bvxm_batch.set_input_from_db(0,cam_in);
    bvxm_batch.set_input_float(1,theta);
    bvxm_batch.set_input_float(2,phi);
    bvxm_batch.run_process();
    (id,type) = bvxm_batch.commit_output(0);
    rot_cam = dbvalue(id,type);
    return rot_cam;

def get_perspective_cam_center(pcam):
    bvxm_batch.init_process("vpglGetPerspectiveCamCenterProcess");
    bvxm_batch.set_input_from_db(0, pcam);
    bvxm_batch.run_process();
    (x_id,x_type) = bvxm_batch.commit_output(0);
    x = bvxm_batch.get_output_float(x_id);
    (y_id,type) = bvxm_batch.commit_output(1);
    y = bvxm_batch.get_output_float(y_id);
    (z_id,type) = bvxm_batch.commit_output(2);
    z = bvxm_batch.get_output_float(z_id);
    return x, y, z;

def create_perspective_camera2(pcam, cent_x, cent_y, cent_z):
    bvxm_batch.init_process("vpglCreatePerspectiveCameraProcess2");
    bvxm_batch.set_input_from_db(0, pcam);
    bvxm_batch.set_input_float(1, cent_x);
    bvxm_batch.set_input_float(2, cent_y);
    bvxm_batch.set_input_float(3, cent_z);
    bvxm_batch.run_process();
    (c_id,c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id,c_type);
    return cam;

def create_perspective_camera_with_rot(pcam, phi, theta, cent_x, cent_y, cent_z):
    bvxm_batch.init_process("vpglCreatePerspectiveCameraProcess3");
    bvxm_batch.set_input_from_db(0, pcam);
    bvxm_batch.set_input_float(1, phi);
    bvxm_batch.set_input_float(2, theta);
    bvxm_batch.set_input_float(3, cent_x);
    bvxm_batch.set_input_float(4, cent_y);
    bvxm_batch.set_input_float(5, cent_z);
    bvxm_batch.run_process();
    (c_id,c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id,c_type);
    return cam;

def get_nitf_footprint(nitf_list_filename, out_kml_filename, r = 255, g = 255, b = 255):
    bvxm_batch.init_process('vpglNITFFootprintProcess');
    bvxm_batch.set_input_string(0,nitf_list_filename);
    bvxm_batch.set_input_string(1,out_kml_filename);
    bvxm_batch.set_input_unsigned(2,r)
    bvxm_batch.set_input_unsigned(3,g)
    bvxm_batch.set_input_unsigned(4,b)
    bvxm_batch.run_process();

def get_nitf_footprint2(nitf_img_filename, out_kml_file = "", is_kml = False):
    bvxm_batch.init_process("vpglNITFFootprintProcess2");
    bvxm_batch.set_input_string(0, nitf_img_filename);
    bvxm_batch.set_input_string(1, out_kml_file);
    bvxm_batch.set_input_bool(2, is_kml)
    status = bvxm_batch.run_process()
    if status:
      (id, type) = bvxm_batch.commit_output(0)
      ll_lon = bvxm_batch.get_output_double(id)
      (id, type) = bvxm_batch.commit_output(1)
      ll_lat = bvxm_batch.get_output_double(id)
      (id, type) = bvxm_batch.commit_output(2)
      ll_elev = bvxm_batch.get_output_double(id)
      (id, type) = bvxm_batch.commit_output(3)
      ur_lon = bvxm_batch.get_output_double(id)
      (id, type) = bvxm_batch.commit_output(4)
      ur_lat = bvxm_batch.get_output_double(id)
      (id, type) = bvxm_batch.commit_output(5)
      ur_elev = bvxm_batch.get_output_double(id)
      return ll_lon, ll_lat, ll_elev, ur_lon, ur_lat, ur_elev
    else:
      return 0.0, 0.0, 0.0, 0.0, 0.0, 0.0

def get_geocam_footprint(geocam, geotiff_filename, out_kml_filename,init_finish=True):
    bvxm_batch.init_process('vpglGeoFootprintProcess');
    bvxm_batch.set_input_from_db(0,geocam);
    bvxm_batch.set_input_string(1,geotiff_filename);
    bvxm_batch.set_input_string(2,out_kml_filename);
    bvxm_batch.set_input_bool(3,init_finish);
    bvxm_batch.run_process();

def load_geotiff_cam(tfw_filename, lvcs=0, utm_zone=0, utm_hemisphere=0):
    bvxm_batch.init_process("vpglLoadGeoCameraProcess");
    bvxm_batch.set_input_string(0, tfw_filename);
    if lvcs != 0:
      bvxm_batch.set_input_from_db(1, lvcs);
    bvxm_batch.set_input_int(2, utm_zone);
    bvxm_batch.set_input_unsigned(3, utm_hemisphere);
    bvxm_batch.run_process();
    (c_id,c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id,c_type);
    return cam;

def save_geocam_to_tfw(cam, tfw_filename):
    bvxm_batch.init_process("vpglSaveGeoCameraTFWProcess");
    bvxm_batch.set_input_from_db(0, cam);
    bvxm_batch.set_input_string(1, tfw_filename);
    bvxm_batch.run_process();

def load_geotiff_cam2(filename, ni, nj):
    bvxm_batch.init_process("vpglLoadGeoCameraProcess2");
    bvxm_batch.set_input_string(0, filename);
    bvxm_batch.set_input_unsigned(1, ni);
    bvxm_batch.set_input_unsigned(2, nj);
    bvxm_batch.run_process();
    (c_id,c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id,c_type);
    return cam;

def load_geotiff_from_header(filename, lvcs=0):
    bvxm_batch.init_process("vpglLoadGeotiffCamFromHeaderProcess");
    bvxm_batch.set_input_string(0, filename);
    if lvcs != 0:
      bvxm_batch.set_input_from_db(1, lvcs);
    bvxm_batch.run_process();
    (c_id,c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id,c_type);
    return cam;

def translate_geo_camera(geocam, x, y):
    bvxm_batch.init_process("vpglTranslateGeoCameraProcess");
    bvxm_batch.set_input_from_db(0, geocam);
    bvxm_batch.set_input_double(1, x);
    bvxm_batch.set_input_double(2, y);
    bvxm_batch.run_process();
    (c_id, c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id, c_type);
    return cam;

def geo2generic(geocam, ni, nj, scene_height, level):
    bvxm_batch.init_process("vpglConvertGeoCameraToGenericProcess");
    bvxm_batch.set_input_from_db(0, geocam);
    bvxm_batch.set_input_int(1, ni);
    bvxm_batch.set_input_int(2, nj);
    bvxm_batch.set_input_double(3, scene_height);
    bvxm_batch.set_input_int(4, level);
    bvxm_batch.run_process();
    (c_id, c_type) = bvxm_batch.commit_output(0);
    cam = dbvalue(c_id, c_type);
    return cam;

def save_lvcs(lvcs, lvcs_filename):
    bvxm_batch.init_process("vpglSaveLVCSProcess");
    bvxm_batch.set_input_from_db(0, lvcs);
    bvxm_batch.set_input_string(1, lvcs_filename);
    return bvxm_batch.run_process()

def create_and_save_lvcs(lat,lon,hae,cs_name,lvcs_filename):
    bvxm_batch.init_process("vpglCreateAndSaveLVCSProcess");
    bvxm_batch.set_input_float(0,lat);
    bvxm_batch.set_input_float(1,lon);
    bvxm_batch.set_input_float(2,hae);
    bvxm_batch.set_input_string(3,cs_name)
    bvxm_batch.set_input_string(4,lvcs_filename);
    bvxm_batch.run_process();
    return;

def load_lvcs(lvcs_filename):
    bvxm_batch.init_process("vpglLoadLVCSProcess");
    bvxm_batch.set_input_string(0, lvcs_filename);
    bvxm_batch.run_process();
    (lvcs_id, lvcs_type) = bvxm_batch.commit_output(0);
    lvcs = dbvalue(lvcs_id, lvcs_type);
    return lvcs;

def geo_cam_global_to_img(geocam, lon, lat):
    bvxm_batch.init_process("vpglGeoGlobalToImgProcess");
    bvxm_batch.set_input_from_db(0, geocam);
    bvxm_batch.set_input_double(1, lon);
    bvxm_batch.set_input_double(2, lat);
    bvxm_batch.run_process();
    (id, type) = bvxm_batch.commit_output(0);
    u = bvxm_batch.get_output_int(id);
    (id, type) = bvxm_batch.commit_output(1);
    v = bvxm_batch.get_output_int(id);
    return u, v;

def convert_perspective_to_nvm(cams_dir,imgs_dir, output_nvm):
    bvxm_batch.init_process("vpglExportCamerasToNvmProcess");
    bvxm_batch.set_input_string(0, cams_dir);
    bvxm_batch.set_input_string(1, imgs_dir);
    bvxm_batch.set_input_string(2, output_nvm);
    return bvxm_batch.run_process();
def interpolate_perspective_cameras(cam0,cam1,ncams,outdir):
    bvxm_batch.init_process("vpglInterpolatePerspectiveCamerasProcess");
    bvxm_batch.set_input_from_db(0, cam0);
    bvxm_batch.set_input_from_db(1, cam1);
    bvxm_batch.set_input_unsigned(2, ncams);
    bvxm_batch.set_input_string(3, outdir);
    return bvxm_batch.run_process();

def compute_affine_from_local_rational(cropped_cam, min_x, min_y, min_z, max_x, max_y, max_z, n_points=100):
    bvxm_batch.init_process("vpglComputeAffineFromRationalProcess");
    bvxm_batch.set_input_from_db(0, cropped_cam);
    bvxm_batch.set_input_double(1, min_x);
    bvxm_batch.set_input_double(2, min_y);
    bvxm_batch.set_input_double(3, min_z);
    bvxm_batch.set_input_double(4, max_x);
    bvxm_batch.set_input_double(5, max_y);
    bvxm_batch.set_input_double(6, max_z);
    bvxm_batch.set_input_unsigned(7, n_points);
    bvxm_batch.run_process();
    (id, type) = bvxm_batch.commit_output(0);
    out_cam = dbvalue(id, type);
    return out_cam

## use the affine cameras of the images to compute an affine fundamental matrix and rectify them (flatten epipolar lines to scan lines and align them)
## use the 3-d box that the cameras see to compute correspondences for minimally distortive alignment
def affine_rectify_images(img1, affine_cam1, img2, affine_cam2, min_x, min_y, min_z, max_x, max_y, max_z, local_ground_plane_height = 5, n_points=100):
  bvxm_batch.init_process("vpglAffineRectifyImagesProcess");
  bvxm_batch.set_input_from_db(0, img1);
  bvxm_batch.set_input_from_db(1, affine_cam1);
  bvxm_batch.set_input_from_db(2, img2);
  bvxm_batch.set_input_from_db(3, affine_cam2);
  bvxm_batch.set_input_double(4, min_x);
  bvxm_batch.set_input_double(5, min_y);
  bvxm_batch.set_input_double(6, min_z);
  bvxm_batch.set_input_double(7, max_x);
  bvxm_batch.set_input_double(8, max_y);
  bvxm_batch.set_input_double(9, max_z);
  bvxm_batch.set_input_unsigned(10, n_points);
  bvxm_batch.set_input_double(11, local_ground_plane_height);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img1 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  out_cam1 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(2);
  out_img2 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(3);
  out_cam2 = dbvalue(id, type);
  return out_img1, out_cam1, out_img2, out_cam2

## use the affine cameras of the images to compute an affine fundamental matrix and rectify them (flatten epipolar lines to scan lines and align them)
## use the 3-d box that the cameras see to compute correspondences for minimally distortive alignment, use the local rational cameras to find the correspondence points
def affine_rectify_images2(img1, affine_cam1, local_rational_cam1, img2, affine_cam2, local_rational_cam2, min_x, min_y, min_z, max_x, max_y, max_z, output_path_H1, output_path_H2, local_ground_plane_height = 5, n_points=100):
  bvxm_batch.init_process("vpglAffineRectifyImagesProcess2");
  bvxm_batch.set_input_from_db(0, img1);
  bvxm_batch.set_input_from_db(1, affine_cam1);
  bvxm_batch.set_input_from_db(2, local_rational_cam1);
  bvxm_batch.set_input_from_db(3, img2);
  bvxm_batch.set_input_from_db(4, affine_cam2);
  bvxm_batch.set_input_from_db(5, local_rational_cam2);
  bvxm_batch.set_input_double(6, min_x);
  bvxm_batch.set_input_double(7, min_y);
  bvxm_batch.set_input_double(8, min_z);
  bvxm_batch.set_input_double(9, max_x);
  bvxm_batch.set_input_double(10, max_y);
  bvxm_batch.set_input_double(11, max_z);
  bvxm_batch.set_input_unsigned(12, n_points);
  bvxm_batch.set_input_double(13, local_ground_plane_height);
  bvxm_batch.set_input_string(14, output_path_H1);
  bvxm_batch.set_input_string(15, output_path_H2);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img1 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  out_cam1 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(2);
  out_img2 = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(3);
  out_cam2 = dbvalue(id, type);
  return out_img1, out_cam1, out_img2, out_cam2

## use the affine cameras of the images to compute an affine fundamental matrix and write the f matrix out
def affine_f_matrix(affine_cam1, affine_cam2, output_path):
  bvxm_batch.init_process("vpglAffineFMatrixProcess");
  bvxm_batch.set_input_from_db(0, affine_cam1);
  bvxm_batch.set_input_from_db(1, affine_cam2);
  bvxm_batch.set_input_string(2, output_path);
  bvxm_batch.run_process();

def construct_height_map_from_disparity(img1, img1_disp, min_disparity, local_rational_cam1, img2, local_rational_cam2, min_x, min_y, min_z, max_x, max_y, max_z, path_H1, path_H2):
  bvxm_batch.init_process("vpglConstructHeightMapProcess");
  bvxm_batch.set_input_from_db(0, img1);
  bvxm_batch.set_input_from_db(1, local_rational_cam1);
  bvxm_batch.set_input_string(2, img1_disp);
  bvxm_batch.set_input_float(3, min_disparity);
  bvxm_batch.set_input_from_db(4, img2);
  bvxm_batch.set_input_from_db(5, local_rational_cam2);
  bvxm_batch.set_input_double(6, min_x);
  bvxm_batch.set_input_double(7, min_y);
  bvxm_batch.set_input_double(8, min_z);
  bvxm_batch.set_input_double(9, max_x);
  bvxm_batch.set_input_double(10, max_y);
  bvxm_batch.set_input_double(11, max_z);
  bvxm_batch.set_input_string(12, path_H1);
  bvxm_batch.set_input_string(13, path_H2);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_map = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  disparity_map = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(2);
  ortho_disp_map = dbvalue(id, type);
  return out_map, disparity_map, ortho_disp_map

## use the 3-d box to crop an image using image camera, given certain uncertainty value in meter unit
## note that the input 3-d box is in unit of wgs84 geo coordinates
def crop_image_using_3d_box(img_res, camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev, uncertainty, lvcs=0):
  bvxm_batch.init_process("vpglCropImgUsing3DboxProcess")
  bvxm_batch.set_input_from_db(0, img_res);
  bvxm_batch.set_input_from_db(1, camera);
  bvxm_batch.set_input_double(2, lower_left_lon);
  bvxm_batch.set_input_double(3, lower_left_lat);
  bvxm_batch.set_input_double(4, lower_left_elev);
  bvxm_batch.set_input_double(5, upper_right_lon);
  bvxm_batch.set_input_double(6, upper_right_lat);
  bvxm_batch.set_input_double(7, upper_right_elev);
  bvxm_batch.set_input_double(8, uncertainty);
  if lvcs != 0:
    bvxm_batch.set_input_from_db(9, lvcs);
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    local_cam  = dbvalue(id, type);
    (id, type) = bvxm_batch.commit_output(1);
    i0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(2);
    j0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(3);
    ni = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(4);
    nj = bvxm_batch.get_output_unsigned(id)
    return status, local_cam, i0, j0, ni, nj;
  else:
    return status, dbvalue(0, ""), 0, 0, 0, 0;

## use the 3-d box to crop an image using image camera, given certain uncertainty value in meter unit
## note that the elevation of 3-d box is obtained from DEM height map
def crop_image_using_3d_box_dem(img_res, camera, ll_lon, ll_lat, ur_lon, ur_lat, dem_folder, extra_height, uncertainty, lvcs = 0):
  bvxm_batch.init_process("vpglCropImgUsing3DboxDemProcess")
  bvxm_batch.set_input_from_db(0, img_res)
  bvxm_batch.set_input_from_db(1, camera)
  bvxm_batch.set_input_double(2, ll_lon)
  bvxm_batch.set_input_double(3, ll_lat)
  bvxm_batch.set_input_double(4, ur_lon)
  bvxm_batch.set_input_double(5, ur_lat)
  bvxm_batch.set_input_string(6, dem_folder)
  bvxm_batch.set_input_double(7, extra_height)
  bvxm_batch.set_input_double(8, uncertainty)
  if lvcs != 0:
    bvxm_batch.set_input_from_db(9, lvcs)
  status = bvxm_batch.run_process()
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    local_cam  = dbvalue(id, type);
    (id, type) = bvxm_batch.commit_output(1);
    i0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(2);
    j0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(3);
    ni = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(4);
    nj = bvxm_batch.get_output_unsigned(id)
    return status, local_cam, i0, j0, ni, nj;
  else:
    return status, dbvalue(0, ""), 0.0, 0.0, 0.0, 0.0

## use the 3-d box to crop an ortho image using its geo camera
## note that the input 3-d box is in unit of wgs84 geo coordinates
def crop_ortho_image_using_3d_box(img_res, camera, lower_left_lon, lower_left_lat, lower_left_elev, upper_right_lon, upper_right_lat, upper_right_elev):
  bvxm_batch.init_process("vpglCropOrthoUsing3DboxPRocess")
  bvxm_batch.set_input_from_db(0, img_res);
  bvxm_batch.set_input_from_db(1, camera);
  bvxm_batch.set_input_double(2, lower_left_lon);
  bvxm_batch.set_input_double(3, lower_left_lat);
  bvxm_batch.set_input_double(4, lower_left_elev);
  bvxm_batch.set_input_double(5, upper_right_lon);
  bvxm_batch.set_input_double(6, upper_right_lat);
  bvxm_batch.set_input_double(7, upper_right_elev);
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    local_geo_cam  = dbvalue(id, type);
    (id, type) = bvxm_batch.commit_output(1);
    i0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(2);
    j0 = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(3);
    ni = bvxm_batch.get_output_unsigned(id)
    (id, type) = bvxm_batch.commit_output(4);
    nj = bvxm_batch.get_output_unsigned(id)
    return status, local_geo_cam, i0, j0, ni, nj;
  else:
    return status, dbvalue(0, ""), 0, 0, 0, 0;

# give a location (lat, lon) coordinates, return its associate utm coords
def utm_coords(lon, lat):
  bvxm_batch.init_process("vpglComputeUTMZoneProcess");
  bvxm_batch.set_input_double(0, lon);
  bvxm_batch.set_input_double(1, lat);
  result = bvxm_batch.run_process();
  if result:
    (id, type) = bvxm_batch.commit_output(0);
    x = bvxm_batch.get_output_double(id);
    (id, type) = bvxm_batch.commit_output(1);
    y = bvxm_batch.get_output_double(id);
    (id, type) = bvxm_batch.commit_output(2);
    utm_zone = bvxm_batch.get_output_int(id);
    (id, type) = bvxm_batch.commit_output(3);
    northing = bvxm_batch.get_output_int(id);
    return x, y, utm_zone, northing
  else:
    return 0.0, 0.0, 0, 0;
    return 0.0, 0.0, 0, 0;

# get the world point (wgs84) given the image point and rational camera
# Pass default initial guess point (-1.0, -1.0, -1.0) and plane height (-1.0) if initial is unknown at all
def rational_cam_img_to_global(camera, i, j, init_lon=-1.0, init_lat=-1.0, init_elev=-1.0, pl_elev = -1.0, error_tol = 0.05):
  bvxm_batch.init_process("vpglRationalImgToGlobalProcess");
  bvxm_batch.set_input_from_db(0, camera);
  bvxm_batch.set_input_unsigned(1, i);
  bvxm_batch.set_input_unsigned(2, j);
  bvxm_batch.set_input_double(3, init_lon);
  bvxm_batch.set_input_double(4, init_lat);
  bvxm_batch.set_input_double(5, init_elev);
  bvxm_batch.set_input_double(6, pl_elev);
  bvxm_batch.set_input_double(7, error_tol);
  status = bvxm_batch.run_process();
  if status:
    (id, type) = bvxm_batch.commit_output(0);
    lon = bvxm_batch.get_output_double(id);
    (id, type) = bvxm_batch.commit_output(1);
    lat = bvxm_batch.get_output_double(id);
    (id, type) = bvxm_batch.commit_output(2);
    elev = bvxm_batch.get_output_double(id)
    return lon, lat, elev
  else:
    return -1.0, -1.0, -1.0

def rational_cam_nadirness(camera, lat, lon, elev):
  bvxm_batch.init_process("vpglRationalCamNadirnessProcess");
  bvxm_batch.set_input_from_db(0, camera);
  bvxm_batch.set_input_double(1, lat);
  bvxm_batch.set_input_double(2, lon);
  bvxm_batch.set_input_double(3, elev);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  val = bvxm_batch.get_output_double(id)
  return val

# use lvcs to calculate GSD of nitf satellite image
def calculate_nitf_gsd(rational_cam, lon1, lat1, elev1, distance = 1000):
  # create a lvcs
  lvcs = create_lvcs(lat1, lon1, elev1, "wgs84")
  lat2, lon2, elev2 = convert_local_to_global_coordinates(lvcs, distance, distance, 0.0)
  # calculate image pixel
  i1, j1 = project_point(rational_cam, lon1, lat1, elev1)
  i2, j2 = project_point(rational_cam, lon2, lat2, elev2)
  gsd_i = distance / (i2-i1)
  gsd_j = distance / (j2-j1)
  if (gsd_i < 0.0): gsd_i = -1*gsd_i
  if (gsd_j < 0.0): gsd_j = -1*gsd_j
  bvxm_batch.remove_data(lvcs.id)
  return gsd_i, gsd_j


def isfm_rational_camera( trackfile, output_folder, pixel_radius):
  bvxm_batch.init_process("vpglIsfmRationalCameraProcess");
  bvxm_batch.set_input_string(0, trackfile);
  bvxm_batch.set_input_string(1, output_folder);
  bvxm_batch.set_input_float(2, pixel_radius);  ## pixel radius to count for inliers
  if not bvxm_batch.run_process():
    return None, -1.0, -1.0
  (id, type) = bvxm_batch.commit_output(0);
  cam = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  error = bvxm_batch.get_output_float(id);
  (id, type) = bvxm_batch.commit_output(2);
  inliers = bvxm_batch.get_output_float(id);
  return cam,error,inliers

def isfm_rational_camera_seed(track_file, out_folder, dem_folder, ll_lon = 0.0, ll_lat = 0.0, ur_lon = 0.0, ur_lat = 0.0, height_diff = 20.0, pixel_radius = 2.0, enforce_existing = False):
  bvxm_batch.init_process("vpglIsfmRationalCameraSeedProcess")
  bvxm_batch.set_input_string(0, track_file)
  bvxm_batch.set_input_string(1, out_folder)
  bvxm_batch.set_input_string(2, dem_folder)
  bvxm_batch.set_input_float(3, ll_lon)
  bvxm_batch.set_input_float(4, ll_lat)
  bvxm_batch.set_input_float(5, ur_lon)
  bvxm_batch.set_input_float(6, ur_lat)
  bvxm_batch.set_input_double(7, height_diff)
  bvxm_batch.set_input_float(8, pixel_radius)
  bvxm_batch.set_input_bool(9, enforce_existing)
  status = bvxm_batch.run_process()
  return status

def isfm_rational_camera_with_init(track_file, dem_folder, ll_lon = 0.0, ll_lat = 0.0, ur_lon = 0.0, ur_lat = 0.0, height_diff = 20.0, pixel_radius = 2.0):
  bvxm_batch.init_process("vpglIsfmRationalCameraWithInitialProcess")
  bvxm_batch.set_input_string(0, track_file)
  bvxm_batch.set_input_string(1, dem_folder)
  bvxm_batch.set_input_double(2, ll_lon)
  bvxm_batch.set_input_double(3, ll_lat)
  bvxm_batch.set_input_double(4, ur_lon)
  bvxm_batch.set_input_double(5, ur_lat)
  bvxm_batch.set_input_double(6, height_diff)
  bvxm_batch.set_input_float(7, pixel_radius)
  if not bvxm_batch.run_process():
    return None, -1.0, -1.0
  (id, type) = bvxm_batch.commit_output(0)
  cam = dbvalue(id, type)
  (id, type) = bvxm_batch.commit_output(1)
  error = bvxm_batch.get_output_float(id)
  (id, type) = bvxm_batch.commit_output(2)
  inliers = bvxm_batch.get_output_float(id)
  return cam, error, inliers
