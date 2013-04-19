from bstm_register import bstm_batch, dbvalue;
import os
#############################################################################
# PROVIDES higher level python functions to make bstm_batch
# code more readable/refactored
#############################################################################

# Print ocl info for all devices
def ocl_info():
  #print("Init Manager");
  bstm_batch.init_process("boclInitManagerProcess");
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  mgr = dbvalue(id, type);

  print("Get OCL info");
  bstm_batch.init_process("bocl_info_process");
  bstm_batch.set_input_from_db(0,mgr)
  bstm_batch.run_process();

def load_scene(scene_str):
  #print("Loading a Scene from file: ", scene_str);
  bstm_batch.init_process("bstmLoadSceneProcess");
  bstm_batch.set_input_string(0, scene_str);
  bstm_batch.run_process();
  (scene_id, scene_type) = bstm_batch.commit_output(0);
  scene = dbvalue(scene_id, scene_type);
  return scene;


#does the opencl prep work on an input scene
def load_opencl(scene_str, device_string="gpu"):
  scene = load_scene(scene_str);

  ###############################################################
  # Create cache, opencl manager, device, and gpu cache
  ###############################################################
  #print("Create Main Cache");
  bstm_batch.init_process("bstmCreateCacheProcess");
  bstm_batch.set_input_from_db(0,scene);
  bstm_batch.set_input_string(1,"lru");
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cache = dbvalue(id, type);

  #print("Init Manager");
  bstm_batch.init_process("boclInitManagerProcess");
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  mgr = dbvalue(id, type);

  #print("Get Gpu Device");
  bstm_batch.init_process("boclGetDeviceProcess");
  bstm_batch.set_input_string(0,device_string)
  bstm_batch.set_input_from_db(1,mgr)
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  device = dbvalue(id, type);

  #print("Create Gpu Cache");
  bstm_batch.init_process("bstmOclCreateCacheProcess");
  bstm_batch.set_input_from_db(0,device)
  bstm_batch.set_input_from_db(1,scene)
  bstm_batch.run_process();
  (id, type) = bstm_batch.commit_output(0);
  openclcache = dbvalue(id, type);

  return scene, cache, mgr, device, openclcache;

#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################
def render(scene, cache, cam, time=0, ni=1624, nj=1224, render_label=False, device=None,ident="") :
  if cache.type == "bstm_cache_sptr" :
    print "bstm_batch CPU render grey and vis not yet implemented";
    return;
  elif cache.type == "bstm_opencl_cache_sptr" and device :
    bstm_batch.init_process("bstmOclRenderExpectedImageProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_unsigned(4,ni);
    bstm_batch.set_input_unsigned(5,nj);
    bstm_batch.set_input_float(6,time);
    bstm_batch.set_input_bool(7,render_label);
    bstm_batch.run_process();
    (id,type) = bstm_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    (id,type) = bstm_batch.commit_output(1);
    vis_image = dbvalue(id,type);
    return exp_image,vis_image;
  else :
    print "ERROR: Cache type not recognized: ", cache.type;


######################################################################
# cache methods
#####################################################################
#generic write cache to disk
def write_cache(cache, do_clear = 0) :
  if cache.type == "bstm_cache_sptr" :
    bstm_batch.init_process("bstmWriteCacheProcess");
    bstm_batch.set_input_from_db(0,cache);
    bstm_batch.set_input_bool(1,do_clear);
    bstm_batch.run_process();
  else :
    print "ERROR: Cache type needs to be bstm_cache_sptr, not ", cache.type;

#generic clear cache
def clear_cache(cache) :
  if cache.type == "bstm_cache_sptr" :
    bstm_batch.init_process("bstmClearCacheProcess");
    bstm_batch.set_input_from_db(0,cache);
    bstm_batch.run_process();
  elif cache.type == "bstm_opencl_cache_sptr" :
    bstm_batch.init_process("bstmOclClearCacheProcess");
    bstm_batch.set_input_from_db(0,cache);
    bstm_batch.run_process();
  else :
    print "ERROR: Cache type needs to be bstm_cache_sptr, not ", cache.type;


######################################################################
# trajectory methods
#####################################################################
def init_trajectory(scene, startInc, endInc, radius, ni=1280, nj=720) :
  bstm_batch.init_process("bstmViewInitTrajectoryProcess");
  bstm_batch.set_input_from_db(0, scene);
  bstm_batch.set_input_double(1, float(startInc) );  #incline0
  bstm_batch.set_input_double(2, float(endInc) ); #incline1
  bstm_batch.set_input_double(3, float(radius) ); #radius
  bstm_batch.set_input_unsigned(4, ni); #ni
  bstm_batch.set_input_unsigned(5, nj); #nj
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  trajectory = dbvalue(id,type);
  return trajectory; 

def trajectory_next(trajectory) :
  bstm_batch.init_process("bstmViewTrajectoryNextProcess");
  bstm_batch.set_input_from_db(0, trajectory);
  bstm_batch.run_process();
  (id,type) = bstm_batch.commit_output(0);
  cam = dbvalue(id,type);
  return cam;

def trajectory_size(trajectory):
  bstm_batch.init_process("bstmViewTrajectorySizeProcess")
  bstm_batch.set_input_from_db(0, trajectory)
  bstm_batch.run_process()
  (id,type) = bstm_batch.commit_output(0)
  size = bstm_batch.get_output_unsigned(id)
  bstm_batch.remove_data(id)
  return size


