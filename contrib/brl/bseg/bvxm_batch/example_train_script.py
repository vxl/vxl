import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

print("Creating Voxel World");
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
bvxm_batch.set_input_string(0,"d:/dec/matlab/reg3d/batch_test");
bvxm_batch.run_process();
world_id = bvxm_batch.commit_output(0);


import glob
image_fnames = glob.glob("f:/dec/helicopter/sequences/seq2/images/*.png");
camera_fnames = glob.glob("f:/dec/helicopter/sequences/seq2/cameras/*.txt");

for i in range(0,100,25):
  print("Loading Camera");
  bvxm_batch.init_process("bvxmLoadProjCameraProcess");
  bvxm_batch.set_input_string(0,camera_fnames[i]);
  bvxm_batch.run_process();
  cam_id = bvxm_batch.commit_output(0);
 
  print("Loading Image");
  bvxm_batch.init_process("bvxmLoadImageViewProcess");
  bvxm_batch.set_input_string(0,image_fnames[i]);
  bvxm_batch.run_process();
  image_id = bvxm_batch.commit_output(0);
 
  print("Updating World");
  bvxm_batch.init_process("bvxmUpdateProcess");
  bvxm_batch.set_input_from_db(0,image_id);
  bvxm_batch.set_input_from_db(1,cam_id);
  bvxm_batch.set_input_from_db(2,world_id);
  bvxm_batch.run_process();
  prob_img_id = bvxm_batch.commit_output(0);

print("Done");

 