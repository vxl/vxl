"""
This is a sample script by Ozge C Ozcanli
02/19/2008
"""

import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

print("Creating Voxel World");
# clon CreateVoxelWorldProcess and make it the current process
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
# call the parse method of the current process
bvxm_batch.set_params_process("D:/projects/lockheed-voxel-world/CreateVoxelWorldProcess.xml");
# run the current process
bvxm_batch.run_process();
world_id = bvxm_batch.commit_output(0);

import glob
image_fnames = glob.glob("Z:/video/ieden/image_pvd_helicopter/seq2/images/*.png");
camera_fnames = glob.glob("Z:/video/ieden/image_pvd_helicopter/seq2/cameras/*.txt");

print("Loading Camera");
bvxm_batch.init_process("bvxmLoadProjCameraProcess");
bvxm_batch.set_input_string(0,camera_fnames[1]);
bvxm_batch.run_process();
cam_id = bvxm_batch.commit_output(0);
 
print("Loading Image");
bvxm_batch.init_process("bvxmLoadImageViewProcess");
bvxm_batch.set_input_string(0,image_fnames[1]);
bvxm_batch.run_process();
image_id = bvxm_batch.commit_output(0);
 
print("Normalize Image");
bvxm_batch.init_process("bvxmNormalizeImageProcess");
bvxm_batch.set_params_process("D:/projects/lockheed-voxel-world/bvxmNormalizeImageProcess.xml");
bvxm_batch.set_input_from_db(0,image_id);
bvxm_batch.set_input_from_db(1,cam_id);
bvxm_batch.set_input_from_db(2,world_id);
bvxm_batch.run_process();
prob_img_id = bvxm_batch.commit_output(0);

print("Done");

 