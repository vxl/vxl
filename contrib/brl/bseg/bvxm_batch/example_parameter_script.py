"""
This is a sample script by Ozge C Ozcanli
02/19/2008
"""

import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

print("Creating Voxel World")
# clon CreateVoxelWorldProcess and make it the current process
batch.init_process("bvxmCreateVoxelWorldProcess")
# call the parse method of the current process
batch.set_params_process(
    "D:/projects/lockheed-voxel-world/CreateVoxelWorldProcess.xml")
# run the current process
batch.run_process()
world_id = batch.commit_output(0)

import glob
image_fnames = glob.glob(
    "Z:/video/ieden/image_pvd_helicopter/seq2/images/*.png")
camera_fnames = glob.glob(
    "Z:/video/ieden/image_pvd_helicopter/seq2/cameras/*.txt")

print("Loading Camera")
batch.init_process("bvxmLoadProjCameraProcess")
batch.set_input_string(0, camera_fnames[1])
batch.run_process()
cam_id = batch.commit_output(0)

print("Loading Image")
batch.init_process("bvxmLoadImageViewProcess")
batch.set_input_string(0, image_fnames[1])
batch.run_process()
image_id = batch.commit_output(0)

print("Normalize Image")
batch.init_process("bvxmNormalizeImageProcess")
batch.set_params_process(
    "D:/projects/lockheed-voxel-world/bvxmNormalizeImageProcess.xml")
batch.set_input_from_db(0, image_id)
batch.set_input_from_db(1, cam_id)
batch.set_input_from_db(2, world_id)
batch.run_process()
prob_img_id = batch.commit_output(0)

print("Done")
