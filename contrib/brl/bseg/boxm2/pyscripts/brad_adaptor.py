from boxm2_register import boxm2_batch, dbvalue

#############################################################################
# PROVIDES higher level brad python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################


# estimate atmospheric parameters
def estimate_atmospheric_parameters(image, metadata):
  boxm2_batch.init_process("bradEstimateAtmosphericParametersProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params

