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
bvpl_batch.set_input_string(0,"D:/vj/scripts/Lidar_edges/ocp_200_100_0_250_200_100.vox");
bvpl_batch.set_input_string(1,"float");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
world = dbvalue(world_id,world_type);

print("Creating 2D edge kernel");
bvpl_batch.init_process("bvplCreateEdge2dKernelProcess");
bvpl_batch.set_params_process("./edge2d_kernel_params.xml");
bvpl_batch.run_process();
(kernel_id,kernel_type)= bvpl_batch.commit_output(0);
kernel = dbvalue(kernel_id,kernel_type);

print("Running Operator");
bvpl_batch.init_process("bvplNeighborhoodOperatorProcess");
bvpl_batch.set_input_from_db(0,world );
bvpl_batch.set_input_from_db(1,kernel);
bvpl_batch.set_input_string(2,"float");
bvpl_batch.set_input_string(3,"edge2d");
bvpl_batch.set_input_string(4,"D:/vj/scripts/Lidar_edges/x_conv.vox");
bvpl_batch.run_process();
(result_x_world_id,result_x_world_type)= bvpl_batch.commit_output(0);
result_x_world = dbvalue(result_x_world_id,result_x_world_type);


print("Writing World");
bvpl_batch.init_process("bvxmSaveGridRawProcess");
bvpl_batch.set_input_from_db(0,result_x_world);
bvpl_batch.set_input_string(1,"D:/vj/scripts/Lidar_edges/x_crop_world.raw");
bvpl_batch.set_input_string(2,"float");
bvpl_batch.run_process();

