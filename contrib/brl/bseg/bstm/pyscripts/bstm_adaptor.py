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
def render(scene, device, cache, cam, time=0, ni=1624, nj=1224, render_label=False) :
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



def render_change(scene, device, cache, cam, time=0, ni=1624, nj=1224) :
  if cache.type == "bstm_cache_sptr" :
    print "bstm_batch CPU render grey and vis not yet implemented";
    return;
  elif cache.type == "bstm_opencl_cache_sptr" and device :
    bstm_batch.init_process("bstmOclRenderExpectedChangeProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_unsigned(4,ni);
    bstm_batch.set_input_unsigned(5,nj);
    bstm_batch.set_input_float(6, time );
    bstm_batch.run_process();
    (id,type) = bstm_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image;
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


# detect change wrapper,
def change_detect(scene, device, cache, cam, img, time, mask_img=None, raybelief="", max_mode=False) :
    bstm_batch.init_process("bstmOclChangeDetectionProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_from_db(4,img);
    bstm_batch.set_input_from_db(5,mask_img);
    bstm_batch.set_input_string(6, raybelief);
    bstm_batch.set_input_bool(7, max_mode);
    bstm_batch.set_input_float(8, time);

    bstm_batch.run_process();
    (id,type) = bstm_batch.commit_output(0);
    cd_img = dbvalue(id,type);
    return cd_img;


def label_change(scene, device, cache, cam, change_img, change_t, label, time) :
    bstm_batch.init_process("bstmOclLabelRayProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_from_db(4,change_img);
    bstm_batch.set_input_float(5, change_t);
    bstm_batch.set_input_float(6, time);
    bstm_batch.set_input_int(7, label);
    bstm_batch.run_process();

# detect change wrapper,
def update(scene, device, cache, cam, img, time, mog_var = -1, mask_img = None, update_alpha = True, update_changes_only = False):
    bstm_batch.init_process("bstmOclUpdateProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_from_db(4,img);
    bstm_batch.set_input_float(5,time);
    bstm_batch.set_input_float(6,mog_var);
    bstm_batch.set_input_from_db(7,mask_img);
    bstm_batch.set_input_bool(8,update_alpha);
    bstm_batch.set_input_bool(9,update_changes_only);

    bstm_batch.run_process();

# detect change wrapper,
def update_color(scene, device, cache, cam, img, time, mog_var = -1, mask_img = None, update_alpha = True):
    bstm_batch.init_process("bstmOclUpdateColorProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_from_db(4,img);
    bstm_batch.set_input_float(5,time);
    bstm_batch.set_input_float(6,mog_var);
    bstm_batch.set_input_from_db(7,mask_img);
    bstm_batch.set_input_bool(8,update_alpha);
    bstm_batch.run_process();

# update change wrapper,
def update_change(scene, device, cache, cam, img, time, mask_img = None):
    bstm_batch.init_process("bstmOclUpdateChangeProcess");
    bstm_batch.set_input_from_db(0,device);
    bstm_batch.set_input_from_db(1,scene);
    bstm_batch.set_input_from_db(2,cache);
    bstm_batch.set_input_from_db(3,cam);
    bstm_batch.set_input_from_db(4,img);
    bstm_batch.set_input_from_db(5,mask_img);
    bstm_batch.set_input_float(6,time);
    bstm_batch.run_process();
    (id,type) = bstm_batch.commit_output(0);
    cd_img = dbvalue(id,type);
    return cd_img;

    # return

def refine(scene, cpu_cache, p_threshold, time):
    bstm_batch.init_process("bstmCppRefineSpacetimeProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cpu_cache);
    bstm_batch.set_input_float(2,p_threshold);
    bstm_batch.set_input_float(3,time);
    bstm_batch.run_process();


def refine_space(scene, cpu_cache, change_prob_t, time):
    bstm_batch.init_process("bstmCppRefineSpaceProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cpu_cache);
    bstm_batch.set_input_float(2,change_prob_t);
    bstm_batch.set_input_float(3,time);
    bstm_batch.run_process();

def refine_time(scene, cpu_cache, change_prob_t, time):
    bstm_batch.init_process("bstmCppRefineTTProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cpu_cache);
    bstm_batch.set_input_float(2,change_prob_t);
    bstm_batch.set_input_float(3,time);
    bstm_batch.run_process();


def merge(scene, cpu_cache, p_threshold, time):
    bstm_batch.init_process("bstmCppMergeTTProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cpu_cache);
    bstm_batch.set_input_float(2,p_threshold);
    bstm_batch.set_input_float(3,time);
    bstm_batch.run_process();

def filter_changes(scene, cpu_cache, time):
    bstm_batch.init_process("bstmCppMajorityFilterProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cpu_cache);
    bstm_batch.set_input_float(2,time);
    bstm_batch.run_process();

def scene_statistics(scene, cache):
    bstm_batch.init_process("bstmSceneStatisticsProcess");
    bstm_batch.set_input_from_db(0, scene );
    bstm_batch.set_input_from_db(1, cache );
    bstm_batch.run_process();
    (s1_id, s1_type) = bstm_batch.commit_output(0);
    (s2_id, s2_type) = bstm_batch.commit_output(1);
    (s3_id, s3_type) = bstm_batch.commit_output(2);
    s1 = bstm_batch.get_output_float(s1_id);
    s2 = bstm_batch.get_output_float(s2_id);
    s3 = bstm_batch.get_output_unsigned(s3_id);
    return [s1,s2,s3]

def label_tt_depth(scene, cache):
    bstm_batch.init_process("bstmCppLabelTTDepthProcess");
    bstm_batch.set_input_from_db(0, scene );
    bstm_batch.set_input_from_db(1, cache );
    bstm_batch.run_process();

def export_pt_cloud(scene, cache, output_filename, prob_t, time, output_aux=True):
    bstm_batch.init_process("bstmCppExtractPointCloudProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cache);
    bstm_batch.set_input_float(2, prob_t);
    bstm_batch.set_input_float(3, time);
    bstm_batch.run_process();

    bstm_batch.init_process("bstmCppExportPointCloudProcess");
    bstm_batch.set_input_from_db(0,scene);
    bstm_batch.set_input_from_db(1,cache);
    bstm_batch.set_input_string(2, output_filename);
    bstm_batch.set_input_bool(3, output_aux);
    bstm_batch.set_input_float(4, time);

    bstm_batch.run_process();
    return;
