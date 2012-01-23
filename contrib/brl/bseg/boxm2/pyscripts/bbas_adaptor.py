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

def batch_normalize(img_list, cam_list, nitf_list, x_range, y_range, DEM_fname, geoid_height, output_fname):
    boxm2_batch.init_process('bbasBatchNormalizeProcess')
    boxm2_batch.set_input_string(0,img_list)
    boxm2_batch.set_input_string(1,cam_list)
    boxm2_batch.set_input_string(2,nitf_list)
    boxm2_batch.set_input_float(3,x_range[0])
    boxm2_batch.set_input_float(4,y_range[0])
    boxm2_batch.set_input_float(5,x_range[1])
    boxm2_batch.set_input_float(6,y_range[1])
    boxm2_batch.set_input_string(7,DEM_fname)
    boxm2_batch.set_input_float(8,geoid_height)
    boxm2_batch.set_input_string(9,output_fname)
    boxm2_batch.run_process()

def batch_estimate(img_list, cam_list, nitf_list, x_range, y_range, DEM_fname, geoid_height, output_fname):
    boxm2_batch.init_process('bbasBatchEstimateProcess')
    boxm2_batch.set_input_string(0,img_list)
    boxm2_batch.set_input_string(1,cam_list)
    boxm2_batch.set_input_string(2,nitf_list)
    boxm2_batch.set_input_float(3,x_range[0])
    boxm2_batch.set_input_float(4,y_range[0])
    boxm2_batch.set_input_float(5,x_range[1])
    boxm2_batch.set_input_float(6,y_range[1])
    boxm2_batch.set_input_string(7,DEM_fname)
    boxm2_batch.set_input_float(8,geoid_height)
    boxm2_batch.set_input_string(9,output_fname)
    boxm2_batch.run_process()

def batch_estimate_probs(img_list, cam_list, nitf_list, x_range, y_range, DEM_fname, geoid_height, output_fname):
    boxm2_batch.init_process('bbasBatchEstimateProbsProcess')
    boxm2_batch.set_input_string(0,img_list)
    boxm2_batch.set_input_string(1,cam_list)
    boxm2_batch.set_input_string(2,nitf_list)
    boxm2_batch.set_input_float(3,x_range[0])
    boxm2_batch.set_input_float(4,y_range[0])
    boxm2_batch.set_input_float(5,x_range[1])
    boxm2_batch.set_input_float(6,y_range[1])
    boxm2_batch.set_input_string(7,DEM_fname)
    boxm2_batch.set_input_float(8,geoid_height)
    boxm2_batch.set_input_string(9,output_fname)
    boxm2_batch.run_process()

#Removes elements from brdb (list of elements, or just one)
def remove_from_db(dbvals) :
  if not isinstance(dbvals, (list, tuple)) : 
    dbvals = [dbvals]
  for dbval in dbvals:
    boxm2_batch.init_process("bbasRemoveFromDbProcess")
    boxm2_batch.set_input_unsigned(0, dbval.id);
    boxm2_batch.run_process();

