from boxm2_adaptor import *
from vil_adaptor import *;
from vpgl_adaptor import *;
from os.path import basename, splitext

#############################################################################
# boxm2_scene_adaptor class offers super simple model manipulation syntax
# you can always force the process to use CPP by just passing in "cpp" as the last
# arg to any function in this class
#############################################################################
class boxm2_scene_adaptor:

  #scene adaptor init
  def __init__(self, scene_str, rgb=False, device_string="gpu") :

    #init (list) self vars
    self.rgb = rgb;
    self.scene = None;
    self.active_cache = None;
    self.device_string = None;
    self.cpu_cache = None;
    self.device = None;
    self.opencl_cache = None;
    self.ocl_mgr = None;
    self.str_cache = None; 
    self.model_dir = None; 

    #if device_string is gpu, load up opencl
    if device_string[0:3]=="gpu" :
      self.scene, self.cpu_cache, self.ocl_mgr, self.device, self.opencl_cache = load_opencl(scene_str, device_string);
      self.active_cache = self.opencl_cache;
    elif device_string[0:3]=="cpp" :
      self.scene, self.cpu_cache = load_cpp(scene_str);
      self.active_cache = self.cpu_cache;
    
    #store model directory for later use
    self.model_dir = self.describe(); 

  #describe scene
  def describe(self) :
    return describe_scene(self.scene);
  
  #update wrapper, can pass in a Null device to use
  def update(self, cam, img, update_alpha=True, mask=None, device_string="") :
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
      update_grey(self.scene, cache, cam, img, dev, "", mask, update_alpha);
  #update wrapper, can pass in a Null device to use
  def update_app(self, cam, img, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
	  print " Not  implemented in C++ yet ";
	  return;
    update_app_grey(self.scene, cache, cam, img, dev);

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

  #render wrapper, same as above
  def render_vis(self, cam, ni=1280, nj=720, device_string="") : 
    cache = self.active_cache; 
    dev = self.device; 
    #check if force gpu or cpu
    if device_string=="gpu" : 
      cache = self.opencl_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache; 
      dev = None; 
    if self.rgb : 
      print "Render visibility map not implemented for color scenes"; 
      return; 
    else : 
      expimg, vis_image = render_grey_and_vis(self.scene, cache, cam, ni, nj, dev); 
    return expimg, vis_image; 
  
  #render depth image wrapper
  def render_depth(self, cam, ni=1280, nj=720, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" : 
      cache = self.opencl_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache; 
      dev = None; 
    expimg,varimg = render_depth(self.scene, cache, cam, ni, nj, dev); 
    return expimg,varimg; 
  
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
  #ingest heigh map
  def ingest_height_map(self,x_img,y_img,z_img, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    ingest_height_map(self.scene, cache,x_img,y_img,z_img, dev);
    return ;

  # detect change wrapper,
  def change_detect(self, cam, img, exp_img, n=1, raybelief="", max_mode=False, rgb=False, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencel_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    cd_img = change_detect(self.scene,cache,cam,img,exp_img,dev,rgb,n,raybelief,max_mode);
    return cd_img;

  def refine(self, thresh=0.3, device_string="") :
    if device_string=="":
      nCells = refine(self.scene, self.active_cache, thresh, self.device);
    elif device_string=="gpu" :
      nCells = refine(self.scene, self.opencl_cache, thresh, self.device);
    elif device_string=="cpp" :
      nCells = refine(self.scene, self.cpu_cache, thresh, None);
    return nCells

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
  def write_cache(self, do_clear = 0) :
    write_cache(self.cpu_cache, do_clear);

  #clear cache (both caches if OPENCL scene)
  def clear_cache(self) :
    clear_cache(self.cpu_cache);
    if self.opencl_cache: 
      clear_cache(self.opencl_cache);


  #####################################################################
  ######### BATCH UPDATE METHODS ######################################
  #####################################################################
  def create_stream_cache(self, imgs, interval=1, types=""):
    
    # write image identifiers to file
    #imgRange = range(0, len(imgs), interval); 
    #num_imgs = len(imgRange); 
    image_id_fname = self.model_dir + "/image_list.txt";
    fd = open(image_id_fname,"w");
    print >> fd, len(imgs)
    #for i in imgRange:
    #  print >>fd, "img_%05d"%i 
    for img in imgs:
      fname, fextension = splitext(img); 
      bname = basename(fname)
      print >> fd, bname; 
    fd.close();

    #write type identifiers into file
    type_id_fname = self.model_dir + "/type_names_list.txt";
    fd2 = open(type_id_fname,"w");
    print >>fd2, 4;
    print >>fd2, "aux0";
    print >>fd2, "aux1";
    print >>fd2, "aux2";
    print >>fd2, "aux3";
    fd2.close();

    # open the stream cache, this is a read-only cache
    boxm2_batch.init_process("boxm2CreateStreamCacheProcess");
    boxm2_batch.set_input_from_db(0,self.scene);
    boxm2_batch.set_input_string(1,type_id_fname);
    boxm2_batch.set_input_string(2,image_id_fname);
    boxm2_batch.set_input_float(3,6);
    boxm2_batch.run_process();
    (cache_id, cache_type) = boxm2_batch.commit_output(0);
    self.str_cache = dbvalue(cache_id, cache_type);

  #writes aux data for each image in imgs array
  def write_aux_data(self, imgs, cams) :
    for idx in range( len(imgs) ) :
      print '--------------------------';
      print "processing image " + imgs[idx];
      
      #load cam/img
      img, ni, nj = load_image (imgs[idx]);
      pcam        = load_perspective_camera(cams[idx]); 
      gcam        = persp2gen(pcam,ni,nj); 
     
      # update aux per view call
      fname, fextension = splitext(imgs[idx]); 
      imageID = basename(fname)
      self.update_aux(img, gcam, imageID); 
  
  #create an imagewise aux buffer for cam/img
  def update_aux(self, img, cam, imgId, device_string="") : 
    if device_string=="":
      update_aux_per_view(self.scene, self.active_cache, img, cam, imgId, self.device)
    elif device_string=="gpu" :
      update_aux_per_view(self.scene, self.opencl_cache, img, cam, imgId, self.device) 
    elif device_string=="cpp" :
      update_aux_per_view(self.scene, self.cpu_cache, img, cam, imgId, None)

  #takes already created aux buffers (for each image) and fits a Mixture of 3 
  #Gaussians to each cell, saves the apperaance
  def batch_paint(self, imgs, cams, device_string="") :
    #verify stream cache
    if(self.str_cache==None): 
      self.create_stream_cache(imgs); 

    #sigma norm table?
    under_estimation_probability = 0.2;
    boxm2_batch.init_process("bstaSigmaNormTableProcess");
    boxm2_batch.set_input_float(0,under_estimation_probability);
    boxm2_batch.run_process();
    (id,type)=boxm2_batch.commit_output(0);
    n_table=dbvalue(id,type);

    # call batch pain process
    if device_string=="":
      boxm2_batch.init_process("boxm2OclPaintBatchProcess");
      boxm2_batch.set_input_from_db(0, self.device);
      boxm2_batch.set_input_from_db(1, self.scene);
      boxm2_batch.set_input_from_db(2, self.opencl_cache);
      boxm2_batch.set_input_from_db(3, self.str_cache);
      boxm2_batch.set_input_from_db(4, n_table);
      boxm2_batch.run_process();
    elif device_string=="cpu":
      boxm2_batch.init_process("boxm2CppBatchUpdateAppProcess");
      boxm2_batch.set_input_from_db(0, self.scene);
      boxm2_batch.set_input_from_db(1, self.cpu_cache);
      boxm2_batch.set_input_from_db(2, self.str_cache);
      boxm2_batch.set_input_from_db(3, n_table);
      boxm2_batch.run_process();

    # close the files so that they can be reloaded after the next iteration
    boxm2_batch.init_process("boxm2StreamCacheCloseFilesProcess");
    boxm2_batch.set_input_from_db(0,self.str_cache);
    boxm2_batch.run_process();
    
    #write out afterwards
    self.write_cache(); 
    
  def cpu_batch_paint(self, imgs, cams) :
    if(self.str_cache==None): 
      self.create_stream_cache(imgs); 

    #sigma norm table?
    under_estimation_probability = 0.2;
    boxm2_batch.init_process("bstaSigmaNormTableProcess");
    boxm2_batch.set_input_float(0,under_estimation_probability);
    boxm2_batch.run_process();
    (id,type)=boxm2_batch.commit_output(0);
    n_table=dbvalue(id,type);

    #loop over images creating aux data
    for idx in range(0, len(imgs)):
    
      #load cam/img
      img, ni, nj = load_image (imgs[idx]);
      pcam        = load_perspective_camera(cams[idx]); 
      gcam        = persp2gen(pcam,ni,nj); 

      #create norm intensity (num rays...)
      boxm2_batch.init_process("boxm2CppCreateNormIntensitiesProcess");
      boxm2_batch.set_input_from_db(0, self.scene);
      boxm2_batch.set_input_from_db(1, self.cpu_cache);
      boxm2_batch.set_input_from_db(2, gcam);
      boxm2_batch.set_input_from_db(3, img);
      boxm2_batch.set_input_string(4, "img_"+"%05d"%idx);
      boxm2_batch.run_process();

      #create aux
      boxm2_batch.init_process("boxm2CppCreateAuxDataOPT2Process");
      boxm2_batch.set_input_from_db(0, self.scene);
      boxm2_batch.set_input_from_db(1, self.cpu_cache);
      boxm2_batch.set_input_from_db(2, gcam);
      boxm2_batch.set_input_from_db(3, img);
      boxm2_batch.set_input_string(4, "img_"+"%05d"%idx);
      boxm2_batch.run_process();
      self.write_cache(True); 

    boxm2_batch.init_process("boxm2CppBatchUpdateOPT2Process");
    boxm2_batch.set_input_from_db(0, self.scene);
    boxm2_batch.set_input_from_db(1, self.cpu_cache);
    boxm2_batch.set_input_from_db(2, self.str_cache);
    boxm2_batch.set_input_from_db(3, n_table);
    boxm2_batch.run_process();

    # close the files so that they can be reloaded after the next iteration
    boxm2_batch.init_process("boxm2StreamCacheCloseFilesProcess");
    boxm2_batch.set_input_from_db(0, self.str_cache);
    boxm2_batch.run_process();
    
    self.write_cache(); 
