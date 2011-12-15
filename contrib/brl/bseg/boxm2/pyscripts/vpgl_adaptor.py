from boxm2_register import boxm2_batch, dbvalue;
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

#Scale = (scale_u, scale_v), ppoint = (u,v), center = (x,y,z), look_pt = (x,y,z)
def create_perspective_camera( scale, ppoint, center, look_pt ) :
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
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

# returns cam center from azimuth (degrees), elevation (degrees), radius, look point
def get_camera_center( azimuth, elevation, radius, lookPt) :
  deg_to_rad = math.pi/180.0; 
  el = elevation*deg_to_rad
  az = azimuth*deg_to_rad;
  cx = radius*math.sin(el)*math.cos(az);
  cy = radius*math.sin(el)*math.sin(az);
  cz = radius*math.cos(el);
  center = (cx + lookPt[0], cy + lookPt[1], cz + lookPt[2]); 
  return center;

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

#################################################
# perspective go generic conversion
#################################################
def persp2gen(pcam, ni, nj) :
  boxm2_batch.init_process("vpglConvertToGenericCameraProcess");
  boxm2_batch.set_input_from_db(0, pcam);
  boxm2_batch.set_input_unsigned(1, ni);
  boxm2_batch.set_input_unsigned(2, nj);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  gcam = dbvalue(id,type);
  return gcam;

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

