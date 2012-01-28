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

# radiometrically normalize a sat image (cropped) based on its metadata
def radiometrically_calibrate(cropped_image, meta):
  boxm2_batch.init_process("bradNITFAbsRadiometricCalibrationProcess")
  boxm2_batch.set_input_from_db(0, cropped_image)
  boxm2_batch.set_input_from_db(1, meta)
  boxm2_batch.run_process()
  (id, type) = boxm2_batch.commit_output(0)
  cropped_img_cal = dbvalue(id, type)
  return cropped_img_cal

# parse metadata of a sat image, reads sat image, nitf2 header and a metadata file in the same folder as the image if found
def read_nitf_metadata(nitf_filename, imd_folder=""):
  boxm2_batch.init_process("bradNITFReadMetadataProcess")
  boxm2_batch.set_input_string(0, nitf_filename)  # requires full path and name
  boxm2_batch.set_input_string(1, imd_folder)  # pass empty if meta is in img folder
  boxm2_batch.run_process()
  (id, type) = boxm2_batch.commit_output(0)
  meta = dbvalue(id, type)
  return meta
