import brl_init
import bvpl_batch as batch
dbvalue = brl_init.register_batch(batch)

# first creat an empty world.
print("Load Voxel World")
batch.init_process("bvxmLoadGridProcess")
batch.set_input_string(
    0, "D:/vj/scripts/Lidar_edges/ocp_bin_0_scale_0.vox")
batch.set_input_string(1, "float")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
world = dbvalue(world_id, world_type)

print("Crop Voxel World")
batch.init_process("bvxmCropGridProcess")
batch.set_params_process(
    "D:/vj/scripts/Lidar_edges/bvxmCropGridProcess.xml")
batch.set_input_from_db(0, world)
batch.run_process()
(crop_world_id, crop_world_type) = batch.commit_output(0)
crop_world = dbvalue(crop_world_id, crop_world_type)

print("Writing World")
batch.init_process("bvxmSaveGridRawProcess")
batch.set_input_from_db(0, crop_world)
batch.set_input_string(1, "D:/vj/scripts/Lidar_edges/crop_world.raw")
batch.set_input_string(2, "float")
batch.run_process()
