#############################################################################
# Created by Vishal Jain
# Nov 10, 2009
# LEMS, Brown University
#############################################################################

import brl_init
import bvpl_batch as batch
dbvalue = brl_init.register_batch(batch)

# first creat an empty world.
world_dir = "D:/tests/lidar_edge_reg/"
cwd_dir = "D:/tests/lidar_edge_reg/"

print("Load Voxel World")
batch.init_process("bvxmLoadGridProcess")
batch.set_input_string(0, world_dir + "ocp_opinion_bin_0_scale_0.vox")
batch.set_input_string(1, "ocp_opinion")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
world = dbvalue(world_id, world_type)

print("Creating 3D edge kernel")
batch.init_process("bvplCreateEdge3dKernelVectorProcess")
batch.set_input_unsigned(0, 2)
batch.set_input_unsigned(1, 5)
batch.set_input_unsigned(2, 5)
batch.run_process()
(kernel_id, kernel_type) = batch.commit_output(0)
kernel = dbvalue(kernel_id, kernel_type)

print("Running Operator")
batch.init_process("bvplVectorOperatorProcess")
batch.set_input_from_db(0, world)
batch.set_input_from_db(1, kernel)
batch.set_input_string(2, "opinion")
batch.set_input_string(3, "edge_algebraic_mean")
batch.set_input_string(4, cwd_dir + "response.vox")
batch.set_input_string(5, cwd_dir + "id_orientation.vox")
batch.run_process()
(id, type) = batch.commit_output(0)
response_world = dbvalue(id, type)
(id, type) = batch.commit_output(1)
id_orientation = dbvalue(id, type)

# this is for visulaization
print("Convert Voxel World")
batch.init_process("bvxmExpectationOpinionGridProcess")
batch.set_input_from_db(0, response_world)
batch.set_input_string(1, cwd_dir + "response_float.vox")
batch.set_input_string(2, "expectation")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
response_float = dbvalue(world_id, world_type)

# compute 3d edges
##################
print("Creating 3D edge kernel")
batch.init_process("bvplCreateEdge3dKernelVectorProcess")
batch.set_input_unsigned(0, 4)
batch.set_input_unsigned(1, 7)
batch.set_input_unsigned(2, 7)
batch.run_process()
(kernel_id, kernel_type) = batch.commit_output(0)
kernel_edge = dbvalue(kernel_id, kernel_type)

print("Running Operator")
batch.init_process("bvplSusanOpinionOperatorProcess")
batch.set_input_from_db(0, id_orientation)
batch.set_input_from_db(1, response_world)
batch.set_input_string(2, cwd_dir + "edges3d_data_4_7_7.vox")
batch.set_input_from_db(3, kernel)
batch.run_process()
(id, type) = batch.commit_output(0)
edges3d = dbvalue(id, type)

print("Convert Voxel World")
batch.init_process("bvxmExpectationOpinionGridProcess")
batch.set_input_from_db(0, edges3d)
batch.set_input_string(1, cwd_dir + "edges_bin_0_scale_0.vox")
batch.set_input_string(2, "belief")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
edges3d_exp = dbvalue(world_id, world_type)
