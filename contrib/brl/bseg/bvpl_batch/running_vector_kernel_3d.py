#############################################################################
## Created by Vishal Jain
## Nov 10, 2009
## LEMS, Brown University
#############################################################################

import bvpl_batch
bvpl_batch.register_processes();
bvpl_batch.register_datatypes();


class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

#first creat an empty world.
world_dir="D:/tests/lidar_edge_reg/";
cwd_dir="D:/tests/lidar_edge_reg/";

print("Load Voxel World");
bvpl_batch.init_process("bvxmLoadGridProcess");
bvpl_batch.set_input_string(0,world_dir+"ocp_opinion_bin_0_scale_0.vox");
bvpl_batch.set_input_string(1,"ocp_opinion");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
world = dbvalue(world_id,world_type);

print("Creating 3D edge kernel");
bvpl_batch.init_process("bvplCreateEdge3dKernelVectorProcess");
bvpl_batch.set_input_unsigned(0,2);
bvpl_batch.set_input_unsigned(1,5);
bvpl_batch.set_input_unsigned(2,5);
bvpl_batch.run_process();
(kernel_id,kernel_type)= bvpl_batch.commit_output(0);
kernel = dbvalue(kernel_id,kernel_type);

print("Running Operator");
bvpl_batch.init_process("bvplVectorOperatorProcess");
bvpl_batch.set_input_from_db(0,world );
bvpl_batch.set_input_from_db(1,kernel);
bvpl_batch.set_input_string(2,"opinion");
bvpl_batch.set_input_string(3,"edge_algebraic_mean");
bvpl_batch.set_input_string(4,cwd_dir+"response.vox");
bvpl_batch.set_input_string(5,cwd_dir+"id_orientation.vox");
bvpl_batch.run_process();
(id,type)= bvpl_batch.commit_output(0);
response_world = dbvalue(id,type);
(id,type)= bvpl_batch.commit_output(1);
id_orientation = dbvalue(id,type);

# this is for visulaization
print("Convert Voxel World");
bvpl_batch.init_process("bvxmExpectationOpinionGridProcess");
bvpl_batch.set_input_from_db(0,response_world);
bvpl_batch.set_input_string(1,cwd_dir+"response_float.vox");
bvpl_batch.set_input_string(2,"expectation");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
response_float = dbvalue(world_id,world_type);

# compute 3d edges
##################
print("Creating 3D edge kernel");
bvpl_batch.init_process("bvplCreateEdge3dKernelVectorProcess");
bvpl_batch.set_input_unsigned(0,4);
bvpl_batch.set_input_unsigned(1,7);
bvpl_batch.set_input_unsigned(2,7);
bvpl_batch.run_process();
(kernel_id,kernel_type)= bvpl_batch.commit_output(0);
kernel_edge = dbvalue(kernel_id,kernel_type);

print("Running Operator");
bvpl_batch.init_process("bvplSusanOpinionOperatorProcess");
bvpl_batch.set_input_from_db(0,id_orientation );
bvpl_batch.set_input_from_db(1,response_world);
bvpl_batch.set_input_string(2,cwd_dir+"edges3d_data_4_7_7.vox");
bvpl_batch.set_input_from_db(3,kernel);
bvpl_batch.run_process();
(id,type)= bvpl_batch.commit_output(0);
edges3d = dbvalue(id,type);

print("Convert Voxel World");
bvpl_batch.init_process("bvxmExpectationOpinionGridProcess");
bvpl_batch.set_input_from_db(0,edges3d);
bvpl_batch.set_input_string(1,cwd_dir+"edges_bin_0_scale_0.vox");
bvpl_batch.set_input_string(2,"belief");
bvpl_batch.run_process();
(world_id,world_type)= bvpl_batch.commit_output(0);
edges3d_exp = dbvalue(world_id,world_type);

