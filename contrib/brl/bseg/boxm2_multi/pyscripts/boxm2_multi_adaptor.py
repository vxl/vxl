from boxm2_register import boxm2_batch, dbvalue;

#############################################################################
# PROVIDES higher level python functions to make boxm2_batch
# code more readable/refactored
#############################################################################


#does the opencl prep work on an input scene
def load_multi_cache(scene, numdevices=1):
  ###############################################################
  # Create multi cache, opencl manager, device, and gpu cache
  ###############################################################
  #print("Create Main Cache");
  boxm2_batch.init_process("boxm2CreateMultiCacheProcess");
  boxm2_batch.set_input_from_db(0,scene);
  boxm2_batch.set_input_int(1,numdevices);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  mcache = dbvalue(id, type);

  return mcache


###############################################
# Model building stuff
###############################################
# Generic update - will use GPU if device/openclcache are passed in
def update_multi(scene, mcache, cam, img,  ident="", update_alpha=True, var=-1.0,update_app=True,tnear = 100000.0 , tfar = 0.000001 ) :
  #If no device is passed in, do cpu update
    print("boxm2_batch multi GPU update");
    boxm2_batch.init_process("boxm2MultiUpdateProcess");
    boxm2_batch.set_input_from_db(0,mcache);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_from_db(3,img);
    boxm2_batch.set_input_string(4,ident);
    boxm2_batch.set_input_bool(5, update_alpha);
    boxm2_batch.set_input_float(6, var);
    boxm2_batch.set_input_bool(7, update_app);
    boxm2_batch.set_input_float(8, tnear);
    boxm2_batch.set_input_float(9, tfar);
    return boxm2_batch.run_process();
#####################################################################
# Generic render, returns a dbvalue expected image
# Cache can be either an OPENCL cache or a CPU cache
#####################################################################
def render_multi(scene, mcache, cam, ni=1280, nj=720, ident_string="",tnear=100000.0,tfar=100000.0) :
    boxm2_batch.init_process("boxm2MultiRenderProcess");
    boxm2_batch.set_input_from_db(0,mcache);
    boxm2_batch.set_input_from_db(1,scene);
    boxm2_batch.set_input_from_db(2,cam);
    boxm2_batch.set_input_unsigned(3,ni);
    boxm2_batch.set_input_unsigned(4,nj);
    boxm2_batch.set_input_string(5,ident_string);
    boxm2_batch.set_input_float(6,tnear);
    boxm2_batch.set_input_float(7,tfar);
    boxm2_batch.run_process();
    (id,type) = boxm2_batch.commit_output(0);
    exp_image = dbvalue(id,type);
    return exp_image;


######################################################################
# cache methods
#####################################################################
#generic write cache to disk
def write_mcache(mcache, do_clear = 0) :
    boxm2_batch.init_process("boxm2WriteMultiCacheProcess");
    boxm2_batch.set_input_from_db(0,mcache);
    boxm2_batch.set_input_bool(1,do_clear);
    boxm2_batch.run_process();


