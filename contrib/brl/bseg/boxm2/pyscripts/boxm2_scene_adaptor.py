from boxm2_adaptor import *
from boxm2_tools_adaptor import *
from boxm2_filtering_adaptor import *
from vil_adaptor import *;
from vpgl_adaptor import *;
from os.path import basename, splitext
import sys

#############################################################################
# boxm2_scene_adaptor class offers super simple model manipulation syntax
# you can always force the process to use CPP by just passing in "cpp" as the last
# arg to any function in this class
#############################################################################
class boxm2_scene_adaptor(object):

  #scene adaptor init
  def __init__(self, scene_str, device_string="gpu") :

    #init (list) self vars
    self.scene = None;
    self.active_cache = None;
    self.device_string = None;
    self.cpu_cache = None;
    self.device = None;
    self.opencl_cache = None;
    self.ocl_mgr = None;
    self.str_cache = None;
    self.model_dir = None;
    self.bbox = None;
    self.lvcs = None;

    #if device_string is gpu, load up opencl
    if device_string[0:3]=="gpu" :
      self.scene, self.cpu_cache, self.ocl_mgr, self.device, self.opencl_cache = load_opencl(scene_str, device_string);
      self.active_cache = self.opencl_cache;
    elif device_string[0:3]=="cpu" :
      #self.scene, self.cpu_cache, self.ocl_mgr, self.device, self.opencl_cache = load_opencl(scene_str, device_string);
      #self.active_cache = self.opencl_cache;
      self.scene, self.cpu_cache = load_cpp(scene_str);
      self.active_cache = self.cpu_cache;
    elif device_string[0:3]=="cpp" :
      self.scene, self.cpu_cache = load_cpp(scene_str);
      self.active_cache = self.cpu_cache;
    else :
      print "UNKNOWN device type: ", device_string
      print "exiting."
      sys.exit(-1)

    #store model directory for later use
    self.bbox = scene_bbox(self.scene);
    self.description = describe_scene(self.scene);
    self.model_dir = self.description['dataPath'];
    self.rgb = self.description['appType'] == "boxm2_gauss_rgb";
    self.lvcs = scene_lvcs(self.scene);

  def __del__(self):
    boxm2_batch.remove_data(self.scene.id)
    boxm2_batch.remove_data(self.cpu_cache.id)
    boxm2_batch.remove_data(self.ocl_mgr.id)
    boxm2_batch.remove_data(self.device.id)
    boxm2_batch.remove_data(self.opencl_cache.id)
    boxm2_batch.remove_data(self.lvcs.id)

  #describe scene (returns data path)
  def describe(self) :
    return self.description;

  #returns scene bounding box
  def bounding_box(self) :
    return self.bbox

  def lvcs(self) :
    return self.lvcs

  def cache() :
    return self.cache;

  def init_alpha(self, pinit=0.01, thresh = 1.0):
    cache = self.opencl_cache
    dev = self.device
    init_alpha(self.scene, cache, dev,pinit,thresh);

  #update with alternate explaination prior and appearance density
  def update_with_alt(self, cam, img, update_alpha=True, mask=None, var=-1.0, alt_prior=None, alt_density=None):
    cache = self.opencl_cache
    dev = self.device
    update_grey_with_alt(self.scene, cache, cam, img, dev, "", mask, update_alpha, var, alt_prior, alt_density)
  #update wrapper, can pass in a Null device to use
  def update(self, cam, img, update_alpha=True,update_app=True, mask=None, device_string="", var=-1.0, ident_string="", tnear = 100000.0, tfar = 0.0000001) :
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
      return update_rgb(self.scene, cache, cam, img, dev);
    else :
      return update_grey(self.scene, cache, cam, img, dev, ident_string, mask, update_alpha, var, update_app, tnear, tfar);

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
    if self.rgb :
      update_rgb(self.scene, cache, cam, img, dev, "", False);
    else :
      update_app_grey(self.scene, cache, cam, img, dev);
  #update skky wrapper, can pass in a Null device to use
  def update_sky(self, cam, img, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      print " Not  implemented in C++ yet ";
      return;
    update_sky(self.scene, cache, cam, img, dev);

  #render wrapper, same as above
  def render(self, cam, ni=1280, nj=720, device_string="", ident_string="", tnear = 1000000.0,tfar = 1000000.0, ) :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    if self.rgb :
      expimg, vis_image, status = render_rgb(self.scene, cache, cam, ni, nj, dev,tnear,tfar);
      boxm2_batch.remove_data(vis_image.id)
    else :
      expimg = render_grey(self.scene, cache, cam, ni, nj, dev, ident_string,tnear,tfar);
    return expimg;

  #render wrapper, same as above
  def render_vis(self, cam, ni=1280, nj=720, device_string="", ident="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    if self.rgb :
      expimg, vis_image, status = render_rgb(self.scene, cache, cam, ni, nj, dev,ident);
    else :
      expimg, vis_image = render_grey_and_vis(self.scene, cache, cam, ni, nj, dev,ident);
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
    expimg,varimg,visimg = render_depth(self.scene, cache, cam, ni, nj, dev);
    return expimg,varimg,visimg;

  #render depth image with loading given region wrapper
  def render_depth_region(self, cam, lat, lon, elev, radius, ni=1280, nj=720, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    expimg, varimg, visimg = render_depth_region(self.scene, cache, cam, lat, lon, elev, radius, ni, nj, dev);
    return expimg, varimg, visimg;

  #render z image wrapper
  def render_z_image(self, cam, ni=1280, nj=720, normalize = False, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    #check if force gpu or cpu
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    z_exp_img, z_var_img = render_z_image(self.scene, cache, cam, ni, nj, normalize, dev);
    return z_exp_img, z_var_img;


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
  def ingest_height_map(self,x_img,y_img,z_img, zero_out_alpha=True,device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    ingest_height_map(self.scene, cache,x_img,y_img,z_img, zero_out_alpha, dev);
    return ;

  #ingest heigh map
  def ingest_height_map_space(self,x_img,y_img,z_img, crust_thickness,device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    ingest_height_map_space(self.scene, cache,x_img,y_img,z_img, crust_thickness, dev);
    return ;

  #ingest to zero out alphas along the rays given by the input images
  def ingest_to_zero_out_alpha(self,x_img,y_img,z_img,device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    ingest_to_zero_out_alpha(self.scene, cache, x_img, y_img, z_img, dev);
    return ;

  #ingest label map
  #def ingest_label_map(self,x_img,y_img,z_img,label_img,device_string="") :
  def ingest_label_map(self,x_img,y_img,z_img,label_img,ident,device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    #ingest_label_map(self.scene, cache, x_img, y_img, z_img, label_img, dev);
    ingest_label_map(self.scene, cache, x_img, y_img, z_img, label_img, ident, dev);
    return;

  #ingest label map
  def ingest_osm_label_map(self,x_img,y_img,z_img, label_img, ident="land", device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string == "gpu":
      cache = self.opencl_cache;
    elif device_string == "cpp":
      cache = self.cpu_cache;
      dev = None;
    ingest_osm_label_map(self.scene, cache, x_img, y_img, z_img, label_img, ident, dev);
    return;

  #ingest buckeye-style dem
  def ingest_buckeye_dem(self, first_ret_fname, last_ret_fname, geoid_height,geocam, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    ingest_buckeye_dem(self.scene, cache, first_ret_fname, last_ret_fname, geoid_height,geocam, dev);
    return ;

  def probability_of(self, cam, image):
    cache = self.active_cache;
    dev = self.device;
    outimg = compute_probability_of_image(self.device,self.scene, self.opencl_cache, cam, image);
    return outimg;

  def cubic_probability_of(self, cam, image,model_ident,img_ident):
    cache = self.active_cache;
    dev = self.device;
    outimg = cubic_compute_probability_of_image(self.device,self.scene, self.opencl_cache, cam, image, model_ident,img_ident);
    return outimg;

  # detect change wrapper,
  def change_detect(self, cam, img, exp_img, n=1, raybelief="", max_mode=False, rgb=False, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    cd_img = change_detect(self.scene,cache,cam,img,exp_img,dev,rgb,n,raybelief,max_mode);
    return cd_img;

  # detect change wrapper,
  def change_detect2(self, cam, img, max_mode=False,tnear = 100000, tfar = 0.00001, device_string="") :
    cache = self.active_cache;
    dev = self.device;
    if device_string=="gpu" :
      cache = self.opencl_cache;
    elif device_string=="cpp" :
      cache = self.cpu_cache;
      dev = None;
    cd_img,vis_img = change_detect2(self.scene,cache,cam,img,max_mode,tnear,tfar,dev);
    return cd_img,vis_img;

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

  # Apply multiple filters to  scene
  def kernel_vector_filter(self, filters ) :
    return apply_filters(self.scene, self.opencl_cache, self.device, filters);

  # Interpolate normal from various responses
  def interpolate_normals(self, filters) :
    return interpolate_normals(self.scene, self.opencl_cache, self.device, filters);

  # Extract cell centers to XYZ for fast access
  def extract_cell_centers(self, prob_thresh=0.0) :
    return extract_cell_centers(self.scene, self.cpu_cache, prob_thresh=0.0)

  #  Flip normals towards direction of maximum visibility
  def flip_normals(self, use_sum=False) :
    return flip_normals(self.scene, self.opencl_cache, self.device, use_sum)

  # Export points and normals to a .PLY file or XYZ. Points and normals need to be extracted first
  def export_points_and_normals(self, file_out, save_aux=True, prob_thresh=0.0, vis_thresh=0.0, nmag_thresh=0.0, bbox_file=""):
    return export_points_and_normals(self.scene, self.cpu_cache, file_out, save_aux, prob_thresh, vis_thresh, nmag_thresh, bbox_file)

  # Adds auxiliary data to vertices in a .PLY
  def add_aux_info_to_ply(self, file_in, file_out):
    add_aux_info_to_ply(self.scene, self.cpu_cache, file_in, file_out);

  #only write the cpu_cache to disk
  def write_cache(self, do_clear = 0) :
    write_cache(self.cpu_cache, do_clear);

  #clear cache (both caches if OPENCL scene)
  def clear_cache(self) :
    clear_cache(self.cpu_cache);
    if self.opencl_cache:
      clear_cache(self.opencl_cache);

  ################################
  #get info functions
  def get_info_along_ray(self,cam,u,v,prefix,identifier="") :
    return get_info_along_ray(self.scene,self.cpu_cache,cam,u,v,prefix,identifier)

  def query_cell_brdf(self, point, model_type) :
    return query_cell_brdf(self.scene,self.cpu_cache,point,model_type)

  #####################################################################
  ######### BATCH UPDATE METHODS ######################################
  #####################################################################
  def create_stream_cache(self, imgs, interval=1, types="", max_gb=6.0):

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
    boxm2_batch.set_input_float(3,max_gb);
    boxm2_batch.run_process();
    (cache_id, cache_type) = boxm2_batch.commit_output(0);
    self.str_cache = dbvalue(cache_id, cache_type);

  # remove stream cache object from database
  def destroy_stream_cache(self):
    if self.str_cache:
       boxm2_batch.remove_data(self.str_cache.id)
       self.str_cache = None

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
  def update_aux(self, img, cam, imgId, device_string="", mask=None) :
    if device_string=="":
      update_aux_per_view(self.scene, self.active_cache, img, cam, imgId, self.device, mask)
    elif device_string=="gpu" :
      update_aux_per_view(self.scene, self.opencl_cache, img, cam, imgId, self.device, mask)
    elif device_string=="cpp" :
      update_aux_per_view(self.scene, self.cpu_cache, img, cam, imgId, None, mask)

  #create an imagewise aux buffer for batch update of normal-albedo-array appearance model
  def update_aux_naa(self, img, cam, metadata, atm_params, imgId, alt_prior, alt_density) :
    update_aux_per_view_naa(self.scene, self.opencl_cache, img, cam, metadata, atm_params, imgId, alt_prior, alt_density, self.device)

  #takes already created aux buffers (for each image) and fits a Mixture of 3
  #Gaussians to each cell, saves the appearance
  def batch_paint(self, imgs, cams, device_string="") :
    #verify stream cache
    if (self.str_cache==None):
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
    if (self.str_cache==None):
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

  def cpu_batch_compute_normal_albedo(self, metadata_filename_list, atmospheric_params_filename_list):
    boxm2_batch.init_process("boxm2CppBatchComputeNormalAlbedoProcess")
    boxm2_batch.set_input_from_db(0,self.scene)
    boxm2_batch.set_input_from_db(1,self.cpu_cache)
    boxm2_batch.set_input_from_db(2,self.str_cache)
    boxm2_batch.set_input_string(3,metadata_filename_list)
    boxm2_batch.set_input_string(4,atmospheric_params_filename_list)
    boxm2_batch.run_process()

    # close the files so that they can be reloaded after the next iteration
    boxm2_batch.init_process("boxm2StreamCacheCloseFilesProcess");
    boxm2_batch.set_input_from_db(0,self.str_cache);
    boxm2_batch.run_process();

  def ocl_batch_compute_normal_albedo(self, img_id_list, metadata_filename_list, atmospheric_params_filename_list):
    boxm2_batch.init_process("boxm2OclBatchComputeNormalAlbedoArrayProcess")
    boxm2_batch.set_input_from_db(0,self.device)
    boxm2_batch.set_input_from_db(1,self.scene)
    boxm2_batch.set_input_from_db(2,self.opencl_cache)
    boxm2_batch.set_input_string(3,img_id_list)
    boxm2_batch.set_input_string(4,metadata_filename_list)
    boxm2_batch.set_input_string(5,atmospheric_params_filename_list)
    boxm2_batch.run_process()

  def render_expected_image_naa(self, camera, ni,nj, metadata, atmospheric_params):
    boxm2_batch.init_process("boxm2OclRenderExpectedImageNAAProcess");
    boxm2_batch.set_input_from_db(0,self.device)
    boxm2_batch.set_input_from_db(1,self.scene)
    boxm2_batch.set_input_from_db(2,self.opencl_cache)
    boxm2_batch.set_input_from_db(3,camera)
    boxm2_batch.set_input_unsigned(4,ni)
    boxm2_batch.set_input_unsigned(5,nj)
    boxm2_batch.set_input_from_db(6,metadata)
    boxm2_batch.set_input_from_db(7,atmospheric_params)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    exp_image = dbvalue(id,type)
    (id,type) = boxm2_batch.commit_output(1)
    mask_image = dbvalue(id,type)
    return(exp_image, mask_image)

  def update_alpha_naa(self, image, camera, metadata, atmospheric_params, alt_prior, alt_density):
    boxm2_batch.init_process("boxm2OclUpdateAlphaNAAProcess")
    boxm2_batch.set_input_from_db(0, self.device)
    boxm2_batch.set_input_from_db(1, self.scene)
    boxm2_batch.set_input_from_db(2, self.opencl_cache)
    boxm2_batch.set_input_from_db(3, camera)
    boxm2_batch.set_input_from_db(4, image)
    boxm2_batch.set_input_from_db(5, metadata)
    boxm2_batch.set_input_from_db(6, atmospheric_params)
    boxm2_batch.set_input_from_db(7, alt_prior)
    boxm2_batch.set_input_from_db(8, alt_density)
    if not (boxm2_batch.run_process()):
      print("ERROR: run_process() returned False")
    return

  def render_expected_albedo_normal(self, camera, ni, nj):
    boxm2_batch.init_process("boxm2OclRenderExpectedAlbedoNormalProcess");
    boxm2_batch.set_input_from_db(0,self.device)
    boxm2_batch.set_input_from_db(1,self.scene)
    boxm2_batch.set_input_from_db(2,self.opencl_cache)
    boxm2_batch.set_input_from_db(3,camera)
    boxm2_batch.set_input_unsigned(4,ni)
    boxm2_batch.set_input_unsigned(5,nj)
    boxm2_batch.run_process()
    (id,type) = boxm2_batch.commit_output(0)
    exp_albedo = dbvalue(id,type)
    (id,type) = boxm2_batch.commit_output(1)
    exp_normal = dbvalue(id,type)
    (id,type) = boxm2_batch.commit_output(2)
    mask_image = dbvalue(id,type)
    return(exp_albedo, exp_normal, mask_image)


  def transform(self, tx,ty,tz,rx,ry,rz,scale):
    boxm2_batch.init_process("boxm2TransformModelProcess")
    boxm2_batch.set_input_from_db(0,self.scene)
    boxm2_batch.set_input_float(1,tx)
    boxm2_batch.set_input_float(2,ty)
    boxm2_batch.set_input_float(3,tz)
    boxm2_batch.set_input_float(4,rx)
    boxm2_batch.set_input_float(5,ry)
    boxm2_batch.set_input_float(6,rz)
    boxm2_batch.set_input_float(7,scale)
    boxm2_batch.run_process()
    return

  def compute_sun_affine_camera(self, sun_az, sun_el, astro_coords = True):
    (camera, ni, nj) = compute_sun_affine_camera(self.scene, sun_az, sun_el, astro_coords)
    return (camera, ni, nj)

  def update_sun_visibilities(self, sun_camera, ni, nj, prefix_name=""):
    update_sun_visibilities(self.scene, self.device, self.opencl_cache, self.cpu_cache, sun_camera, ni, nj, prefix_name)

  def render_shadow_map(self, camera, ni, nj, prefix_name=''):
    shadow_map = render_shadow_map(self.scene, self.device, self.opencl_cache, camera, ni, nj, prefix_name)
    return shadow_map

  def render_scene_mask(self, camera, ni, nj, ground_plane_only=False):
    mask = create_mask_image(self.scene, camera, ni, nj, ground_plane_only);
    return mask;

  def normals_to_id(self) :
    print("Normals to id ");
    boxm2_batch.init_process("boxm2CppNormalsToIdProcess");
    boxm2_batch.set_input_from_db(0,self.scene);
    boxm2_batch.set_input_from_db(1,self.cpu_cache);
    return boxm2_batch.run_process();
