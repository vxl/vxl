import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();
# print the defaults for the process given by name

bvxm_batch.process_print_default_params("CreateVoxelWorldProcess", "./world_model_params.xml"); 
bvxm_batch.process_print_default_params("BvamRoiInitProcess", "./roi_params.xml"); 
bvxm_batch.process_print_default_params("BvamGenerateEdgeMapProcess", "./edge_map_params.xml"); 
bvxm_batch.process_print_default_params("BvamRpcRegistrationProcess", "./rpc_registration_parameters.xml");
bvxm_batch.process_print_default_params("BvamNormalizeImageProcess", "./normalize.xml"); 
bvxm_batch.process_print_default_params("BvamChangeDetectionDisplayProcess", "./change_display_params.xml"); 


print("Done");
