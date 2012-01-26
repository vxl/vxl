from boxm2_register import boxm2_batch, dbvalue

#############################################################################
# PROVIDES higher level bbas python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################
def atmospheric_correct(image, sun_z):
  boxm2_batch.init_process('bbasAtmosphericCorrProcess')
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_float(1,sun_z)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  image_corrected = dbvalue(id,type)
  return image_corrected 

def estimate_irradiance(image, sun_z, mean_albedo=1.0):
  boxm2_batch.init_process('bbasEstimateIrradianceProcess')
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_float(1,sun_z)
  boxm2_batch.set_input_float(2,mean_albedo)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  img_float = dbvalue(id,type)
  (id,type) = boxm2_batch.commit_output(1)
  irrad = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return(img_float, irrad)

def sun_angles(image_path):
  boxm2_batch.init_process('bbasSunAnglesProcess')
  boxm2_batch.set_input_string(0,image_path)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  sun_el = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(1)
  sun_az = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return(sun_az, sun_el)

def sun_position(image_path):
  boxm2_batch.init_process('bbasSunAnglesProcess')
  boxm2_batch.set_input_string(0,image_path)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  sun_el = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(1)
  sun_az = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(2)
  sun_dist = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)  
  return(sun_az, sun_el, sun_dist)

def camera_angles(camera, x,y,z):
  boxm2_batch.init_process('bbasCameraAnglesProcess')
  boxm2_batch.set_input_from_db(0,camera)
  boxm2_batch.set_input_float(1,x)
  boxm2_batch.set_input_float(2,y)
  boxm2_batch.set_input_float(3,z)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  cam_az = boxm2_batch.get_output_float(id)
  (id,type) = boxm2_batch.commit_output(1)
  cam_el = boxm2_batch.get_output_float(id)
  return (cam_az, cam_el)

# radiometrically normalize a Quickbird image (cropped) based on AbsCalFactor in IMD file
def radiometrically_calibrate(cropped_image, imd_filename):
  boxm2_batch.init_process("bradNITFAbsRadiometricCalibrationProcess")
  boxm2_batch.set_input_from_db(0, cropped_image)
  boxm2_batch.set_input_string(1, imd_filename)
  boxm2_batch.run_process()
  (id, type) = boxm2_batch.commit_output(0)
  cropped_img_cal = dbvalue(id, type)
  return cropped_img_cal

def estimate_radiance_values(image, sun_el, sun_dist, sensor_el, solar_irrad=None, downwelled_irrad=None, optical_depth=None):
  boxm2_batch.init_process("bradEstimateRadianceValuesProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_float(1,sun_el)
  boxm2_batch.set_input_float(2,sun_dist)
  boxm2_batch.set_input_float(3,sensor_el)
  if solar_irrad != None:
     boxm2_batch.set_input_float(4,solar_irrad)
  if downwelled_irrad != None:
     boxm2_batch.set_input_float(5,downwelled_irrad)
  if optical_depth != None:
     boxm2_batch.set_input_float(6, optical_depth)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  airlight = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(1)
  ref_horizontal = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  (id,type) = boxm2_batch.commit_output(2)
  ref_sun_facing = boxm2_batch.get_output_float(id)
  boxm2_batch.remove_data(id)
  return (airlight, ref_horizontal, ref_sun_facing)


#Removes elements from brdb (list of elements, or just one)
def remove_from_db(dbvals) :
  if not isinstance(dbvals, (list, tuple)) : 
    dbvals = [dbvals]
  for dbval in dbvals:
    boxm2_batch.init_process("bbasRemoveFromDbProcess")
    boxm2_batch.set_input_unsigned(0, dbval.id);
    boxm2_batch.run_process();

