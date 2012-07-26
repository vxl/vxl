from boxm2_register import boxm2_batch, dbvalue;
#import math, numpy
import math

###################
#camera loading
###################
def load_perspective_camera(file_path) :
  boxm2_batch.init_process("vpglLoadPerspectiveCameraProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z), up = (x,y,z)
def create_perspective_camera( scale, ppoint, center, look_pt, up ) :
  boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess");
  boxm2_batch.set_input_double(0, scale[0]);
  boxm2_batch.set_input_double(1, ppoint[0]);
  boxm2_batch.set_input_double(2, scale[1]);
  boxm2_batch.set_input_double(3, ppoint[1]);
  boxm2_batch.set_input_double(4, center[0]);
  boxm2_batch.set_input_double(5, center[1]);
  boxm2_batch.set_input_double(6, center[2]);
  boxm2_batch.set_input_double(7, look_pt[0]);
  boxm2_batch.set_input_double(8, look_pt[1]);
  boxm2_batch.set_input_double(9, look_pt[2]);
  boxm2_batch.set_input_double(10, up[0]);
  boxm2_batch.set_input_double(11, up[1]);
  boxm2_batch.set_input_double(12, up[2]);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

#resize a camera from size0 =(ni,nj) to size1 (ni_1, nj_1)
def resample_perspective_camera( cam, size0, size1 ):
  boxm2_batch.init_process("vpglResamplePerspectiveCameraProcess")
  boxm2_batch.set_input_from_db(0, cam) 
  boxm2_batch.set_input_int(1, size0[0])
  boxm2_batch.set_input_int(2, size0[1])
  boxm2_batch.set_input_int(3, size1[0])
  boxm2_batch.set_input_int(4, size1[1])
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
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
  boxm2_batch.init_process("vpglLoadRationalCameraProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def load_rational_camera_nitf(file_path) :
  boxm2_batch.init_process("vpglLoadRationalCameraNITFProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;
  
def load_local_rational_camera(file_path) :
  boxm2_batch.init_process("vpglLoadLocalRationalCameraProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def create_local_rational_camera(rational_cam_fname, lvcs_fname):
    boxm2_batch.init_process('vpglCreateLocalRationalCameraProcess')
    boxm2_batch.set_input_string(0,rational_cam_fname)
    boxm2_batch.set_input_string(1,lvcs_fname)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    cam = dbvalue(id,type)
    return cam

###################
#camera saving
###################
def save_rational_camera(camera,path) :
  boxm2_batch.init_process("vpglSaveRationalCameraProcess");
  boxm2_batch.set_input_from_db(0,camera);
  boxm2_batch.set_input_string(1,path);
  boxm2_batch.run_process();
def save_perspective_camera(camera,path) :
  boxm2_batch.init_process("vpglSavePerspectiveCameraProcess");
  boxm2_batch.set_input_from_db(0,camera);
  boxm2_batch.set_input_string(1,path);
  boxm2_batch.run_process();  
  
#################################################
# perspective go generic conversion
#################################################
def persp2gen(pcam, ni, nj, level=0) :
  boxm2_batch.init_process("vpglConvertToGenericCameraProcess");
  boxm2_batch.set_input_from_db(0, pcam);
  boxm2_batch.set_input_unsigned(1, ni);
  boxm2_batch.set_input_unsigned(2, nj);
  boxm2_batch.set_input_unsigned(3, level);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  gcam = dbvalue(id,type);
  return gcam;
  
def persp2genWmargin(pcam, ni, nj, margin, level=0) :
  boxm2_batch.init_process("vpglConvertToGenericCameraWithMarginProcess");
  boxm2_batch.set_input_from_db(0, pcam);
  boxm2_batch.set_input_unsigned(1, ni);
  boxm2_batch.set_input_unsigned(2, nj);
  boxm2_batch.set_input_unsigned(3, level);
  boxm2_batch.set_input_int(4, margin);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  gcam = dbvalue(id,type);
  (id,type) = boxm2_batch.commit_output(1);
  ni = boxm2_batch.get_output_unsigned(id);
  (id,type) = boxm2_batch.commit_output(2);
  nj = boxm2_batch.get_output_unsigned(id);
  (id,type) = boxm2_batch.commit_output(3);
  new_pers_cam = dbvalue(id,type);
  return (gcam, ni, nj, new_pers_cam);

#gets bounding box from a directory of cameras... (incomplete)_
def camera_dir_planar_bbox(dir_name) :
  boxm2_batch.init_process("vpglGetBoundingBoxProcess");
  boxm2_batch.set_input_string(0, dir_name);
  boxm2_batch.run_process();

def project_point(camera,x,y,z):
    boxm2_batch.init_process('vpglProjectProcess')
    boxm2_batch.set_input_from_db(0,camera)
    boxm2_batch.set_input_float(1,x)
    boxm2_batch.set_input_float(2,y)
    boxm2_batch.set_input_float(3,z)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    u = boxm2_batch.get_output_float(id)
    (id,type) = boxm2_batch.commit_output(1)
    v = boxm2_batch.get_output_float(id)
    return (u,v)

# gets view direction at a point for a perspective camera
def get_view_at_point(persp_cam,x,y,z):
  boxm2_batch.init_process("vpglGetViewDirectionAtPointProcess");
  boxm2_batch.set_input_from_db(0,persp_cam);
  boxm2_batch.set_input_float(1,x);
  boxm2_batch.set_input_float(2,y);
  boxm2_batch.set_input_float(3,z);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  theta=boxm2_batch.get_output_float(id);
  (id,type) = boxm2_batch.commit_output(1);
  phi=boxm2_batch.get_output_float(id);
  return theta, phi;

def get_3d_from_depth(persp_cam,u,v,t) :
  boxm2_batch.init_process("vpglGenerate3dPointFromDepthProcess");
  boxm2_batch.set_input_from_db(0,persp_cam);
  boxm2_batch.set_input_float(1,u);
  boxm2_batch.set_input_float(2,v);
  boxm2_batch.set_input_float(3,t);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  x=boxm2_batch.get_output_float(id);
  (id, type) = boxm2_batch.commit_output(1);
  y=boxm2_batch.get_output_float(id);
  (id, type) = boxm2_batch.commit_output(2);
  z=boxm2_batch.get_output_float(id);
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
    boxm2_batch.init_process("vpglGenerate3dPointFromCamsProcess");
    boxm2_batch.set_input_unsigned_array(0, camlist)
    boxm2_batch.set_input_int_array(1, ptlist)
    boxm2_batch.run_process();
    (id, type) = boxm2_batch.commit_output(0);
    x=boxm2_batch.get_output_float(id);
    (id, type) = boxm2_batch.commit_output(1);
    y=boxm2_batch.get_output_float(id);
    (id, type) = boxm2_batch.commit_output(2);
    z=boxm2_batch.get_output_float(id);
    return x,y,z;

# create a generic camera
def convert_to_generic_camera(cam_in, ni, nj, level=0):
    boxm2_batch.init_process('vpglConvertToGenericCameraProcess')
    boxm2_batch.set_input_from_db(0,cam_in)
    boxm2_batch.set_input_unsigned(1,ni)
    boxm2_batch.set_input_unsigned(2,nj)
    boxm2_batch.set_input_unsigned(3,level)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    generic_cam = dbvalue(id,type)
    return generic_cam
    
# create a generic camera from a local rational with user-specified z range
def convert_local_rational_to_generic(cam_in, ni, nj, min_z, max_z, level=0):
    boxm2_batch.init_process('vpglConvertLocalRationalToGenericProcess')
    boxm2_batch.set_input_from_db(0,cam_in)
    boxm2_batch.set_input_unsigned(1,ni)
    boxm2_batch.set_input_unsigned(2,nj)
    boxm2_batch.set_input_float(3,min_z)
    boxm2_batch.set_input_float(4,max_z)
    boxm2_batch.set_input_unsigned(5,level)
    if not boxm2_batch.run_process():
        return None
    (id,type) = boxm2_batch.commit_output(0)
    generic_cam = dbvalue(id,type)
    return generic_cam

# correct a rational camera
def correct_rational_camera(cam_in, offset_x, offset_y):
    boxm2_batch.init_process('vpglCorrectRationalCameraProcess')
    boxm2_batch.set_input_from_db(0,cam_in)
    boxm2_batch.set_input_double(1,offset_x)
    boxm2_batch.set_input_double(2,offset_y)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    corrected_cam = dbvalue(id,type)
    return corrected_cam

# convert lat,lon,el to local coordinates
def convert_to_local_coordinates(lvcs_filename,lat,lon,el):
    boxm2_batch.init_process('vpglConvertToLocalCoordinatesProcess')
    boxm2_batch.set_input_string(0,lvcs_filename)
    boxm2_batch.set_input_float(1,lat)
    boxm2_batch.set_input_float(2,lon)
    boxm2_batch.set_input_float(3,el)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id,type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    (id,type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(id)
    boxm2_batch.remove_data(id)
    return (x,y,z)

# randomly sample a camera rotated around principle axis
def perturb_camera(cam_in, angle, rng):
    boxm2_batch.init_process('vpglPerturbPerspCamOrientProcess')
    boxm2_batch.set_input_from_db(0,cam_in)
    boxm2_batch.set_input_float(1,angle)
    boxm2_batch.set_input_from_db(2,rng)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    pert_cam = dbvalue(id,type)
    (theta_id,type) = boxm2_batch.commit_output(1)
    (phi_id,type) = boxm2_batch.commit_output(2)
    theta = boxm2_batch.get_output_float(theta_id);
    phi = boxm2_batch.get_output_float(phi_id);
    return pert_cam, theta, phi
    
def write_perspective_cam_vrml(vrml_filename, pcam, camera_rad, axis_length, r, g, b):
    boxm2_batch.init_process("bvrmlWritePerspectiveCamProcess");
    boxm2_batch.set_input_string(0,vrml_filename);
    boxm2_batch.set_input_from_db(1,pcam);
    boxm2_batch.set_input_float(2,camera_rad);
    boxm2_batch.set_input_float(3,axis_length);
    boxm2_batch.set_input_float(4,r);
    boxm2_batch.set_input_float(5,g);
    boxm2_batch.set_input_float(6,b);
    boxm2_batch.run_process();
    
# rotate a camera around principle axis
def rotate_perspective_camera(cam_in, theta, phi):
    boxm2_batch.init_process('vpglRotatePerspCamProcess')
    boxm2_batch.set_input_from_db(0,cam_in)
    boxm2_batch.set_input_float(1,theta)
    boxm2_batch.set_input_float(2,phi)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    rot_cam = dbvalue(id,type)
    return rot_cam

def get_perspective_cam_center(pcam):
    boxm2_batch.init_process("vpglGetPerspectiveCamCenterProcess");
    boxm2_batch.set_input_from_db(0, pcam);
    boxm2_batch.run_process()
    (x_id,x_type) = boxm2_batch.commit_output(0)
    x = boxm2_batch.get_output_float(x_id);
    (y_id,type) = boxm2_batch.commit_output(1)
    y = boxm2_batch.get_output_float(y_id);
    (z_id,type) = boxm2_batch.commit_output(2)
    z = boxm2_batch.get_output_float(z_id);
    return x, y, z
    
def create_perspective_camera2(pcam, cent_x, cent_y, cent_z):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess2");
    boxm2_batch.set_input_from_db(0, pcam);
    boxm2_batch.set_input_float(1, cent_x);
    boxm2_batch.set_input_float(2, cent_y);
    boxm2_batch.set_input_float(3, cent_z);
    boxm2_batch.run_process()
    (c_id,c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id,c_type);
    return cam
    
def create_perspective_camera_with_rot(pcam, phi, theta, cent_x, cent_y, cent_z):
    boxm2_batch.init_process("vpglCreatePerspectiveCameraProcess3");
    boxm2_batch.set_input_from_db(0, pcam);
    boxm2_batch.set_input_float(1, phi);
    boxm2_batch.set_input_float(2, theta);
    boxm2_batch.set_input_float(3, cent_x);
    boxm2_batch.set_input_float(4, cent_y);
    boxm2_batch.set_input_float(5, cent_z);
    boxm2_batch.run_process()
    (c_id,c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id,c_type);
    return cam
    
def get_nitf_footprint(nitf_list_filename, out_kml_filename):
    boxm2_batch.init_process('vpglNITFFootprintProcess')
    boxm2_batch.set_input_string(0,nitf_list_filename);
    boxm2_batch.set_input_string(1,out_kml_filename);
    boxm2_batch.run_process()
    
def get_geocam_footprint(geocam, geotiff_filename, out_kml_filename,init_finish=True):
    boxm2_batch.init_process('vpglGeoFootprintProcess')
    boxm2_batch.set_input_from_db(0,geocam);
    boxm2_batch.set_input_string(1,geotiff_filename);
    boxm2_batch.set_input_string(2,out_kml_filename);
    boxm2_batch.set_input_bool(3,init_finish);
    boxm2_batch.run_process()

def load_geotiff_cam(tfw_filename, lvcs, utm_zone, utm_hemisphere):
    boxm2_batch.init_process("vpglLoadGeoCameraProcess");
    boxm2_batch.set_input_string(0, tfw_filename);
    boxm2_batch.set_input_from_db(1, lvcs);
    boxm2_batch.set_input_int(2, utm_zone);
    boxm2_batch.set_input_unsigned(3, utm_hemisphere);
    boxm2_batch.run_process()
    (c_id,c_type) = boxm2_batch.commit_output(0)
    cam = dbvalue(c_id,c_type);
    return cam

def translate_geo_camera(geocam, x, y):
    boxm2_batch.init_process("vpglTranslateGeoCameraProcess");
    boxm2_batch.set_input_from_db(0, geocam);
    boxm2_batch.set_input_double(1, x);
    boxm2_batch.set_input_double(2, y);
    boxm2_batch.run_process();
    (c_id, c_type) = boxm2_batch.commit_output(0);
    cam = dbvalue(c_id, c_type);
    return cam

def geo2generic(geocam, ni, nj, scene_height, level):
    boxm2_batch.init_process("vpglConvertGeoCameraToGenericProcess");
    boxm2_batch.set_input_from_db(0, geocam);
    boxm2_batch.set_input_int(1, ni);
    boxm2_batch.set_input_int(2, nj);
    boxm2_batch.set_input_double(3, scene_height);
    boxm2_batch.set_input_int(4, level);
    boxm2_batch.run_process();
    (c_id, c_type) = boxm2_batch.commit_output(0);
    cam = dbvalue(c_id, c_type);
    return cam
    



