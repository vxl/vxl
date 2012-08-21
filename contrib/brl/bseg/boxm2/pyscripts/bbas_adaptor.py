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
  
def save_sun_index(output_file_name, longitude, latitude, year, hour, minute, radius):
  boxm2_batch.init_process("bradSaveSunIndexProcess");
  boxm2_batch.set_input_string(0,output_file_name);
  boxm2_batch.set_input_float(1, longitude);
  boxm2_batch.set_input_float(2, latitude);
  boxm2_batch.set_input_int(3,year);
  boxm2_batch.set_input_int(4,hour);
  boxm2_batch.set_input_int(5,minute);
  boxm2_batch.set_input_int(6,radius);
  boxm2_batch.run_process();
  
def sun_dir_bin(meta,illum_bin_filename):
  boxm2_batch.init_process("bradSunDirBinProcess");
  boxm2_batch.set_input_from_db(0,meta);
  boxm2_batch.set_input_string(1,illum_bin_filename);
  boxm2_batch.run_process();
  (id,type)=boxm2_batch.commit_output(0);
  bin = boxm2_batch.get_output_int(id);
  return bin;

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

#redirects std out to a file
def set_stdout(file) :
  boxm2_batch.set_stdout(file);

#resets std out to terminal
def reset_stdout() :
  boxm2_batch.reset_stdout();

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
  
def vrml_write_line(vrml_filename, pt1_coords, pt2_coords, r, g, b ):
  boxm2_batch.init_process("bvrmlWriteLineProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.set_input_float(1,pt1_coords[0]); # x
  boxm2_batch.set_input_float(2,pt1_coords[1]); # y
  boxm2_batch.set_input_float(3,pt1_coords[2]); # z
  boxm2_batch.set_input_float(4,pt2_coords[0]); # x
  boxm2_batch.set_input_float(5,pt2_coords[1]); # y
  boxm2_batch.set_input_float(6,pt2_coords[2]); # z
  boxm2_batch.set_input_float(7, r); # red
  boxm2_batch.set_input_float(8, g); # green
  boxm2_batch.set_input_float(9, b); # blue
  boxm2_batch.run_process();
  
def vrml_filter_ply(vrml_filename, ply_file, points_file, dist_threshold,nearest=False):
  boxm2_batch.init_process("bvrmlFilteredPlyProcess");
  boxm2_batch.set_input_string(0,vrml_filename);
  boxm2_batch.set_input_string(1,ply_file);
  boxm2_batch.set_input_string(2,points_file);
  boxm2_batch.set_input_float(3,dist_threshold);
  boxm2_batch.set_input_bool(4,nearest);
  boxm2_batch.run_process();
  
def initialize_rng(seed=0):
  boxm2_batch.init_process("bstaInitializeRandomSeedProcess");
  boxm2_batch.set_input_unsigned(0,seed);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0)
  rng = dbvalue(id,type)
  return rng
  
def load_joint_hist3d(filename):
  boxm2_batch.init_process("bstaLoadJointHist3dProcess");
  boxm2_batch.set_input_string(0,filename);
  boxm2_batch.run_process();
  (h_id,h_type) = boxm2_batch.commit_output(0);
  h = dbvalue(h_id, h_type);
  return h
  
def texture_classifier_kernel_margin(dictionary):
  boxm2_batch.init_process("sdetTextureClassifierKernelMarginProcess");
  boxm2_batch.set_input_string(0,dictionary);
  boxm2_batch.run_process();
  (m_id, m_type) = boxm2_batch.commit_output(0);
  margin = boxm2_batch.get_output_int(m_id);
  return margin;
  
def texture_classifier(tcl, dictionary, img, block_size=64):
  boxm2_batch.init_process("sdetTextureClassifierProcess");
  boxm2_batch.set_input_from_db(0, tcl);
  boxm2_batch.set_input_string(1,dictionary);
  boxm2_batch.set_input_from_db(2, img);
  boxm2_batch.set_input_unsigned(3,block_size);    # size of blocks
  boxm2_batch.run_process();
  (img_id, img_type) = boxm2_batch.commit_output(0);
  img_classified = dbvalue(img_id, img_type);
  return img_classified
  
def create_texture_classifier(lambda0, lambda1,n_scales,scale_interval,angle_interval,laplace_radius,gauss_radius,k,n_samples):
  boxm2_batch.init_process("sdetCreateTextureClassifierProcess");
  boxm2_batch.set_input_float(0,lambda0);
  boxm2_batch.set_input_float(1,lambda1);
  boxm2_batch.set_input_unsigned(2,n_scales);
  boxm2_batch.set_input_float(3, scale_interval);
  boxm2_batch.set_input_float(4, angle_interval);
  boxm2_batch.set_input_float(5, laplace_radius);
  boxm2_batch.set_input_float(6, gauss_radius);
  boxm2_batch.set_input_unsigned(7, k);
  boxm2_batch.set_input_unsigned(8,n_samples);
  boxm2_batch.run_process();
  (tclsf_id, tclsf_type)=boxm2_batch.commit_output(0);
  tclsf = dbvalue(tclsf_id, tclsf_type);
  return tclsf;
  
def texture_training(tclsf, image, exp_poly_path,texton_dict_path, name_of_category="mod", compute_textons=True):
  boxm2_batch.init_process("sdetTextureTrainingProcess");
  boxm2_batch.set_input_from_db(0,tclsf);
  boxm2_batch.set_input_bool(1,compute_textons);# compute textons
  boxm2_batch.set_input_from_db(2,image);
  boxm2_batch.set_input_string(3,exp_poly_path);
  boxm2_batch.set_input_string(4,name_of_category);
  boxm2_batch.set_input_string(5,texton_dict_path);
  boxm2_batch.run_process();
  # write out the texton dictionary on finish
  boxm2_batch.finish_process();

def print_texton_dictionary(tclsf, texton_dict_path, print_mode="inter_dist"):
  boxm2_batch.init_process("sdetPrintTextonDictProcess");
  boxm2_batch.set_input_from_db(0,tclsf);
  boxm2_batch.set_input_string(1,texton_dict_path);
  boxm2_batch.set_input_string(2,print_mode);
  boxm2_batch.run_process();
  
def texture_classifier_exp_img(tclsf, dict_expected, fimage, fexp, block_size=64):
  boxm2_batch.init_process("sdetExpImgClassifierProcess");
  boxm2_batch.set_input_from_db(0, tclsf);
  boxm2_batch.set_input_string(1,dict_expected);
  boxm2_batch.set_input_from_db(2, fimage);
  boxm2_batch.set_input_from_db(3, fexp);
  boxm2_batch.set_input_unsigned(4,64);
  boxm2_batch.run_process();
  (text_class_id, text_class_type) = boxm2_batch.commit_output(0);
  text_class = dbvalue(text_class_id, text_class_type);
  return text_class;

def solve_gain_offset(model_image, test_image, test_mask):
  boxm2_batch.init_process("bripSolveGainOffsetProcess");
  boxm2_batch.set_input_from_db(0, model_image);
  boxm2_batch.set_input_from_db(1, test_image);
  boxm2_batch.set_input_from_db(3, test_mask);
  boxm2_batch.run_process();
  (map_image_id, map_image_type) = boxm2_batch.commit_output(0);
  map_image = dbvalue(map_image_id, map_image_type);
  return map_image
  
def triangulate_site_corrs(site_file, out_file):
  boxm2_batch.init_process("bwmTriangulateCorrProcess");
  boxm2_batch.set_input_string(0,site_file);
  boxm2_batch.set_input_string(1,out_file);
  boxm2_batch.run_process();
  
def generate_depth_maps(depth_scene_file, output_folder, output_name_prefix, downsampling_level=0):
  boxm2_batch.init_process("bpglGenerateDepthMapsProcess");
  boxm2_batch.set_input_string(0,depth_scene_file);
  boxm2_batch.set_input_unsigned(1,downsampling_level);
  boxm2_batch.set_input_string(2,output_folder);
  boxm2_batch.set_input_string(3,output_name_prefix);
  boxm2_batch.run_process();
  

  
  


