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

def sun_angles(image_path):
  boxm2_batch.init_process('bbasSunAnglesProcess')
  boxm2_batch.set_input_string(0,image_path)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  sun_el = boxm2_batch.get_output_float(id)
  (id,type) = boxm2_batch.commit_output(1)
  sun_az = boxm2_batch.get_output_float(id)
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


