from boxm2_register import boxm2_batch, dbvalue;

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

def load_rational_camera(file_path) :
  boxm2_batch.init_process("vpglLoadRationalCameraProcess")
  boxm2_batch.set_input_string(0, file_path)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  cam = dbvalue(id,type)
  return cam

def load_rational_camera_nitf(file_path) :
  boxm2_batch.init_process("vpglLoadRationalCameraNITFProcess")
  boxm2_batch.set_input_string(0, file_path)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  cam = dbvalue(id,type)
  return cam

###################
#camera saving
###################
def save_rational_camera(camera,path) :
  boxm2_batch.init_process("vpglSaveRationalCameraProcess")
  boxm2_batch.set_input_from_db(0,camera)
  boxm2_batch.set_input_string(1,path)
  boxm2_batch.run_process()

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

