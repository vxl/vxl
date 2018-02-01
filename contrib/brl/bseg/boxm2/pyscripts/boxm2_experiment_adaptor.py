import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)
import os
#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################

# Print ocl info for all devices


def update_auxQ(scene, device, opencl_cache, cam, img, ident="", view_ident=""):
    #print("Init Manager");
    batch.init_process("boxm2OclUpdateAuxQPerViewProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_from_db(4, img)
    batch.set_input_string(5, ident)
    batch.set_input_string(6, view_ident)
    status = batch.run_process()
    if(status != True):
        print "Error in Running Aux Q "


def update_PusingQ(scene, device, opencl_cache, operation=2, view_ident=""):
    #print("Init Manager");
    batch.init_process("boxm2OclUpdateUsingQProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_int(3, operation)
    batch.set_input_string(4, view_ident)
    status = batch.run_process()
    if(status != True):
        print "Error in Running Aux Q "
