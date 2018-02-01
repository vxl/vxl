import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_input_string(0, "D:/dec/matlab/reg3d/batch_test")
batch.run_process()
world_id = batch.commit_output(0)


import glob
image_fnames = glob.glob("F:/dec/helicopter/sequences/seq2/images/*.png")
camera_fnames = glob.glob("F:/dec/helicopter/sequences/seq2/cameras/*.txt")

for i in range(0, 100, 25):
    print("Loading Camera")
    batch.init_process("bvxmLoadProjCameraProcess")
    batch.set_input_string(0, camera_fnames[i])
    batch.run_process()
    cam_id = batch.commit_output(0)

    print("Loading Image")
    batch.init_process("bvxmLoadImageViewProcess")
    batch.set_input_string(0, image_fnames[i])
    batch.run_process()
    image_id = batch.commit_output(0)

    print("Updating World")
    batch.init_process("bvxmUpdateProcess")
    batch.set_input_from_db(0, image_id)
    batch.set_input_from_db(1, cam_id)
    batch.set_input_from_db(2, world_id)
    batch.run_process()
    prob_img_id = batch.commit_output(0)

print("Done")
