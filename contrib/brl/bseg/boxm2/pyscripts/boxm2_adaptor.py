#############################################################################
# PROVIDES higher level python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################
import boxm2_batch,os;
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();

#class used for python/c++ pointers in database
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

#does the opencl prep work on an input scene
def load_opencl(scene_str):
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
  boxm2_batch.set_input_string(0,"gpu")
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
  img = dbvalue(id,type);
  return img; 
  
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


      
