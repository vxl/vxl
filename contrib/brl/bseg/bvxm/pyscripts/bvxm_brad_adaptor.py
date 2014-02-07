from bvxm_register import bvxm_batch, dbvalue

#############################################################################
# PROVIDES higher level brad python functions to make bvxm_batch
# code more readable/refactored
#############################################################################


# parse metadata of a sat image, reads sat image, nitf2 header and a metadata file in the same folder as the image if found
def read_nitf_metadata(nitf_filename, imd_folder=""):
  bvxm_batch.init_process("bradNITFReadMetadataProcess")
  bvxm_batch.set_input_string(0, nitf_filename)  # requires full path and name
  bvxm_batch.set_input_string(1, imd_folder)  # pass empty if meta is in img folder
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0)
  meta = dbvalue(id, type)
  return meta

# radiometrically normalize a sat image (cropped) based on its metadata
def radiometrically_calibrate(cropped_image, meta):
  bvxm_batch.init_process("bradNITFAbsRadiometricCalibrationProcess")
  bvxm_batch.set_input_from_db(0, cropped_image)
  bvxm_batch.set_input_from_db(1, meta)
  bvxm_batch.run_process()
  (id, type) = bvxm_batch.commit_output(0)
  cropped_img_cal = dbvalue(id, type)
  return cropped_img_cal

# estimate atmospheric parameters
def estimate_atmospheric_parameters(image, metadata, mean_reflectance = None, constrain_parameters=None):
  bvxm_batch.init_process("bradEstimateAtmosphericParametersProcess")
  bvxm_batch.set_input_from_db(0,image)
  bvxm_batch.set_input_from_db(1,metadata)
  if mean_reflectance != None:
     bvxm_batch.set_input_float(2,mean_reflectance)
  if constrain_parameters != None:
      bvxm_batch.set_input_bool(3, constrain_parameters)
  result = bvxm_batch.run_process()
  if result :
    (id,type) = bvxm_batch.commit_output(0)
    atm_params = dbvalue(id,type)
  else :
    atm_params = 0
  return atm_params

# convert radiance values to estimated reflectances
def estimate_reflectance(image, metadata, atmospheric_params):
  bvxm_batch.init_process("bradEstimateReflectanceProcess")
  bvxm_batch.set_input_from_db(0,image)
  bvxm_batch.set_input_from_db(1,metadata)
  bvxm_batch.set_input_from_db(2,atmospheric_params)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  reflectance_img = dbvalue(id,type)
  return reflectance_img

# convert reflectance image back to digital count
def convert_reflectance_to_digital_count(reflectance_image, metadata, atmospheric_params, normalize_0_1 = False, max_digital_count = 2047):
  bvxm_batch.init_process("bradConvertReflectanceToDigitalCountProcess")
  bvxm_batch.set_input_from_db(0,reflectance_image)
  bvxm_batch.set_input_from_db(1,metadata)
  bvxm_batch.set_input_from_db(2,atmospheric_params)
  bvxm_batch.set_input_bool(3,normalize_0_1)
  bvxm_batch.set_input_unsigned(4,max_digital_count)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  output_img = dbvalue(id,type)
  return output_img

def find_sun_dir_bin(metadata, output_file):
  bvxm_batch.init_process("bradSunDirBinProcess");
  bvxm_batch.set_input_from_db(0,metadata)
  bvxm_batch.set_input_string(1,output_file);
  bvxm_batch.run_process();
  (bin_id,bin_type)=bvxm_batch.commit_output(0);
  bin = bvxm_batch.get_output_int(bin_id);
  return bin

# save atmospheric parameters
def save_atmospheric_parameters(atm_params, filename):
  bvxm_batch.init_process("bradSaveAtmosphericParametersProcess")
  bvxm_batch.set_input_from_db(0,atm_params)
  bvxm_batch.set_input_string(1,filename)
  bvxm_batch.run_process()
  return

# save image_metadata
def save_image_metadata(mdata, filename):
  bvxm_batch.init_process("bradSaveImageMetadataProcess")
  bvxm_batch.set_input_from_db(0, mdata)
  bvxm_batch.set_input_string(1,filename)
  bvxm_batch.run_process()
  return

# load image_metadata
def load_image_metadata(filename):
  bvxm_batch.init_process("bradLoadImageMetadataProcess")
  bvxm_batch.set_input_string(0,filename)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  mdata = dbvalue(id,type)
  return mdata

# load atmospheric parameters
def load_atmospheric_parameters(filename):
  bvxm_batch.init_process("bradLoadAtmosphericParametersProcess")
  bvxm_batch.set_input_string(0,filename)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params

# compute sun angles given location and time
def get_sun_angles_date_time(lat, lon, year, month, day, hour, minute):
  bvxm_batch.init_process("bradGetSunAnglesDateTimeProcess")
  bvxm_batch.set_input_float(0,lat)
  bvxm_batch.set_input_float(1,lon)
  bvxm_batch.set_input_int(2,year)
  bvxm_batch.set_input_int(3,month)
  bvxm_batch.set_input_int(4,day)
  bvxm_batch.set_input_int(5,hour)
  bvxm_batch.set_input_int(6,minute)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  sun_az = bvxm_batch.get_output_float(id)
  bvxm_batch.remove_data(id)
  (id,type) = bvxm_batch.commit_output(1)
  sun_el = bvxm_batch.get_output_float(id)
  bvxm_batch.remove_data(id)
  return sun_az, sun_el

# set sun angles in metadata
def set_sun_angles(mdata, sun_az, sun_el):
  bvxm_batch.init_process("bradSetSunAnglesProcess")
  bvxm_batch.set_input_from_db(0,mdata)
  bvxm_batch.set_input_float(1, sun_az)
  bvxm_batch.set_input_float(2, sun_el)
  bvxm_batch.run_process()

# extract sun azimuth and elevation angles from metadata
def get_sun_angles(mdata):
  bvxm_batch.init_process("bradGetSunAnglesProcess")
  bvxm_batch.set_input_from_db(0,mdata)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  sun_az = bvxm_batch.get_output_float(id)
  bvxm_batch.remove_data(id)
  (id,type) = bvxm_batch.commit_output(1)
  sun_el = bvxm_batch.get_output_float(id)
  bvxm_batch.remove_data(id)
  return sun_az, sun_el

#create a new image_metadata object
def create_image_metadata(gain=1.0, offset=0.0, view_az = 0.0, view_el = 90.0, sun_az = 0.0, sun_el = 90.0, sun_irrad = None):
  bvxm_batch.init_process("bradCreateImageMetadataProcess")
  bvxm_batch.set_input_float(0,gain)
  bvxm_batch.set_input_float(1,offset)
  bvxm_batch.set_input_float(2,view_az)
  bvxm_batch.set_input_float(3,view_el)
  bvxm_batch.set_input_float(4,sun_az)
  bvxm_batch.set_input_float(5,sun_el)
  if sun_irrad != None:
    bvxm_batch.set_input_float(6,sun_irrad)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  mdata = dbvalue(id,type)
  return mdata

# create a new atmopsheric_parameters object
def create_atmospheric_parameters(airlight=0.0, skylight=0.0, optical_depth=0.0):
  bvxm_batch.init_process("bradCreateAtmosphericParametersProcess")
  bvxm_batch.set_input_float(0,airlight)
  bvxm_batch.set_input_float(1,skylight)
  bvxm_batch.set_input_float(2,optical_depth)
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  atm_params = dbvalue(id,type)
  return atm_params

def estimate_shadow_density(image, metadata, atmospheric_params):
  bvxm_batch.init_process("bradEstimateShadowsProcess")
  bvxm_batch.set_input_from_db(0,image)
  bvxm_batch.set_input_from_db(1,metadata)
  bvxm_batch.set_input_from_db(2,atmospheric_params)
  bvxm_batch.set_input_bool(3,True) # True returns prob. density value
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  shadow_density = dbvalue(id,type)
  return shadow_density

def estimate_shadows(image, metadata, atmospheric_params):
  bvxm_batch.init_process("bradEstimateShadowsProcess")
  bvxm_batch.set_input_from_db(0,image)
  bvxm_batch.set_input_from_db(1,metadata)
  bvxm_batch.set_input_from_db(2,atmospheric_params)
  bvxm_batch.set_input_bool(3,False) # False returns probability value
  bvxm_batch.run_process()
  (id,type) = bvxm_batch.commit_output(0)
  shadow_probs = dbvalue(id,type)
  return shadow_probs

def load_eigenspace(filename):
  bvxm_batch.init_process("bradLoadEigenspaceProcess");
  bvxm_batch.set_input_string(0, filename);
  bvxm_batch.run_process();
  (eig_id,eig_type)=bvxm_batch.commit_output(0);
  eig = dbvalue(eig_id, eig_type);
  return eig;

def classify_image(eig, h_no, h_atmos, input_image_filename, tile_ni, tile_nj):
  bvxm_batch.init_process("bradClassifyImageProcess");
  bvxm_batch.set_input_from_db(0, eig);
  bvxm_batch.set_input_from_db(1, h_no);
  bvxm_batch.set_input_from_db(2, h_atmos);
  bvxm_batch.set_input_string(3, input_image_filename);
  bvxm_batch.set_input_unsigned(4, tile_ni);
  bvxm_batch.set_input_unsigned(5, tile_nj);
  bvxm_batch.run_process();
  (vid, vtype) = bvxm_batch.commit_output(0);
  q_img = dbvalue(vid, vtype);
  (vid, vtype) = bvxm_batch.commit_output(1);
  q_img_orig_size = dbvalue(vid, vtype);
  return q_img, q_img_orig_size
