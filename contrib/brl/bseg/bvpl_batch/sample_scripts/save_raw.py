import bvpl_batch
bvpl_batch.register_processes();
bvpl_batch.register_datatypes();


class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

#first creat an empty world.
print("Load Voxel World");
bvpl_batch.init_process("bvxmLoadGridProcess");
bvpl_batch.set_input_string(0,"D:/vj/scripts/Lidar_edges/ocp_bin_0_scale_0.vox");
bvpl_batch.set_input_string(1,"float");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
world = dbvalue(world_id,world_type);

print("Crop Voxel World");
bvpl_batch.init_process("bvxmCropGridProcess");
bvpl_batch.set_params_process("D:/vj/scripts/Lidar_edges/bvxmCropGridProcess.xml");
bvpl_batch.set_input_from_db(0,world);
bvpl_batch.run_process();
(crop_world_id,crop_world_type)= bvpl_batch.commit_output(0);
crop_world = dbvalue(crop_world_id,crop_world_type);

print("Writing World");
bvpl_batch.init_process("bvxmSaveGridRawProcess");
bvpl_batch.set_input_from_db(0,crop_world);
bvpl_batch.set_input_string(1,"D:/vj/scripts/Lidar_edges/crop_world.raw");
bvpl_batch.set_input_string(2,"float");
bvpl_batch.run_process();

