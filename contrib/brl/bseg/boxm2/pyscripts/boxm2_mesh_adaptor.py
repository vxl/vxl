from boxm2_register import boxm2_batch, dbvalue;

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
