from boxm2_register import boxm2_batch, dbvalue; 
import os

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch 
# code more readable/refactored
#############################################################################

# Print ocl info for all devices
def ocl_info():
  print("Init Manager");
  boxm2_batch.init_process("boclInitManagerProcess");
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0);
  mgr = dbvalue(id, type);

  print("Get OCL info");
  boxm2_batch.init_process("bocl_info_process");
  boxm2_batch.set_input_from_db(0,mgr)
  boxm2_batch.run_process();




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

# describe scene process, returns the path containing scene data
def describe_scene(scene):
  boxm2_batch.init_process("boxm2DescribeSceneProcess");
  boxm2_batch.set_input_from_db(0, scene);
  boxm2_batch.run_process();
  (id, type) = boxm2_batch.commit_output(0); 
  dataPath = boxm2_batch.get_output_string(id); 
  (id, type) = boxm2_batch.commit_output(1);
  appType = boxm2_batch.get_output_string(id);
  description = { 
                  'dataPath': dataPath, 
                  'appType': appType, 
                }
  return description; 
  
# returns bounding box as two tuple points (minpt, maxpt)
def scene_bbox(scene):
  boxm2_batch.init_process("boxm2SceneBboxProcess");
  boxm2_batch.set_input_from_db(0, scene);
  boxm2_batch.run_process();
  out = []
  for outIdx in range(6):
    (id, type) = boxm2_batch.commit_output(outIdx); 
    pt = boxm2_batch.get_output_double(id); 
    out.append(pt);
  minPt = (out[0], out[1], out[2]);
  maxPt = (out[3], out[4], out[5]);
  return (minPt, maxPt); 
  
###############################################
# Model building stuff
###############################################
# Generic update - will use GPU if device/openclcache are passed in
def update_grey(scene, cache, cam, img, device=None, ident="", mask=None, update_alpha=True, var=-1.0) :
  #If no device is passed in, do cpu update
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU update";
    boxm2_batch.init_process("boxm2CppUpdateImageProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_from_db(3,img);
    return boxm2_batch.run_process();
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
    boxm2_batch.set_input_float(8, var);
    return boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    return False;

# Update with alternate possible pixel explaination - uses GPU
def update_grey_with_alt(scene, cache, cam, img, device=None, ident="", mask=None, update_alpha=True, var=-1.0, alt_prior=None, alt_density=None) :
  #If no device is passed in, do cpu update
  if cache.type == "boxm2_cache_sptr" :
    print "ERROR: CPU update not implemented for update_with_alt";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    print("boxm2_batch GPU update with alt");
    boxm2_batch.init_process("boxm2OclUpdateWithAltProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_from_db(4,img);
    boxm2_batch.set_input_string(5,ident);
    if mask :
      boxm2_batch.set_input_from_db(6,mask);
    boxm2_batch.set_input_bool(7, update_alpha); 
    boxm2_batch.set_input_float(8, var);
    boxm2_batch.set_input_from_db(9, alt_prior)
    boxm2_batch.set_input_from_db(10, alt_density)
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 


def update_app_grey(scene, cache, cam, img, device=None) :
  #If no device is passed in, do cpu update
  print("boxm2 GPU App online update");
  boxm2_batch.init_process("boxm2OclPaintOnlineProcess");
  boxm2_batch.set_input_from_db(0,device);
  boxm2_batch.set_input_from_db(1,scene);
  boxm2_batch.set_input_from_db(2,cache);
  boxm2_batch.set_input_from_db(3,img);
  boxm2_batch.set_input_from_db(4,cam);
  boxm2_batch.run_process();
def update_hist_app_grey(scene, cache, cam, img, device=None) :
  #If no device is passed in, do cpu update
  print("boxm2 GPU App online update");
  boxm2_batch.init_process("boxm2OclUpdateHistogramAppProcess");
  boxm2_batch.set_input_from_db(0,device);
  boxm2_batch.set_input_from_db(1,scene);
  boxm2_batch.set_input_from_db(2,cache);
  boxm2_batch.set_input_from_db(3,img);
  boxm2_batch.set_input_from_db(4,cam);
  boxm2_batch.run_process();    
####################################################################
# Generic update - will use GPU if device/openclcache are passed in
####################################################################
def update_rgb(scene, cache, cam, img, device=None, mask="", updateAlpha=True) :
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
    boxm2_batch.set_input_string(5, "");  #identifier
    boxm2_batch.set_input_string(6, mask) #mask file
    boxm2_batch.set_input_bool(7, updateAlpha);
    boxm2_batch.run_process();
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 

def update_with_quality(scene, cache, cam, img, q_img, identifier="") :
  boxm2_batch.init_process("boxm2CppUpdateUsingQualityProcess");
  boxm2_batch.set_input_from_db(0, scene);
  boxm2_batch.set_input_from_db(1, cache);
  boxm2_batch.set_input_from_db(2, cam);
  boxm2_batch.set_input_from_db(3, img);
  boxm2_batch.set_input_from_db(4, q_img);
  boxm2_batch.set_input_string(5, identifier);
  boxm2_batch.run_process();
  
# Generic update - will use GPU if device/openclcache are passed in
def update_cpp(scene, cache, cam, img, ident="") :
  boxm2_batch.init_process("boxm2CppUpdateImageProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_from_db(1,cache);
  boxm2_batch.set_input_from_db(2,cam);
  boxm2_batch.set_input_from_db(3,img);
  boxm2_batch.set_input_string(4,ident);
  boxm2_batch.run_process();

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
def ingest_height_map(scene, cache,x_img,y_img,z_img, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_adaptor, render height map cpp process not implemented"; 

  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclIngestDemProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,z_img);
    boxm2_batch.set_input_from_db(4,x_img);
    boxm2_batch.set_input_from_db(5,y_img);
    boxm2_batch.run_process();
    return ; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type;
    
# Ingest a Buckeye-Style DEM, i.e. first return and last return image pair
def ingest_buckeye_dem(scene, cache, first_return_fname, last_return_fname, geoid_height, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_adaptor, ingest_buckeye cpp process not implemented"; 

  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclIngestBuckeyeDemProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_string(3,first_return_fname);
    boxm2_batch.set_input_string(4,last_return_fname);
    boxm2_batch.set_input_float(5,geoid_height);
    boxm2_batch.run_process();
    return ; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type;
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################
def render_grey(scene, cache, cam, ni=1280, nj=720, device=None, ident_string="") :
  if cache.type == "boxm2_cache_sptr" :
    boxm2_batch.init_process("boxm2CppRenderExpectedImageProcess");
    boxm2_batch.set_input_from_db(0,scene);
    boxm2_batch.set_input_from_db(1,cache);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_unsigned(3,ni);
    boxm2_batch.set_input_unsigned(4,nj);
    boxm2_batch.set_input_string(5,ident_string);
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
    boxm2_batch.set_input_string(6,ident_string);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image; 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################
def render_grey_and_vis(scene, cache, cam, ni=1280, nj=720, device=None) :
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU render grey and vis not yet implemented";
    return; 
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
    (id,type) = boxm2_batch.commit_output(1);
    vis_image = dbvalue(id,type); 
    return exp_image,vis_image; 
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
    (id,type) = boxm2_batch.commit_output(1); 
    var_image = dbvalue(id,type); 
    return exp_image, var_image 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
    
#####################################################################    
# render image of expected z values 
#####################################################################
def render_z_image(scene, cache, cam, ni=1280, nj=720, normalize = False, device=None) : 
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU render depth not yet implemented";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclRenderExpectedZImageProcess");
    boxm2_batch.set_input_from_db(0,device);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cache);
    boxm2_batch.set_input_from_db(3,cam);
    boxm2_batch.set_input_unsigned(4,ni);
    boxm2_batch.set_input_unsigned(5,nj);
    boxm2_batch.set_input_bool(6,normalize)
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    z_exp_image = dbvalue(id,type);
    (id,type) = boxm2_batch.commit_output(1); 
    z_var_image = dbvalue(id,type); 
    return z_exp_image, z_var_image 
  else : 
    print "ERROR: Cache type not recognized: ", cache.type; 
#####################################################################
# change detection wrapper
#####################################################################
def change_detect(scene, cache, cam, img, exp_img, device=None, rgb=False, n=1, raybelief="", max_mode=False) : 
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
    boxm2_batch.set_input_bool(8, max_mode); 
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
    

####################################################################
# Visualize Change Wrapper
####################################################################
def visualize_change(change_img, in_img, thresh=.5) : 
  boxm2_batch.init_process("boxm2OclVisualizeChangeProcess"); 
  boxm2_batch.set_input_from_db(0,change_img); 
  boxm2_batch.set_input_from_db(1,in_img); 
  boxm2_batch.set_input_float(2,thresh);
  boxm2_batch.run_process(); 
  (id,type) = boxm2_batch.commit_output(0); 
  vis_img = dbvalue(id,type); 
  return vis_img; 

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
    nCells = boxm2_batch.get_output_int(id); 
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
def write_cache(cache, do_clear = 0) : 
  if cache.type == "boxm2_cache_sptr" : 
    boxm2_batch.init_process("boxm2WriteCacheProcess");
    boxm2_batch.set_input_from_db(0,cache);
    boxm2_batch.set_input_bool(1,do_clear);
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

def trajectory_size(trajectory):
  boxm2_batch.init_process("boxm2ViewTrajectorySizeProcess")
  boxm2_batch.set_input_from_db(0, trajectory)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  size = boxm2_batch.get_output_unsigned(id)
  boxm2_batch.remove_data(id)
  return size

def trajectory_direct(trajectory, index):
  boxm2_batch.init_process("boxm2ViewTrajectoryDirectProcess")
  boxm2_batch.set_input_from_db(0,trajectory)
  boxm2_batch.set_input_unsigned(1,index)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  cam = dbvalue(id,type)
  return cam

######################################################################
# camera/scene methods
#####################################################################
def bundle2scene(bundle_file, img_dir, app_model="boxm2_mog3_grey", out_dir="nvm_out") : 
  if app_model == "boxm2_mog3_grey" or app_model =="boxm2_mog3_grey_16": 
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
  init_level = params['init_tree_level'] if 'init_tree_level' in params else 1;
  vox_length = params['vox_length'] if 'vox_length' in params else 1.0; 
  sb_length = params['sub_block_length'] if 'sub_block_length' in params else .125; 
  fname = params['filename'] if 'filename' in params else "scene"; 

  #reconcile sub block length vs voxel length
  if 'sub_block_length' in params : 
    vox_length = sb_length / 8.0; 
  elif 'vox_length' in params  : 
    sb_length = vox_length * 8 ; 

  #set up tuples
  if 'origin' in params: 
    orig_x, orig_y, orig_z = params['origin']
  if 'num_vox' in params:
    num_vox_x, num_vox_y, num_vox_z = params['num_vox']
  if 'num_blocks' in params:
    n_x, n_y, n_z = params['num_blocks']

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
     boxm2_batch.set_input_unsigned(14,init_level);
     boxm2_batch.run_process();
       
  print("Write Scene");
  boxm2_batch.init_process("boxm2WriteSceneXMLProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_string(1, fname); 
  boxm2_batch.run_process();

def roi_init(NITF_path, camera, scene, convert_to_8bit, params_fname, margin=0) : 
  boxm2_batch.init_process("boxm2RoiInitProcess")
  boxm2_batch.set_params_process(params_fname)
  boxm2_batch.set_input_string(0, NITF_path)
  boxm2_batch.set_input_from_db(1,camera)
  boxm2_batch.set_input_from_db(2,scene)
  boxm2_batch.set_input_bool(3,convert_to_8bit)
  boxm2_batch.set_input_int(4, margin)
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

def create_mask_image(scene, camera, ni, nj, ground_plane_only=False) :
  boxm2_batch.init_process("boxm2CreateSceneMaskProcess")
  boxm2_batch.set_input_from_db(0,scene)
  boxm2_batch.set_input_from_db(1,camera)
  boxm2_batch.set_input_unsigned(2,ni);
  boxm2_batch.set_input_unsigned(3,nj);
  boxm2_batch.set_input_bool(4,ground_plane_only)
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0)
  mask = dbvalue(id,type)
  return mask


######################################################################
# blob detection methods
#####################################################################
#runs blob change detection process
def blob_change_detection( change_img, thresh, depth1=None, depth2=None ) : 
  boxm2_batch.init_process("boxm2BlobChangeDetectionProcess")
  boxm2_batch.set_input_from_db(0,change_img)
  boxm2_batch.set_input_float(1, thresh)
  if( depth1 and depth2 ):
    boxm2_batch.set_input_from_db(2,depth1)
    boxm2_batch.set_input_from_db(3,depth2)
  boxm2_batch.run_process()
  (id,type) = boxm2_batch.commit_output(0)
  blobImg = dbvalue(id,type) 
  return blobImg

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


#########################################################################
#Batch update process
#########################################################################
def update_aux_per_view(scene, cache, img, cam, imgString, device=None, mask=None) : 
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU update aux per view not yet implemented";
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclUpdateAuxPerViewProcess");
    boxm2_batch.set_input_from_db(0, device);
    boxm2_batch.set_input_from_db(1, scene);
    boxm2_batch.set_input_from_db(2, cache);
    boxm2_batch.set_input_from_db(3, cam);
    boxm2_batch.set_input_from_db(4, img);
    boxm2_batch.set_input_string(5, imgString);
    if mask:
        boxm2_batch.set_input_from_db(6,mask)
    boxm2_batch.run_process();
  else:
    print "ERROR: Cache type not recognized: ", cache.type; 
   
# Update Aux for normal-albedo-array appearance model
def update_aux_per_view_naa(scene, cache, img, cam, metadata, atm_params, imgString, alt_prior, alt_density, device=None) : 
  if cache.type == "boxm2_cache_sptr" :
    print "boxm2_batch CPU update aux per view_naa not yet implemented"
  elif cache.type == "boxm2_opencl_cache_sptr" and device : 
    boxm2_batch.init_process("boxm2OclUpdateAuxPerViewNAAProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, cache)
    boxm2_batch.set_input_from_db(3, cam)
    boxm2_batch.set_input_from_db(4, img)
    boxm2_batch.set_input_from_db(5, metadata)
    boxm2_batch.set_input_from_db(6, atm_params)
    boxm2_batch.set_input_string(7, imgString)
    boxm2_batch.set_input_from_db(8, alt_prior)
    boxm2_batch.set_input_from_db(9, alt_density)
    boxm2_batch.run_process();
  else:
    print "ERROR: Cache type not recognized: ", cache.type; 
   
###########################################################
# create sun camera
# astro_coords=True indicates az,el in degrees north of east, degrees above horizon
def compute_sun_affine_camera(scene, sun_az, sun_el, astro_coords = True):
       boxm2_batch.init_process("boxm2ComputeSunAffineCameraProcess")
       boxm2_batch.set_input_from_db(0,scene)
       boxm2_batch.set_input_float(1,sun_el)
       boxm2_batch.set_input_float(2,sun_az)
       boxm2_batch.set_input_bool(3,astro_coords)
       boxm2_batch.run_process()
       (id,type) = boxm2_batch.commit_output(0)
       sun_cam = dbvalue(id,type)
       (ni_id,type) = boxm2_batch.commit_output(1)
       (nj_id,type) = boxm2_batch.commit_output(2)
       ni = boxm2_batch.get_output_unsigned(ni_id)
       nj = boxm2_batch.get_output_unsigned(nj_id)
       boxm2_batch.remove_data(ni_id)
       boxm2_batch.remove_data(nj_id)
       return sun_cam, ni, nj
    
    
#######################################################
# update sun visibility probabilities
def update_sun_visibilities(scene,device,ocl_cache,cache,sun_camera,ni,nj,prefix_name):
    boxm2_batch.init_process("boxm2OclUpdateSunVisibilitiesProcess")
    boxm2_batch.set_input_from_db(0,device)
    boxm2_batch.set_input_from_db(1,scene)
    boxm2_batch.set_input_from_db(2,ocl_cache)
    boxm2_batch.set_input_from_db(3,cache)
    boxm2_batch.set_input_from_db(4,sun_camera)
    boxm2_batch.set_input_unsigned(5, ni)
    boxm2_batch.set_input_unsigned(6, nj)
    boxm2_batch.set_input_string(7,prefix_name)
    boxm2_batch.run_process()

#######################################################
# render shadow map 
def render_shadow_map(scene,device, ocl_cache, camera, ni, nj, prefix_name=''):
    boxm2_batch.init_process("boxm2OclRenderExpectedShadowMapProcess")
    boxm2_batch.set_input_from_db(0,device)
    boxm2_batch.set_input_from_db(1,scene)
    boxm2_batch.set_input_from_db(2,ocl_cache)
    boxm2_batch.set_input_from_db(3,camera)
    boxm2_batch.set_input_unsigned(4, ni)
    boxm2_batch.set_input_unsigned(5, nj)
    boxm2_batch.set_input_string(6,prefix_name)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    shadow_map = dbvalue(id,type)
    return shadow_map
    
def scene_illumination_info(scene):
    boxm2_batch.init_process("boxm2SceneIlluminationInfoProcess");
    boxm2_batch.set_input_from_db(0, scene);
    boxm2_batch.run_process();
    (lon_id,lon_type)=boxm2_batch.commit_output(0);
    longitude = boxm2_batch.get_output_float(lon_id)
    (lat_id,lat_type)=boxm2_batch.commit_output(1);
    latitude = boxm2_batch.get_output_float(lat_id)
    (nb_id,nb_type)=boxm2_batch.commit_output(2);
    nbins = boxm2_batch.get_output_int(nb_id);
    return longitude, latitude, nbins

