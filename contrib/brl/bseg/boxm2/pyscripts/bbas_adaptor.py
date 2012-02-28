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
def bbox_from_ply(filename):
  minpoint = list();
  maxpoint = list();
  boxm2_batch.init_process('imeshPlyBboxProcess')
  boxm2_batch.set_input_string(0,filename)
  boxm2_batch.run_process()
  for i in (0,1,2):
    (id,type) = boxm2_batch.commit_output(i)
    minpoint.append(boxm2_batch.get_output_double(id));
  for i in (3,4,5):
    (id,type) = boxm2_batch.commit_output(i)
    maxpoint.append(boxm2_batch.get_output_double(id));
  return (minpoint, maxpoint)

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

def prob_as_expected(image,atomicity):
  boxm2_batch.init_process("bslExpectedImageProcess")
  boxm2_batch.set_input_from_db(0,image)
  boxm2_batch.set_input_float(1,atomicity)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  image_prob = dbvalue(id,type)
  return image_prob
  
def fuse_beliefs(image1,image2,atomicity):
  boxm2_batch.init_process("bslFusionProcess")
  boxm2_batch.set_input_from_db(0,image1)
  boxm2_batch.set_input_from_db(1,image2)
  boxm2_batch.set_input_float(2,atomicity)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  fused_image = dbvalue(id,type)
  return fused_image

#Removes elements from brdb (list of elements, or just one)
def remove_from_db(dbvals) :
  if not isinstance(dbvals, (list, tuple)) :
    dbvals = [dbvals]
  for dbval in dbvals:
    boxm2_batch.init_process("bbasRemoveFromDbProcess")
    boxm2_batch.set_input_unsigned(0, dbval.id);
    boxm2_batch.run_process();

def vrml_initialize(vrml_filename):
  boxm2_batch.init_process("bvrmlInitializeProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.run_process();

def vrml_write_box(vrml_filename, bbox, is_wire, r, g, b):
  boxm2_batch.init_process("bvrmlWriteBoxProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.set_input_double(1,bbox[0]);  # minx
  boxm2_batch.set_input_double(2,bbox[1]);  # miny
  boxm2_batch.set_input_double(3,bbox[2]);  # minz
  boxm2_batch.set_input_double(4,bbox[3]);  # maxx
  boxm2_batch.set_input_double(5,bbox[4]);  # maxy
  boxm2_batch.set_input_double(6,bbox[5]);  # maxz
  boxm2_batch.set_input_bool(7,is_wire);
  boxm2_batch.set_input_float(8,r);
  boxm2_batch.set_input_float(9,g);
  boxm2_batch.set_input_float(10,b);
  boxm2_batch.run_process();
  
def vrml_write_origin(vrml_filename, axis_len):
  boxm2_batch.init_process("bvrmlWriteOriginAndAxesProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.set_input_float(1,axis_len);
  boxm2_batch.run_process();

def vrml_write_point(vrml_filename, coords, radius, r, g, b ):
  boxm2_batch.init_process("bvrmlWritePointProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.set_input_float(1,coords[0]); # x
  boxm2_batch.set_input_float(2,coords[1]); # y
  boxm2_batch.set_input_float(3,coords[2]); # z
  boxm2_batch.set_input_float(4, radius); # radius
  boxm2_batch.set_input_float(5, r); # red
  boxm2_batch.set_input_float(6, g); # green
  boxm2_batch.set_input_float(7, b); # blue
  boxm2_batch.run_process();
  
def initialize_rng(seed=0):
  boxm2_batch.init_process("bstaInitializeRandomSeedProcess");
  boxm2_batch.set_input_unsigned(0,seed);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0)
  rng = dbvalue(id,type)
  return rng

