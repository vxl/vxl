from boxm2_register import boxm2_batch, dbvalue

#############################################################################
# PROVIDES higher level brad python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################


# parse metadata of a sat image, reads sat image, nitf2 header and a metadata file in the same folder as the image if found
def read_nitf_metadata(nitf_filename, imd_folder=""):
  boxm2_batch.init_process("bradNITFReadMetadataProcess")
  boxm2_batch.set_input_string(0, nitf_filename)  # requires full path and name
  boxm2_batch.set_input_string(1, imd_folder)  # pass empty if meta is in img folder
  boxm2_batch.run_process()
  (id, type) = boxm2_batch.commit_output(0)
  meta = dbvalue(id, type)
  return meta

# radiometrically normalize a sat image (cropped) based on its metadata
def radiometrically_calibrate(cropped_image, meta):
  boxm2_batch.init_process("bradNITFAbsRadiometricCalibrationProcess")
  boxm2_batch.set_input_from_db(0, cropped_image)
  boxm2_batch.set_input_from_db(1, meta)
  boxm2_batch.run_process()
  (id, type) = boxm2_batch.commit_output(0)
  cropped_img_cal = dbvalue(id, type)
  return cropped_img_cal

# estimate atmospheric parameters
def estimate_atmospheric_parameters(image, metadata, mean_reflectance = None):
  boxm2_batch.init_process("bradEstimateAtmosphericParametersProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  if mean_reflectance != None:
     boxm2_batch.set_input_float(2,mean_reflectance)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params

# convert radiance values to estimated reflectances
def estimate_reflectance(image, metadata, atmospheric_params):
  boxm2_batch.init_process("bradEstimateReflectanceProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  boxm2_batch.set_input_from_db(2,atmospheric_params)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  reflectance_img = dbvalue(id,type)
  return reflectance_img

def find_sun_dir_bin(metadata, output_file):
  boxm2_batch.init_process("bradSunDirBinProcess");
  boxm2_batch.set_input_from_db(0,metadata)
  boxm2_batch.set_input_string(1,output_file);
  boxm2_batch.run_process();
  (bin_id,bin_type)=boxm2_batch.commit_output(0);
  bin = boxm2_batch.get_output_int(bin_id);
  return bin
 
# save atmospheric parameters
def save_atmospheric_parameters(atm_params, filename):
  boxm2_batch.init_process("bradSaveAtmosphericParametersProcess")
  boxm2_batch.set_input_from_db(0,atm_params)
  boxm2_batch.set_input_string(1,filename)
  boxm2_batch.run_process()
  return

# save image_metadata
def load_image_metadata(filename):
  boxm2_batch.init_process("bradLoadImageMetadataProcess")
  boxm2_batch.set_input_string(0,filename)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  mdata = dbvalue(id,type)
  return mdata

# save atmospheric parameters
def load_atmospheric_parameters(filename):
  boxm2_batch.init_process("bradLoadAtmosphericParametersProcess")
  boxm2_batch.set_input_string(0,filename)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params
 

