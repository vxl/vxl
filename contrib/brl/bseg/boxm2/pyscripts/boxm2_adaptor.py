from boxm2_register import boxm2_batch, dbvalue; 

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################
def load_scene(scene_str): 
  print("Loading a Scene from file: ", scene_str);
  boxm2_batch.init_process("boxm2LoadSceneProcess");
  boxm2_batch.set_input_string(0, scene_str);
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);
  return scene; 

#does the opencl prep work on an input scene
def load_opencl(scene_str, device_string="gpu"):
  scene = load_scene(scene_str); 

  ###############################################################
  # Create cache, opencl manager, device, and gpu cache
  ###############################################################
  print("Create Main Cache");
  boxm2_batch.init_process("boxm2CreateCacheProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_string(1,"lru");
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cache = dbvalue(id, type);

  print("Init Manager");
  boxm2_batch.init_process("boclInitManagerProcess");
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  mgr = dbvalue(id, type);

  print("Get Gpu Device");
  boxm2_batch.init_process("boclGetDeviceProcess");
  boxm2_batch.set_input_string(0,device_string)
  boxm2_batch.set_input_from_db(1,mgr)
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  device = dbvalue(id, type);

  print("Create Gpu Cache");
  boxm2_batch.init_process("boxm2CreateOpenclCacheProcess");
  boxm2_batch.set_input_from_db(0,device)
  boxm2_batch.set_input_from_db(1,scene)
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  openclcache = dbvalue(id, type);

  return scene, cache, mgr, device, openclcache; 
  
  
#Just loads up CPP cache 
def load_cpp(scene_str) :
  scene = load_scene(scene_str); 

  ###############################################################
  # Create cache, opencl manager, device, and gpu cache
  ###############################################################
  print("Create Main Cache");
  boxm2_batch.init_process("boxm2CreateCacheProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_string(1,"lru");
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cache = dbvalue(id, type);
  return scene, cache; 

###############################################
# Model building stuff
###############################################
# Generic update - will use GPU if device/openclcache are passed in
def update_grey(scene, cache, cam, img, device=None, ident="", mask=None, update_alpha=True) :
  #If no device is passed in, do cpu update
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU update";
    boxm2_batch.init_process("boxm2CppUpdateImageProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_from_db(3,img);
    boxm2_batch.run_process();
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print("boxm2_batch GPU update");
    boxm2_batch.init_process("boxm2OclUpdateProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_from_db(4,img);
    boxm2_batch.set_input_string(5,ident);
    if mask :
      boxm2_batch.set_input_from_db(6,mask);
    boxm2_batch.set_input_bool(7, update_alpha); 
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    
####################################################################
# Generic update - will use GPU if device/openclcache are passed in
####################################################################
def update_rgb(scene, cache, cam, img, device=None, mask="") :
  #If no device is passed in, do cpu update
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch rgb CPU update not implemented";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print("boxm2_batch GPU update");
    boxm2_batch.init_process("boxm2OclUpdateColorProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_from_db(4,img);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 

 # Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
def render_height_map(scene, cache, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_adaptor, render height map cpp process not implemented"; 

  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclRenderExpectedHeightMapProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.run_process();
    
    #z_img
    (id,type) = boxm2_batch.commit_output(0);
    z_image = dbvalue(id,type);
    
    #var image
    (id,type) = boxm2_batch.commit_output(1);
    var_image = dbvalue(id,type);
    
    #x_img
    (id,type) = boxm2_batch.commit_output(2);
    x_image = dbvalue(id,type);
    
    #y_img
    (id,type) = boxm2_batch.commit_output(3);
    y_image = dbvalue(id,type);
    
    #prob_img
    (id,type) = boxm2_batch.commit_output(4);
    prob_image = dbvalue(id,type);
    
    #appearance_img
    (id,type) = boxm2_batch.commit_output(5);
    app_image = dbvalue(id,type);

    return z_image, var_image, x_image, y_image, prob_image, app_image; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
 
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################
def render_grey(scene, cache, cam, ni=1280, nj=720, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    boxm2_batch.init_process("boxm2CppRenderExpectedImageProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_unsigned(3,ni);
    boxm2_batch.set_input_unsigned(4,nj);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image; 
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclRenderExpectedImageProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_unsigned(4,ni);
    boxm2_batch.set_input_unsigned(5,nj);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    
#####################################################################    
# Generic render, returns a dbvalue expected image
#####################################################################
def render_rgb(scene, cache, cam, ni=1280, nj=720, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU render rgb not yet implemented";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclRenderExpectedColorProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_unsigned(4,ni);
    boxm2_batch.set_input_unsigned(5,nj);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
 
#####################################################################    
# render depth map 
#####################################################################
def render_depth(scene, cache, cam, ni=1280, nj=720, device=None) : 
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU render depth not yet implemented";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclRenderExpectedDepthProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_unsigned(4,ni);
    boxm2_batch.set_input_unsigned(5,nj);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
   
#####################################################################
# change detection wrapper
#####################################################################
def change_detect(scene, cache, cam, img, exp_img, device=None, rgb=False, n=1, raybelief="") : 
  if cache.type == "boxm2_cache_sptr" : 
    print "boxm2_batch CPU change detection"; 
    boxm2_batch.init_process("boxm2CppChangeDetectionProcess"); 
    boxm2_batch.set_input_from_db(0,scene); 
    boxm2_batch.set_input_from_db(1,cache); 
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_from_db(3,img); 
    boxm2_batch.set_input_from_db(4,exp_img); 
    boxm2_batch.run_process(); 
    (id,type) = boxm2_batch.commit_output(0); 
    cd_img = dbvalue(id,type); 
    return cd_img; 
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print "boxm2_batch GPU change detection"; 
    boxm2_batch.init_process("boxm2OclChangeDetectionProcess"); 
    boxm2_batch.set_input_from_db(0,device); 
    boxm2_batch.set_input_from_db(1,scene); 
    boxm2_batch.set_input_from_db(2,cache); 
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_from_db(4,img); 
    boxm2_batch.set_input_from_db(5,exp_img); 
    boxm2_batch.set_input_int(6, n); 
    boxm2_batch.set_input_string(7, raybelief);
    boxm2_batch.run_process(); 
    if not rgb :
      (id,type) = boxm2_batch.commit_output(0); 
      cd_img = dbvalue(id,type); 
    else :
      (id,type) = boxm2_batch.commit_output(1);
      cd_img = dbvalue(id,type);
    return cd_img; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type;
    


#####################################################################    
#generic refine (will work on color and grey scenes)
#####################################################################
def refine(scene, cache, thresh=0.3, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU refine";
    boxm2_batch.init_process("boxm2CppRefineProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_float(2,thresh);
    boxm2_batch.run_process();
    return 0;
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print "boxm2_batch GPU refine"; 
    boxm2_batch.init_process("boxm2OclRefineProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);    
    boxm2_batch.set_input_float(3,thresh);
    boxm2_batch.run_process();
    
    #get and report cells output
    (id, type) = boxm2_batch.commit_output(0); 
    nCells = boxm2_batch.get_output_unsigned(id); 
    return nCells;
  else : 
    print "ERROR: Cache type unrecognized: ", cache.type; 
    
#####################################################################
#generic merge method 
#####################################################################
def merge(scene, cache, thresh=0.01, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU merge"; 
    boxm2_batch.init_process("boxm2CppMergeProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_float(2,thresh);
    boxm2_batch.run_process();
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print "boxm2_batch GPU refine"; 
    boxm2_batch.init_process("boxm2OclMergeProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);    
    boxm2_batch.set_input_float(3,thresh);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type unrecognized: ", cache.type; 

#####################################################################
#generic filter scene, should work with color and grey scenes
#####################################################################
def median_filter(scene, cache, device=None) : 
  if cache.type == "boxm2_cache_sptr" : 
    print "boxm2_batch CPU median filter"; 
    boxm2_batch.init_process("boxm2CppFilterProcess"); 
    boxm2_batch.set_input_from_db(1, scene); 
    boxm2_batch.set_input_from_db(2, cache); 
    boxm2_batch.run_process(); 
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print "boxm2_batch GPU median filter";  
    boxm2_batch.init_process("boxm2OclFilterProcess"); 
    boxm2_batch.set_input_from_db(0, device); 
    boxm2_batch.set_input_from_db(1, scene); 
    boxm2_batch.set_input_from_db(2, cache); 
    boxm2_batch.run_process(); 
  else : 
    print "ERROR: Cache type unrecognized: ", cache.type; 


######################################################################
# cache methods
#####################################################################
#generic write cache to disk
def write_cache(cache) : 
  if cache.type == "boxm2_cache_sptr" : 
    boxm2_batch.init_process("boxm2WriteCacheProcess");
    boxm2_batch.set_input_from_db(0,cache);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type; 

#generic clear cache
def clear_cache(cache) : 
  if cache.type == "boxm2_cache_sptr" : 
    boxm2_batch.init_process("boxm2ClearCacheProcess");
    boxm2_batch.set_input_from_db(0,cache);
  elif cache.type == "boxm2_opencl_cache_sptr" :
    boxm2_batch.init_process("boxm2ClearOpenclCacheProcess"); 
    boxm2_batch.set_input_from_db(0,cache);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type; 


######################################################################
# trajectory methods
#####################################################################
def init_trajectory(scene, startInc, endInc, radius, ni=1280, nj=720) :  
  boxm2_batch.init_process("boxm2ViewInitTrajectoryProcess"); 
  boxm2_batch.set_input_from_db(0, scene);
  boxm2_batch.set_input_double(1, float(startInc) );  #incline0
  boxm2_batch.set_input_double(2, float(endInc) ); #incline1
  boxm2_batch.set_input_double(3, float(radius) ); #radius
  boxm2_batch.set_input_unsigned(4, ni); #ni
  boxm2_batch.set_input_unsigned(5, nj); #nj
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  trajectory = dbvalue(id,type);
  return trajectory; 

def trajectory_next(trajectory) : 
  boxm2_batch.init_process("boxm2ViewTrajectoryNextProcess"); 
  boxm2_batch.set_input_from_db(0, trajectory); 
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam; 

######################################################################
# camera/scene methods
#####################################################################
def bundle2scene(bundle_file, img_dir, app_model="boxm2_mog3_grey", out_dir="") : 
  if app_model == "boxm2_mog3_grey": 
    nobs_model = "boxm2_num_obs";
  elif app_model == "boxm2_gauss_rgb" :
    nobs_model = "boxm2_num_obs_single"
  else: 
    print "ERROR appearance model not recognized!!!", app_model; 
    return

  #run process
  boxm2_batch.init_process("boxm2BundleToSceneProcess");
  boxm2_batch.set_input_string(0, bundle_file);
  boxm2_batch.set_input_string(1, img_dir); 
  boxm2_batch.set_input_string(2, app_model); 
  boxm2_batch.set_input_string(3, nobs_model); 
  boxm2_batch.set_input_string(4, out_dir); 
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  uscene = dbvalue(scene_id, scene_type);
  (scene_id, scene_type) = boxm2_batch.commit_output(1);
  rscene = dbvalue(scene_id, scene_type);
  return uscene, rscene; 
  
  
def save_scene(scene, fname) : 
  boxm2_batch.init_process("boxm2WriteSceneXMLProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_string(1, fname); 
  boxm2_batch.run_process();
  
def scale_scene(scene, scale) : 
  boxm2_batch.init_process("boxm2ScaleSceneProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_float(1, scale); 
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);
  return scene; 

# Create multi block scene - params is a hash of scene parameters
def save_multi_block_scene(params) : 

  #load params
  scene_dir = params['scene_dir'] if 'scene_dir' in params else os.getcwd(); 
  app_model = params['app_model'] if 'app_model' in params else "boxm2_mog3_grey";
  obs_model = params['obs_model'] if 'obs_model' in params else "boxm2_num_obs";
  orig_x = params['orig_x'] if 'orig_x' in params else 0.0; 
  orig_y = params['orig_y'] if 'orig_y' in params else 0.0; 
  orig_z = params['orig_z'] if 'orig_z' in params else 0.0; 
  n_x = params['num_block_x'] if 'num_block_x' in params else 8; 
  n_y = params['num_block_y'] if 'num_block_y' in params else 8; 
  n_z = params['num_block_z'] if 'num_block_z' in params else 1; 
  num_vox_x = params['num_vox_x'] if 'num_vox_x' in params else 1536
  num_vox_y = params['num_vox_y'] if 'num_vox_y' in params else 1536
  num_vox_z = params['num_vox_z'] if 'num_vox_z' in params else 512

  #max mb per block, init level, and init prob
  max_data_mb = params['max_block_mb'] if 'max_block_mb' in params else 1000.0; 
  p_init = params['p_init'] if 'p_init' in params else .01; 
  max_level = params['max_tree_level'] if 'max_tree_level' in params else 4; 
  vox_length = params['vox_length'] if 'vox_length' in params else 1.0; 
  sb_length = params['sub_block_length'] if 'sub_block_length' in params else .125; 
  fname = params['filename'] if 'filename' in params else "scene"; 
  
  #reconcile sub block length vs voxel length
  if 'sub_block_length' in params : 
    vox_length = sb_length / 8.0; 
  elif 'vox_length' in params  : 
    sb_length = vox_length * 8 ; 

  #run create scene process
  boxm2_batch.init_process("boxm2CreateSceneProcess");
  boxm2_batch.set_input_string(0,scene_dir);
  boxm2_batch.set_input_string(1,app_model);
  boxm2_batch.set_input_string(2,obs_model);
  boxm2_batch.set_input_float(3,orig_x);
  boxm2_batch.set_input_float(4,orig_y);
  boxm2_batch.set_input_float(5,orig_z);
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);

  #calc number of sub blocks in each block
  num_sb_x = num_vox_x / 8; 
  num_sb_y = num_vox_y / 8; 
  num_sb_z = num_vox_z / 8; 
  num_x = num_sb_x / n_x; 
  num_y = num_sb_y / n_y; 
  num_z = num_sb_z / n_z; 

  for k in range(0,n_z):
   for i in range(0,n_x):
    for j in range(0,n_y):
     local_origin_z = k*num_z*sb_length + orig_z;
     local_origin_y = j*num_y*sb_length + orig_y;
     local_origin_x = i*num_x*sb_length + orig_x;
     
     print("Adding block: ", i," ",j," ",k);
     boxm2_batch.init_process("boxm2AddBlockProcess");
     boxm2_batch.set_input_from_db(0,scene);
     boxm2_batch.set_input_unsigned(1,i);
     boxm2_batch.set_input_unsigned(2,j);
     boxm2_batch.set_input_unsigned(3,k);
     boxm2_batch.set_input_unsigned(4,num_x);
     boxm2_batch.set_input_unsigned(5,num_y);
     boxm2_batch.set_input_unsigned(6,num_z);
     boxm2_batch.set_input_unsigned(7,max_level);
     boxm2_batch.set_input_float(8,local_origin_x);
     boxm2_batch.set_input_float(9,local_origin_y);
     boxm2_batch.set_input_float(10,local_origin_z);
     boxm2_batch.set_input_float(11,sb_length);
     boxm2_batch.set_input_float(12,max_data_mb);
     boxm2_batch.set_input_float(13,p_init);
     boxm2_batch.run_process();
       
  print("Write Scene");
  boxm2_batch.init_process("boxm2WriteSceneXMLProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_string(1, fname); 
  boxm2_batch.run_process();

def roi_init(NITF_path, camera, scene, convert_to_8bit, params_fname) : 
  boxm2_batch.init_process("boxm2RoiInitProcess")
  boxm2_batch.set_params_process(params_fname)
  boxm2_batch.set_input_string(0, NITF_path)
  boxm2_batch.set_input_from_db(1,camera)
  boxm2_batch.set_input_from_db(2,scene)
  boxm2_batch.set_input_bool(3,convert_to_8bit)
  result = boxm2_batch.run_process()
  if result:
    (id,type) = boxm2_batch.commit_output(0)
    local_cam = dbvalue(id,type)
    (id,type) = boxm2_batch.commit_output(1)
    cropped_image = dbvalue(id,type)
    (id,type) = boxm2_batch.commit_output(2)
    uncertainty = boxm2_batch.get_output_float(id)
  else:
    local_cam = 0
    cropped_image = 0
    uncertainty = 0
  return result, local_cam, cropped_image, uncertainty 



######################################################################
# blob detection methods
#####################################################################
#runs blob change detection process
def blob_change_detection( change_img, thresh ) : 
  boxm2_batch.init_process("boxm2BlobChangeDetectionProcess");
  boxm2_batch.set_input_from_db(0,change_img);
  boxm2_batch.set_input_float(1, thresh); 
  boxm2_batch.run_process();

#pixel wise roc process for change detection images
def blob_precision_recall(cd_img, gt_img, mask_img=None) :
  boxm2_batch.init_process("boxm2BlobPrecisionRecallProcess");
  boxm2_batch.set_input_from_db(0,cd_img);
  boxm2_batch.set_input_from_db(1,gt_img);
  if mask_img:
    boxm2_batch.set_input_from_db(2,mask_img);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  precision = boxm2_batch.get_bbas_1d_array_float(id);
  (id,type) = boxm2_batch.commit_output(1);
  recall    = boxm2_batch.get_bbas_1d_array_float(id);

  #return tuple of true positives, true negatives, false positives, etc..
  return (precision, recall);

