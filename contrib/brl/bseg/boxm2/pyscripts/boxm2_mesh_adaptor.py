from boxm2_register import boxm2_batch, dbvalue;

def load_mesh(filename):
  boxm2_batch.init_process("boxm2LoadMeshProcess");
  boxm2_batch.set_input_string(0,filename);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  mesh = dbvalue(id, type);
  return mesh;
  
def gen_mesh(z_img,x_img,y_img,out_dir):
  boxm2_batch.init_process("boxm2ExportMeshProcess");
  boxm2_batch.set_input_from_db(0,z_img);
  boxm2_batch.set_input_from_db(1,x_img);
  boxm2_batch.set_input_from_db(2,y_img);
  boxm2_batch.set_input_string(3,out_dir);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  mesh = dbvalue(id, type);
  return mesh;

def gen_tex_mesh(mesh,in_img_dir, in_cam_dir,out_dir):
  boxm2_batch.init_process("boxm2TextureMeshProcess");
  boxm2_batch.set_input_from_db(0,mesh);
  boxm2_batch.set_input_string(1,in_img_dir);
  boxm2_batch.set_input_string(2,in_cam_dir);
  boxm2_batch.set_input_string(3,out_dir);
  boxm2_batch.run_process();

def gen_point_cloud(scene,cache,filename,thresh = 0.3,depth=3):
  boxm2_batch.init_process("boxm2ExtractPointCloudProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_float(2,thresh); #prob threshold
  boxm2_batch.set_input_unsigned(3,depth); #prob threshold
  boxm2_batch.run_process();

  boxm2_batch.init_process("boxm2ExportOrientedPointCloudProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_string(2,filename);
  boxm2_batch.set_input_float(4,0.5);
  boxm2_batch.run_process();
def gen_oriented_point_cloud(scene,cache,prob_t, norm_mag_t, filename):
  boxm2_batch.init_process("boxm2CppComputeDerivativeProcesses");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_float(2,prob_t);
  boxm2_batch.set_input_float(3,norm_mag_t);



def export_stack(scene,cache,outdir,isopacityonly= True):
  boxm2_batch.init_process("boxm2ExportStackImagesProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_string(2,outdir);
  boxm2_batch.set_input_bool(3,isopacityonly);
  boxm2_batch.run_process();

def paint_mesh(scene,cache,in_file,out_file):
  boxm2_batch.init_process("boxm2PaintMeshProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_string(2,in_file);
  boxm2_batch.set_input_string(3,out_file);
  boxm2_batch.run_process();
