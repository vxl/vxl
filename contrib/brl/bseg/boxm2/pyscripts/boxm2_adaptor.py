#############################################################################
# PROVIDES higher level python functions to make boxm2_batch 
# code more readable/refactored
#
# to use our boxm2 python binding, be sure to add: 
#   <vxl_build_root>/lib/:<vxl_src_root>/contrib/brl/bseg/boxm2/pyscripts/ 
# to your PYTHONPATH environment variable.  
#############################################################################
import boxm2_batch,os;
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();

#class used for python/c++ pointers in database
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string
    
    
# boxm2_adaptor class offers super simple model manipulation syntax
# you can always force the process to use CPP by just passing in "cpp" as the last
# arg to any function in this class
class boxm2_adaptor: 

  #scene adaptor init
  def __init__(self, scene_str, rgb=False, device_string="gpu"):
    
    #init (list) self vars
    self.rgb = rgb; 
    self.scene = None; 
    self.active_cache = None; 
    self.device_string = None; 
    self.cpu_cache = None;
    self.device = None; 
    self.opencl_cache = None; 
    self.ocl_mgr = None; 
   
    #if device_string is gpu, load up opencl
    if device_string[0:3]=="gpu" :  
      self.scene, self.cpu_cache, self.ocl_mgr, self.device, self.opencl_cache = load_opencl(scene_str); 
      self.active_cache = self.opencl_cache; 
    elif device_string[0:3]=="cpp" : 
      scene.scene, self.cpu_cache = load_cpp(scene_str);     
      self.active_cache = self.cpu_cache; 
  
  #update wrapper, can pass in a Null device to use 
  def update(self, cam, img, mask_path="", device_string="") :
    cache = self.active_cache; 
    dev = self.device; 
    
    #check if force gpu or cpu
    if device_string=="gpu" : 
      cache = self.opencl_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache; 
      dev = None; 
      
    #run update grey or RGB
    if self.rgb :
      update_rgb(self.scene, cache, cam, img, dev); 
    else :
      update_grey(self.scene, cache, cam, img, dev); 
      
  #render wrapper, same as above
  def render(self, cam, ni=1280, nj=720, device_string="") : 
    cache = self.active_cache; 
    dev = self.device; 
    #check if force gpu or cpu
    if device_string=="gpu" : 
      cache = self.opencl_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache; 
      dev = None; 
    if self.rgb : 
      expimg = render_rgb(self.scene, cache, cam, ni, nj, dev); 
    else : 
      expimg = render_grey(self.scene, cache, cam, ni, nj, dev); 
    return expimg; 
  
  #render heigh map render
  def render_height_map(self, device_string="") : 
    cache = self.active_cache; 
    dev = self.device;
    if device_string=="gpu" : 
      cache = self.opencl_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache; 
      dev = None; 
    z_image, var_image, x_image, y_image, prob_image, app_image = render_height_map(self.scene, cache, dev); 
    return z_image, var_image, x_image, y_image, prob_image, app_image; 

  # detect change wrapper, 
  def change_detect(self, cam, img, exp_img, device_string="") : 
    cache = self.active_cache; 
    dev = self.device; 
    if device_string=="gpu" : 
      cache = self.opencel_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache;
      dev = None;
    cd_img = change_detect(self.scene,cache,cam,img,exp_img,dev); 
    return cd_img; 
  
  def refine(self, thresh=0.3, device_string="") :
    if device_string=="":
      refine(self.scene, self.active_cache, thresh, self.device); 
    elif device_string=="gpu" :
      refine(self.scene, self.opencl_cache, thresh, self.device); 
    elif device_string=="cpp" :
      refine(self.scene, self.cpu_cache, thresh, None); 

  def merge(self, thresh=0.3, device_string="") :
    if device_string=="":
      merge(self.scene, self.active_cache, thresh, self.device); 
    elif device_string=="gpu" :
      merge(self.scene, self.opencl_cache, thresh, self.device); 
    elif device_string=="cpp" :
      merge(self.scene, self.cpu_cache, thresh, None); 

  def median_filter(self, device_string="") : 
    if device_string=="":
      median_filter(self.scene, self.active_cache, self.device); 
    elif device_string=="gpu" : 
      median_filter(self.scene, self.opencl_cache, self.device); 
    elif device_string=="cpp" :
      median_filter(self.scene, self.cpu_cache, None); 

  #only write the cpu_cache to disk
  def write_cache(self): 
    write_cache(self.cpu_cache); 
       

#does the opencl prep work on an input scene
def load_opencl(scene_str, device_string="gpu"):
  print("Loading a Scene from file: ", scene_str);
  boxm2_batch.init_process("boxm2LoadSceneProcess");
  boxm2_batch.set_input_string(0, scene_str);
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);

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
  print("Loading a Scene from file: ", scene_str);
  boxm2_batch.init_process("boxm2LoadSceneProcess");
  boxm2_batch.set_input_string(0, scene_str);
  boxm2_batch.run_process();
  (scene_id, scene_type) = boxm2_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);

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
def update_grey(scene, cache, cam, img, device=None, mask="") :
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
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    
# Generic update - will use GPU if device/openclcache are passed in
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
 
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
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
    
# Generic render, returns a dbvalue expected image
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
    
def change_detect(scene, cache, cam, img, exp_img, device=None) : 
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
    boxm2_batch.run_process(); 
    (id,type) = boxm2_batch.commit_output(0); 
    cd_img = dbvalue(id,type); 
    return cd_img; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type;
    
#generic refine (will work on color and grey scenes)
def refine(scene, cache, thresh=0.3, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU refine";
    boxm2_batch.init_process("boxm2CppRefineProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_float(2,thresh);
    boxm2_batch.run_process();
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print "boxm2_batch GPU refine"; 
    boxm2_batch.init_process("boxm2OclRefineProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);    
    boxm2_batch.set_input_float(3,thresh);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type unrecognized: ", cache.type; 
    
#generic merge method 
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
    
#generic filter scene, should work with color and grey scenes
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

#generic write cache to disk
def write_cache(cache) : 
  if cache.type == "boxm2_cache_sptr" : 
    boxm2_batch.init_process("boxm2WriteCacheProcess");
    boxm2_batch.set_input_from_db(0,cache);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type needs to be boxm2_cache_sptr, not ", cache.type; 


###################################################
# Camera/Image loading and saving
###################################################
def load_camera(file_path) : 
  boxm2_batch.init_process("vpglLoadPerspectiveCameraProcess"); 
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam; 
  
def persp2gen(pcam, ni, nj) : 
  boxm2_batch.init_process("vpglConvertToGenericCameraProcess"); 
  boxm2_batch.set_input_from_db(0, pcam);
  boxm2_batch.set_input_unsigned(1, ni); 
  boxm2_batch.set_input_unsigned(2, nj); 
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  gcam = dbvalue(id,type);
  return gcam; 

def load_image(file_path) : 
  boxm2_batch.init_process("vilLoadImageViewProcess");
  boxm2_batch.set_input_string(0, file_path);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  (ni_id, ni_type) = boxm2_batch.commit_output(1); 
  (nj_id, nj_type) = boxm2_batch.commit_output(2); 
  ni = boxm2_batch.get_output_unsigned(ni_id); 
  nj = boxm2_batch.get_output_unsigned(nj_id); 
  img = dbvalue(id,type);
  return img, ni, nj; 
  
def save_image(img, file_path) : 
  boxm2_batch.init_process("vilSaveImageViewProcess");
  boxm2_batch.set_input_from_db(0,img);
  boxm2_batch.set_input_string(1,file_path);
  boxm2_batch.run_process();

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
