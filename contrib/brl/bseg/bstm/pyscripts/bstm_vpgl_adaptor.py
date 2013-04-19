from bstm_register import bstm_batch, dbvalue;
#import math, numpy
import math

###################
#camera loading
###################
def load_perspective_camera(file_path) :
  bstm_batch.init_process("vpglLoadPerspectiveCameraProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def load_affine_camera(file_path) :
  bstm_batch.init_process("vpglLoadAffineCameraProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
def load_projective_camera(file_path) :
  bstm_batch.init_process("vpglLoadProjCameraProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;  
#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z), up = (x,y,z)
def create_perspective_camera( scale, ppoint, center, look_pt, up ) :
  bstm_batch.init_process("vpglCreatePerspectiveCameraProcess");
  bstm_batch.set_input_double(0, scale[0]);
  bstm_batch.set_input_double(1, ppoint[0]);
  bstm_batch.set_input_double(2, scale[1]);
  bstm_batch.set_input_double(3, ppoint[1]);
  bstm_batch.set_input_double(4, center[0]);
  bstm_batch.set_input_double(5, center[1]);
  bstm_batch.set_input_double(6, center[2]);
  bstm_batch.set_input_double(7, look_pt[0]);
  bstm_batch.set_input_double(8, look_pt[1]);
  bstm_batch.set_input_double(9, look_pt[2]);
  bstm_batch.set_input_double(10, up[0]);
  bstm_batch.set_input_double(11, up[1]);
  bstm_batch.set_input_double(12, up[2]);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
  
def create_perspective_camera_from_kml(ni, nj, right_fov, top_fov, altitude, heading, tilt, roll, cent_x, cent_y) :
  bstm_batch.init_process("vpglCreatePerspCameraFromKMLProcess");
  bstm_batch.set_input_unsigned(0, ni);
  bstm_batch.set_input_unsigned(1, nj);
  bstm_batch.set_input_double(2, right_fov);
  bstm_batch.set_input_double(3, top_fov);
  bstm_batch.set_input_double(4, altitude);
  bstm_batch.set_input_double(5, heading);
  bstm_batch.set_input_double(6, tilt);
  bstm_batch.set_input_double(7, roll);
  bstm_batch.set_input_double(8, cent_x);
  bstm_batch.set_input_double(9, cent_y);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_perspective_camera_from_kml_file(NI, NJ, kml_file) :
  bstm_batch.init_process("vpglLoadPerspCameraFromKMLFileProcess");
  bstm_batch.set_input_unsigned(0, NI);
  bstm_batch.set_input_unsigned(1, NJ);
  bstm_batch.set_input_string(2, kml_file);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  (id,type) = bstm_batch.commit_output(1);
  longitude = bstm_batch.get_output_double(id);
  (id,type) = bstm_batch.commit_output(2);
  latitude = bstm_batch.get_output_double(id);
  (id,type) = bstm_batch.commit_output(3);
  altitude = bstm_batch.get_output_double(id);
  return cam, longitude, latitude, altitude;

#resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1)
def resample_perspective_camera( cam, size0, size1 ):
  bstm_batch.init_process("vpglResamplePerspectiveCameraProcess")
  bstm_batch.set_input_from_db(0, cam) 
  bstm_batch.set_input_int(1, size0[0])
  bstm_batch.set_input_int(2, size0[1])
  bstm_batch.set_input_int(3, size1[0])
  bstm_batch.set_input_int(4, size1[1])
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  out = dbvalue(id,type)
  return out

# returns cartesian cam center from azimuth (degrees), elevation (degrees), radius, look point
def get_camera_center( azimuth, elevation, radius, lookPt) :
  deg_to_rad = math.pi/180.0; 
  el = elevation*deg_to_rad
  az = azimuth*deg_to_rad;
  cx = radius*math.sin(el)*math.cos(az);
  cy = radius*math.sin(el)*math.sin(az);
  cz = radius*math.cos(el);
  center = (cx + lookPt[0], cy + lookPt[1], cz + lookPt[2]); 
  return center;

#returns spherical coordinates about sCenter given cartesian point
def cart2sphere(cartPt, sCenter):
  #offset cart point
  cartPt = numpy.subtract(cartPt, sCenter)
  rad = math.sqrt( sum(cartPt*cartPt) )
  az = math.atan2(cartPt[1],cartPt[0])
  el = math.acos(cartPt[2]/rad)
  return ( math.degrees(az), math.degrees(el), rad)

def load_rational_camera(file_path) :
  bstm_batch.init_process("vpglLoadRationalCameraProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_rational_camera_nitf(file_path) :
  bstm_batch.init_process("vpglLoadRationalCameraNITFProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
  
def load_local_rational_camera(file_path) :
  bstm_batch.init_process("vpglLoadLocalRationalCameraProcess");
  bstm_batch.set_input_string(0, file_path);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def create_local_rational_camera(rational_cam_fname, lvcs_fname):
    bstm_batch.init_process('vpglCreateLocalRationalCameraProcess')
    bstm_batch.set_input_string(0,rational_cam_fname)
    bstm_batch.set_input_string(1,lvcs_fname)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    cam = dbvalue(id,type)
    return cam

###################
#camera saving
###################
def save_rational_camera(camera,path) :
  bstm_batch.init_process("vpglSaveRationalCameraProcess");
  bstm_batch.set_input_from_db(0,camera);
  bstm_batch.set_input_string(1,path);
  bstm_batch.run_process();
def save_perspective_camera(camera,path) :
  bstm_batch.init_process("vpglSavePerspectiveCameraProcess");
  bstm_batch.set_input_from_db(0,camera);
  bstm_batch.set_input_string(1,path);
  bstm_batch.run_process();  
  
#################################################
# perspective go generic conversion
#################################################
def persp2gen(pcam, ni, nj, level=0) :
  bstm_batch.init_process("vpglConvertToGenericCameraProcess");
  bstm_batch.set_input_from_db(0, pcam);
  bstm_batch.set_input_unsigned(1, ni);
  bstm_batch.set_input_unsigned(2, nj);
  bstm_batch.set_input_unsigned(3, level);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  gcam = dbvalue(id,type);
  return gcam;
  


#gets bounding box from a directory of cameras... (incomplete)_
def camera_dir_planar_bbox(dir_name) :
  bstm_batch.init_process("vpglGetBoundingBoxProcess");
  bstm_batch.set_input_string(0, dir_name);
  bstm_batch.run_process();

def project_point(camera,x,y,z):
    bstm_batch.init_process('vpglProjectProcess')
    bstm_batch.set_input_from_db(0,camera)
    bstm_batch.set_input_float(1,x)
    bstm_batch.set_input_float(2,y)
    bstm_batch.set_input_float(3,z)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    u = bstm_batch.get_output_float(id)
    (id,type) = bstm_batch.commit_output(1)
    v = bstm_batch.get_output_float(id)
    return (u,v)

# gets view direction at a point for a perspective camera
def get_view_at_point(persp_cam,x,y,z):
  bstm_batch.init_process("vpglGetViewDirectionAtPointProcess");
  bstm_batch.set_input_from_db(0,persp_cam);
  bstm_batch.set_input_float(1,x);
  bstm_batch.set_input_float(2,y);
  bstm_batch.set_input_float(3,z);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  theta=bstm_batch.get_output_float(id);
  (id,type) = bstm_batch.commit_output(1);
  phi=bstm_batch.get_output_float(id);
  return theta, phi;

def get_3d_from_depth(persp_cam,u,v,t) :
  bstm_batch.init_process("vpglGenerate3dPointFromDepthProcess");
  bstm_batch.set_input_from_db(0,persp_cam);
  bstm_batch.set_input_float(1,u);
  bstm_batch.set_input_float(2,v);
  bstm_batch.set_input_float(3,t);
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  x=bstm_batch.get_output_float(id);
  (id, type) = bstm_batch.commit_output(1);
  y=bstm_batch.get_output_float(id);
  (id, type) = bstm_batch.commit_output(2);
  z=bstm_batch.get_output_float(id);
  return x,y,z;
  
# triangulates a list of cams and a list of points
def get_3d_from_cams( cams, points ):
    assert( len(cams) == len(points) and len(cams) > 1 )
    #list of points will just be [u1,v1,u2,v2...]
    ptlist = [] 
    for p in points:
        ptlist.append(p[0])
        ptlist.append(p[1])
    #list of cam ids (type will be checked in C++)
    camlist = []
    for cam in cams:
        camlist.append(cam.id)
    bstm_batch.init_process("vpglGenerate3dPointFromCamsProcess");
    bstm_batch.set_input_unsigned_array(0, camlist)
    bstm_batch.set_input_int_array(1, ptlist)
    bstm_batch.run_process();
    (id, type) = bstm_batch.commit_output(0);
    x=bstm_batch.get_output_float(id);
    (id, type) = bstm_batch.commit_output(1);
    y=bstm_batch.get_output_float(id);
    (id, type) = bstm_batch.commit_output(2);
    z=bstm_batch.get_output_float(id);
    return x,y,z;

# create a generic camera
def convert_to_generic_camera(cam_in, ni, nj, level=0):
    bstm_batch.init_process('vpglConvertToGenericCameraProcess')
    bstm_batch.set_input_from_db(0,cam_in)
    bstm_batch.set_input_unsigned(1,ni)
    bstm_batch.set_input_unsigned(2,nj)
    bstm_batch.set_input_unsigned(3,level)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    generic_cam = dbvalue(id,type)
    return generic_cam
    

# randomly sample a camera rotated around principle axis
def perturb_camera(cam_in, angle, rng):
    bstm_batch.init_process('vpglPerturbPerspCamOrientProcess')
    bstm_batch.set_input_from_db(0,cam_in)
    bstm_batch.set_input_float(1,angle)
    bstm_batch.set_input_from_db(2,rng)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    pert_cam = dbvalue(id,type)
    (theta_id,type) = bstm_batch.commit_output(1)
    (phi_id,type) = bstm_batch.commit_output(2)
    theta = bstm_batch.get_output_float(theta_id);
    phi = bstm_batch.get_output_float(phi_id);
    return pert_cam, theta, phi
    
def write_perspective_cam_vrml(vrml_filename, pcam, camera_rad, axis_length, r, g, b):
    bstm_batch.init_process("bvrmlWritePerspectiveCamProcess");
    bstm_batch.set_input_string(0,vrml_filename);
    bstm_batch.set_input_from_db(1,pcam);
    bstm_batch.set_input_float(2,camera_rad);
    bstm_batch.set_input_float(3,axis_length);
    bstm_batch.set_input_float(4,r);
    bstm_batch.set_input_float(5,g);
    bstm_batch.set_input_float(6,b);
    bstm_batch.run_process();
    
# rotate a camera around principle axis
def rotate_perspective_camera(cam_in, theta, phi):
    bstm_batch.init_process('vpglRotatePerspCamProcess')
    bstm_batch.set_input_from_db(0,cam_in)
    bstm_batch.set_input_float(1,theta)
    bstm_batch.set_input_float(2,phi)
    bstm_batch.run_process()
    (id,type) = bstm_batch.commit_output(0)
    rot_cam = dbvalue(id,type)
    return rot_cam

def get_perspective_cam_center(pcam):
    bstm_batch.init_process("vpglGetPerspectiveCamCenterProcess");
    bstm_batch.set_input_from_db(0, pcam);
    bstm_batch.run_process()
    (x_id,x_type) = bstm_batch.commit_output(0)
    x = bstm_batch.get_output_float(x_id);
    (y_id,type) = bstm_batch.commit_output(1)
    y = bstm_batch.get_output_float(y_id);
    (z_id,type) = bstm_batch.commit_output(2)
    z = bstm_batch.get_output_float(z_id);
    return x, y, z
    
def create_perspective_camera2(pcam, cent_x, cent_y, cent_z):
    bstm_batch.init_process("vpglCreatePerspectiveCameraProcess2");
    bstm_batch.set_input_from_db(0, pcam);
    bstm_batch.set_input_float(1, cent_x);
    bstm_batch.set_input_float(2, cent_y);
    bstm_batch.set_input_float(3, cent_z);
    bstm_batch.run_process()
    (c_id,c_type) = bstm_batch.commit_output(0)
    cam = dbvalue(c_id,c_type);
    return cam
    
def create_perspective_camera_with_rot(pcam, phi, theta, cent_x, cent_y, cent_z):
    bstm_batch.init_process("vpglCreatePerspectiveCameraProcess3");
    bstm_batch.set_input_from_db(0, pcam);
    bstm_batch.set_input_float(1, phi);
    bstm_batch.set_input_float(2, theta);
    bstm_batch.set_input_float(3, cent_x);
    bstm_batch.set_input_float(4, cent_y);
    bstm_batch.set_input_float(5, cent_z);
    bstm_batch.run_process()
    (c_id,c_type) = bstm_batch.commit_output(0)
    cam = dbvalue(c_id,c_type);
    return cam
    
