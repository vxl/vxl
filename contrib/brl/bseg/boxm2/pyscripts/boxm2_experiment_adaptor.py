from boxm2_register import boxm2_batch, dbvalue;
import os
#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################

# Print ocl info for all devices
def update_auxQ(scene,device,opencl_cache,cam, img, ident="", view_ident=""):
  #print("Init Manager");
  boxm2_batch.init_process("boxm2OclUpdateAuxQPerViewProcess");
  boxm2_batch.set_input_from_db(0, device);
  boxm2_batch.set_input_from_db(1, scene);
  boxm2_batch.set_input_from_db(2, opencl_cache);
  boxm2_batch.set_input_from_db(3, cam);
  boxm2_batch.set_input_from_db(4, img);
  boxm2_batch.set_input_string(5, ident);
  boxm2_batch.set_input_string(6, view_ident);
  status = boxm2_batch.run_process();
  if(status != True):
	print "Error in Running Aux Q ";

def update_PusingQ(scene,device,opencl_cache,operation = 2,view_ident=""):
  #print("Init Manager");
  boxm2_batch.init_process("boxm2OclUpdateUsingQProcess");
  boxm2_batch.set_input_from_db(0, device);
  boxm2_batch.set_input_from_db(1, scene);
  boxm2_batch.set_input_from_db(2, opencl_cache);
  boxm2_batch.set_input_int(3, operation);
  boxm2_batch.set_input_string(4, view_ident);
  status = boxm2_batch.run_process();
  if(status != True):
	print "Error in Running Aux Q ";