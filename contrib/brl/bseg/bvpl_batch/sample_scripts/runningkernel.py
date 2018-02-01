import brl_init
import bvpl_batch as batch
dbvalue = brl_init.register_batch(batch)

# first creat an empty world.
print("Load Voxel World")
batch.init_process("bvxmLoadGridProcess")
batch.set_input_string(
    0, "D:/vj/scripts/Lidar_edges/ocp_200_100_0_250_200_100.vox")
batch.set_input_string(1, "float")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
world = dbvalue(world_id, world_type)

print("Creating 2D edge kernel")
batch.init_process("bvplCreateEdge2dKernelProcess")
batch.set_params_process("./edge2d_kernel_params.xml")
batch.run_process()
(kernel_id, kernel_type) = batch.commit_output(0)
kernel = dbvalue(kernel_id, kernel_type)

print("Running Operator")
batch.init_process("bvplNeighborhoodOperatorProcess")
batch.set_input_from_db(0, world)
batch.set_input_from_db(1, kernel)
batch.set_input_string(2, "float")
batch.set_input_string(3, "edge2d")
batch.set_input_string(4, "D:/vj/scripts/Lidar_edges/x_conv.vox")
batch.run_process()
(result_x_world_id, result_x_world_type) = batch.commit_output(0)
result_x_world = dbvalue(result_x_world_id, result_x_world_type)


print("Writing World")
batch.init_process("bvxmSaveGridRawProcess")
batch.set_input_from_db(0, result_x_world)
batch.set_input_string(1, "D:/vj/scripts/Lidar_edges/x_crop_world.raw")
batch.set_input_string(2, "float")
batch.run_process()
