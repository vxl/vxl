import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)
# print the defaults for the process given by name

batch.process_print_default_params(
    "CreateVoxelWorldProcess", "./world_model_params.xml")
batch.process_print_default_params(
    "BvamRoiInitProcess", "./roi_params.xml")
batch.process_print_default_params(
    "BvamGenerateEdgeMapProcess", "./edge_map_params.xml")
batch.process_print_default_params(
    "BvamRpcRegistrationProcess", "./rpc_registration_parameters.xml")
batch.process_print_default_params(
    "BvamNormalizeImageProcess", "./normalize.xml")
batch.process_print_default_params(
    "BvamChangeDetectionDisplayProcess", "./change_display_params.xml")


print("Done")
