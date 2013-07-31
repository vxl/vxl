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
def estimate_atmospheric_parameters(image, metadata, mean_reflectance = None, constrain_parameters=None):
  boxm2_batch.init_process("bradEstimateAtmosphericParametersProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  if mean_reflectance != None:
     boxm2_batch.set_input_float(2,mean_reflectance)
  if constrain_parameters != None:
      boxm2_batch.set_input_bool(3, constrain_parameters)
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

# convert reflectance image back to digital count
def convert_reflectance_to_digital_count(reflectance_image, metadata, atmospheric_params, normalize_0_1 = False, max_digital_count = 2047):
  boxm2_batch.init_process("bradConvertReflectanceToDigitalCountProcess")
  boxm2_batch.set_input_from_db(0,reflectance_image)
  boxm2_batch.set_input_from_db(1,metadata)
  boxm2_batch.set_input_from_db(2,atmospheric_params)
  boxm2_batch.set_input_bool(3,normalize_0_1)
  boxm2_batch.set_input_unsigned(4,max_digital_count)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  output_img = dbvalue(id,type)
  return output_img

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
def save_image_metadata(mdata, filename):
  boxm2_batch.init_process("bradSaveImageMetadataProcess")
  boxm2_batch.set_input_from_db(0, mdata)
  boxm2_batch.set_input_string(1,filename)
  boxm2_batch.run_process()
  return 

# load image_metadata
def load_image_metadata(filename):
  boxm2_batch.init_process("bradLoadImageMetadataProcess")
  boxm2_batch.set_input_string(0,filename)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  mdata = dbvalue(id,type)
  return mdata

# load atmospheric parameters
def load_atmospheric_parameters(filename):
  boxm2_batch.init_process("bradLoadAtmosphericParametersProcess")
  boxm2_batch.set_input_string(0,filename)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params
 
# compute sun angles given location and time
def get_sun_angles_date_time(lat, lon, year, month, day, hour, minute):
  boxm2_batch.init_process("bradGetSunAnglesDateTimeProcess")
  boxm2_batch.set_input_float(0,lat)
  boxm2_batch.set_input_float(1,lon)
  boxm2_batch.set_input_int(2,year)
  boxm2_batch.set_input_int(3,month)
  boxm2_batch.set_input_int(4,day)
  boxm2_batch.set_input_int(5,hour)
  boxm2_batch.set_input_int(6,minute)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  sun_az = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(1)
  sun_el = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return sun_az, sun_el

# set sun angles in metadata
def set_sun_angles(mdata, sun_az, sun_el):
  boxm2_batch.init_process("bradSetSunAnglesProcess")
  boxm2_batch.set_input_from_db(0,mdata)
  boxm2_batch.set_input_float(1, sun_az)
  boxm2_batch.set_input_float(2, sun_el)
  boxm2_batch.run_process()

# extract sun azimuth and elevation angles from metadata
def get_sun_angles(mdata):
  boxm2_batch.init_process("bradGetSunAnglesProcess")
  boxm2_batch.set_input_from_db(0,mdata)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  sun_az = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(1)
  sun_el = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return sun_az, sun_el

#create a new image_metadata object
def create_image_metadata(gain=1.0, offset=0.0, view_az = 0.0, view_el = 90.0, sun_az = 0.0, sun_el = 90.0, sun_irrad = None):
  boxm2_batch.init_process("bradCreateImageMetadataProcess")
  boxm2_batch.set_input_float(0,gain)
  boxm2_batch.set_input_float(1,offset)
  boxm2_batch.set_input_float(2,view_az)
  boxm2_batch.set_input_float(3,view_el)
  boxm2_batch.set_input_float(4,sun_az)
  boxm2_batch.set_input_float(5,sun_el)
  if sun_irrad != None:
    boxm2_batch.set_input_float(6,sun_irrad)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  mdata = dbvalue(id,type)
  return mdata

# create a new atmopsheric_parameters object
def create_atmospheric_parameters(airlight=0.0, skylight=0.0, optical_depth=0.0):
  boxm2_batch.init_process("bradCreateAtmosphericParametersProcess")
  boxm2_batch.set_input_float(0,airlight)
  boxm2_batch.set_input_float(1,skylight)
  boxm2_batch.set_input_float(2,optical_depth)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params

def estimate_shadow_density(image, metadata, atmospheric_params):
  boxm2_batch.init_process("bradEstimateShadowsProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  boxm2_batch.set_input_from_db(2,atmospheric_params)
  boxm2_batch.set_input_bool(3,True) # True returns prob. density value
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  shadow_density = dbvalue(id,type)
  return shadow_density

def estimate_shadows(image, metadata, atmospheric_params):
  boxm2_batch.init_process("bradEstimateShadowsProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_from_db(1,metadata)
  boxm2_batch.set_input_from_db(2,atmospheric_params)
  boxm2_batch.set_input_bool(3,False) # False returns probability value
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  shadow_probs = dbvalue(id,type)
  return shadow_probs
  
def load_eigenspace(filename):
  boxm2_batch.init_process("bradLoadEigenspaceProcess");
  boxm2_batch.set_input_string(0, filename);
  boxm2_batch.run_process();
  (eig_id,eig_type)=boxm2_batch.commit_output(0);
  eig = dbvalue(eig_id, eig_type);
  return eig;
  
def classify_image(eig, h_no, h_atmos, input_image_filename, tile_ni, tile_nj):
  boxm2_batch.init_process("bradClassifyImageProcess");
  boxm2_batch.set_input_from_db(0, eig);
  boxm2_batch.set_input_from_db(1, h_no);
  boxm2_batch.set_input_from_db(2, h_atmos);
  boxm2_batch.set_input_string(3, input_image_filename);
  boxm2_batch.set_input_unsigned(4, tile_ni);
  boxm2_batch.set_input_unsigned(5, tile_nj);
  boxm2_batch.run_process();
  (vid, vtype) = boxm2_batch.commit_output(0);
  q_img = dbvalue(vid, vtype);
  (vid, vtype) = boxm2_batch.commit_output(1);
  q_img_orig_size = dbvalue(vid, vtype);
  return q_img, q_img_orig_size


