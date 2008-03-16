import bvxm_batch
bvam_batch.register_processes();
bvam_batch.register_datatypes();
# print the defaults for the process given by name

bvam_batch.process_print_default_params("CreateVoxelWorldProcess", "./world_model_params.xml"); 
bvam_batch.process_print_default_params("BvamRoiInitProcess", "./roi_params.xml"); 
bvam_batch.process_print_default_params("BvamGenerateEdgeMapProcess", "./edge_map_params.xml"); 
bvam_batch.process_print_default_params("BvamRpcRegistrationProcess", "./rpc_registration_parameters.xml");
bvam_batch.process_print_default_params("BvamNormalizeImageProcess", "./normalize.xml"); 
bvam_batch.process_print_default_params("BvamChangeDetectionDisplayProcess", "./change_display_params.xml"); 


print("Done");
