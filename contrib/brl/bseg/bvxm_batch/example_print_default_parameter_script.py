"""
This is a sample script by Ozge C Ozcanli
02/19/2008
"""

import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

print("Print Default Params for Creating Voxel World");
# print the defaults for the process given by name
bvxm_batch.process_print_default_params("bvxmCreateVoxelWorldProcess", "D:/projects/lockheed-voxel-world/CreateVoxelWorldProcessDefaults.xml"); 

print("Print Default Params for Normalize Image Process");
bvxm_batch.process_print_default_params("bvxmNormalizeImageProcess", "D:/projects/lockheed-voxel-world/bvxmNormalizeImageProcessDefaults.xml"); 


print("Done");

 