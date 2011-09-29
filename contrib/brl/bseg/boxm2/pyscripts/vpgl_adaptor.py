from boxm2_register import boxm2_batch, dbvalue; 

###################
#camera loading
###################
def load_camera(file_path) : 
  boxm2_batch.init_process("vpglLoadPerspectiveCameraProcess"); 
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam; 
  
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
