from boxm2_adaptor import *

#############################################################################
# boxm2_scene_adaptor class offers super simple model manipulation syntax
# you can always force the process to use CPP by just passing in "cpp" as the last
# arg to any function in this class
#############################################################################
class boxm2_scene_adaptor: 

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
      self.scene, self.cpu_cache = load_cpp(scene_str);     
      self.active_cache = self.cpu_cache; 
  def describe(self):
	  describe_scene(self.scene);
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
    expimg = render_depth(self.scene, cache, cam, ni, nj, dev); 
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
  def change_detect(self, cam, img, exp_img, n=1, raybelief="", rgb=False, device_string="") : 
    cache = self.active_cache; 
    dev = self.device; 
    if device_string=="gpu" : 
      cache = self.opencel_cache; 
    elif device_string=="cpp" : 
      cache = self.cpu_cache;
      dev = None;
    cd_img = change_detect(self.scene,cache,cam,img,exp_img,dev,rgb,n,raybelief); 
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

  def write_cache(self, do_clear = 0): 
    write_cache(self.cpu_cache, do_clear); 

  #clear cache (both caches if OPENCL scene)
  def clear_cache(self):
    clear_cache(self.cpu_cache); 
    if self.opencl_cache: clear_cache(self.opencl_cache); 
