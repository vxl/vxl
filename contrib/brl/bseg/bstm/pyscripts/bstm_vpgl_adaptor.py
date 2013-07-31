from bstm_register import bstm_batch, dbvalue;
#import math, numpy
import math
from glob import glob

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

