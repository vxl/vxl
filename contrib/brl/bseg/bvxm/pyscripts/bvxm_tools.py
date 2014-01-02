from bvxm_register import bvxm_batch,dbvalue;
from bvxm_adaptor import *
from bvxm_volm_adaptor import *
from bvxm_vpgl_adaptor import *
from bvxm_vil_adaptor import *
import os, time, glob, sys
#############################################################################
# PROVIDES higher level python functions to make bvxm_batch
# code more readable/refactored
#############################################################################

def get_satellite_uncertainty(sat_name):
  if "QuickBird" in sat_name:
    return 40;
  if "WorldView1" in sat_name:
    return 14;
  if "WorldView2" in sat_name:
    return 24;
  if "GeoEye" in sat_name:
    return 5;
  return 40;

## build edge world using a set of images and camera files, the first n_seed images are used as seed
def build_edge_world(scene, scene_id, world_dir, n_seed, image_fnames, cropped_imgs, cropped_cams, uncertainties, param_file_dir, corrected_global_cams, out_dir, save=0):

  ## remove the .vox files if any
  edge_files = glob.glob(world_dir + "/edges_*.vox");
  for edge_file_name in edge_files:
    print "removing " + edge_file_name + "...";
    sys.stdout.flush()
    os.remove(edge_file_name);

  t1 = time.time();
#   out_imgs = out_dir + "img_%d_%s.tif";
  out_edge_imgs = out_dir + "img_%d.tif";
  out_exp_edge_imgs = out_dir + "exp_img_%d.tif"
  cnt_updates = 0;

  # create cropped_edge_imgs from cropped_img
  cropped_edge_imgs = []
  for i in range(0,len(cropped_imgs),1):
    cropped_edge_image = bvxm_detect_edges(cropped_imgs[i], param_file_dir + "bvxmDetectEdgesProcess.xml");
    if save:
      bvxm_save_image(cropped_edge_image, out_edge_imgs % i);
    cropped_edge_imgs.append(cropped_edge_image)

  for i in range(0, len(cropped_edge_imgs), 1):
    cropped_cam = cropped_cams[i]
    cropped_edge_image = cropped_edge_imgs[i]
    uncertainty = uncertainties[i]
    image_name = image_fnames[i]
    print '-----------------------------';
    print "scene: %d, image: %d out of %d" % (scene_id, i, len(image_fnames));
    print "n_seed = ", n_seed
    print "uncertainty = ", uncertainty
    print '-----------------------------';
    sys.stdout.flush();
    if i >= n_seed:
      cam_cor, expected_edge_image, offset_u, offset_v = rpc_registration(scene, cropped_cam, cropped_edge_image, uncertainty, 0, 0, 1);
      if save:
        bvxm_save_image(expected_edge_image, out_exp_edge_imgs % cnt_updates);
      print "-------- will correct with offset_u: " + str(offset_u) + " offset_v: " + str(offset_v);
#       cam_global_cor = correct_rational_camera(cropped_cam, offset_u, offset_v);
      save_rational_camera(cam_cor,corrected_global_cams % image_name);
      # update edge world using corrected camera
      update_edges(scene, cam_cor, cropped_edge_image, param_file_dir + "bvxmUpdateEdgesProcess.xml");
      # clean data
      bvxm_batch.remove_data(expected_edge_image.id)
#       bvxm_batch.remove_data(cam_global_cor.id);
      bvxm_batch.remove_data(cam_cor.id)
    else:
      # update edge world using seed
      update_edges(scene, cropped_cam, cropped_edge_image, param_file_dir + "bvxmUpdateEdgesProcess.xml");

    cnt_updates = cnt_updates+1;
    if save:
      bvxm_save_image(cropped_edge_image, out_edge_imgs % cnt_updates);

  # update the seed camera
  print "update the seed images..."
  sys.stdout.flush()
  cnt_updates = 0
  for i in range(0, len(cropped_edge_imgs), 1):
    cropped_cam = cropped_cams[i]
    cropped_edge_image = cropped_edge_imgs[i]
    uncertainty = uncertainties[i]
    image_name = image_fnames[i]
    print '-----------------------------';
    print "scene: %d, image: %d out of %d" % (scene_id, i, len(image_fnames));
    print "n_seed = ", n_seed
    print "uncertainty = ", uncertainty
    print '-----------------------------';
    if i >= n_seed:
      continue;
    else:
      # update the seed cameras
      cam_cor, expected_edge_image, offset_u, offset_v = rpc_registration(scene, cropped_cam, cropped_edge_image, uncertainty, 0, 0, 1);
      if save:
        bvxm_save_image(expected_edge_image, out_exp_edge_imgs % cnt_updates);
        cnt_updates = cnt_updates + 1
      print "-------- will correct the cropped camera with offset_u: " + str(offset_u) + " offset_v: " + str(offset_v);
#       cam_global_cor = correct_rational_camera(cropped_cam, offset_u, offset_v);
      save_rational_camera(cam_cor,corrected_global_cams % image_name);
      # clean data
      bvxm_batch.remove_data(expected_edge_image.id)
#       bvxm_batch.remove_data(cam_global_cor.id);
      bvxm_batch.remove_data(cam_cor.id)


  # render the ortho map
  out_e_img, out_e_img_byte, out_h_img, ortho_cam = render_ortho_edgemap(scene);
  bvxm_save_image(out_e_img, out_dir + "scene_%d_ortho_edge_map.tif" % scene_id);
  bvxm_batch.remove_data(out_e_img.id);
  bvxm_batch.remove_data(out_e_img_byte.id);
  bvxm_batch.remove_data(out_h_img.id);
  bvxm_batch.remove_data(ortho_cam.id);

  t2 = time.time();
  secs = t2-t1;
  mins = secs/60.0;
  print "Scene: " + str(scene_id) + " execution time: " +str(secs) + " secs = " + str(mins) + " mins for " + str(cnt_updates) + " updates.";

## returns only the resource cnt for that scene
def get_scene_resource_cnt(scene, res):
  cnt = 0;
  lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat = scene_box(scene);

  temp_text_res = "";
  cnt = scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, temp_text_res, "PAN");
  return cnt;

## generate valid cropped images, cameras and uncertainty files that intersect with current scene
## will also check whether there are enough seed images/cams for current scene
def create_scene_crop_image(scene, res, cam_global, min_cnt, max_cnt, param_file_dir, n_seed_necessary=5, edge_threshold = 15,
                            cropped_image_ratio = 0.3, gsd_thres = 1.0):
  cnt = 0;
  n_seed = 0;
  valid_img_names = [];
  valid_cameras = [];
  cropped_valid_imgs = [];
  cropped_valid_cams = [];
  valid_uncertainties = [];

  lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat = scene_box(scene);
  # obtain all images that intersect with current scene, if less than the necessary seed number, skip the scene
  temp_text_res = "./scene_res.txt";
  cnt = scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, temp_text_res, "PAN", gsd_thres);
  print "number of images that overlap the scene: %d with GSD less than: %f" % (cnt, gsd_thres);

  ## not enough satellite images overlap with current scene, skip this scene
  if cnt > max_cnt or cnt < min_cnt:
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, 0, 0;
  ## correct the filename to ensure it works for multiple platform
  f = open(temp_text_res, 'r')
  lines = f.readlines();
  f.close();
  os.remove(temp_text_res)
  res_files_tmp = [];
  for line in lines:
    res_files_tmp.append(line.rstrip('\n'));

  res_files = [];
  for res_file in res_files_tmp:
    rstr = os.path.abspath(res_file);
    res_file = rstr.replace('\\','/');
    res_files.append(res_file);

  ## first check how many cameras are in global 1 if less than required seed number, skip
  for i in range(0, len(res_files), 1):
    head, tail = os.path.split(res_files[i]);
    name, ext = os.path.splitext(tail);
    cam_name = cam_global + "%s_corrected.rpb" % name;
    if os.path.exists(cam_name):
      n_seed = n_seed + 1;

  if n_seed < n_seed_necessary:
    print "only %d refined cameras, less than the required seed number %d, skip" % (n_seed, n_seed_necessary)
    sys.stdout.flush()
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, 0, 0;

  ## get rid of cloud images
  cnt = 0;            # number of non-cloud images
  img_names = []      # valid non-cloud image name
  cam_names = []      # valid cameras associated with non-cloud images
  cam_cat = []        # stores the category of the cameras (cam_global2 (2), cam_global1 (1) or original camera (0) )
  cropped_imgs = []   # all cropped images
  cropped_cams = []   # all cropped cameras
  uncertainties = []  # all uncertainties
  for img_file in res_files:
    head, tail = os.path.split(img_file);
    name, ext = os.path.splitext(tail);
    cam_name1 = cam_global  + "%s_corrected.rpb" % name;
    if os.path.isfile(cam_name1):
      curr_cam_cat = 1;
      curr_cam_res = cam_name1
      cam = load_rational_camera(cam_name1);
      uncertainty_file = param_file_dir + "uncertainty_5m.xml";
    else:
      curr_cam_cat = 0;
      curr_cam_res = img_file;
      cam = load_rational_camera_nitf(img_file);
      uncertainty_file = param_file_dir + "uncertainty_%dm.xml" % get_satellite_uncertainty(img_file);
    # crop the image using refined camera or original camera
    statuscode, cropped_cam, cropped_img, uncertainty = roi_init(img_file, cam, scene, uncertainty_file);
    if statuscode:
      sys.stdout.flush()
      # check the cropped image size
      crop_ni, crop_nj = image_size(cropped_img);
      if crop_ni > crop_nj:  crop_img_ratio = float(crop_nj)/float(crop_ni);
      elif crop_ni < crop_nj:  crop_img_ratio = float(crop_ni)/float(crop_nj);
      else: crop_img_ratio = 1.0
      if crop_img_ratio < cropped_image_ratio:
        print "Cropped image size %d x %d of image %s is not cubic, ignored..." % (crop_ni, crop_nj, name)
        sys.stdout.flush()
        continue;
      cropped_edge_image = bvxm_detect_edges(cropped_img, param_file_dir + "bvxmDetectEdgesProcess.xml");
      edge_sum = img_sum(cropped_edge_image);
      ni, nj = image_size(cropped_edge_image);
      percent = edge_sum/(ni*nj);
      if percent > edge_threshold:
        # store the non-cloud images and all other resources
        img_names.append(img_file)
        cam_names.append(curr_cam_res)
        cam_cat.append(curr_cam_cat);
        cropped_imgs.append(cropped_img);
        cropped_cams.append(cropped_cam);
        uncertainties.append(uncertainty)
        cnt = cnt+1
  num_cloud_imgs = len(res_files)-cnt
  print "%d out of %d images are valid without cloud" % (cnt, len(res_files))
  sys.stdout.flush();
  ## check whether there are enough seed images
  seed_num = 0;
  for camera_cat in cam_cat:
    if camera_cat == 1:  seed_num = seed_num + 1;
  if seed_num < n_seed_necessary:
    print "only %d refined cameras, less than the required seed number %d, skip" % (seed_num, n_seed_necessary)
    sys.stdout.flush()
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, num_cloud_imgs, 0;

  ## rearrange the valid images based on camera category such that the first n_seed_necessary images will be chosen as seed during edge_world creation
  for i in range(0, cnt, 1):
    if (cam_cat[i]==1):
      valid_img_names.append(img_names[i]);
      valid_cameras.append(cam_names[i]);
      cropped_valid_imgs.append(cropped_imgs[i])
      cropped_valid_cams.append(cropped_cams[i])
      valid_uncertainties.append(uncertainties[i])
  print "%d out of %d cameras are in cam_global1" % (len(valid_cameras), cnt)
  sys.stdout.flush()
  ## append all other images
  for i in range(0, cnt, 1):
    if (cam_cat[i] == 0):
      valid_img_names.append(img_names[i]);
      valid_cameras.append(cam_names[i]);
      cropped_valid_imgs.append(cropped_imgs[i])
      cropped_valid_cams.append(cropped_cams[i])
      valid_uncertainties.append(uncertainties[i])
  return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, num_cloud_imgs, cnt;


## return the valid cropped images/cameras that intersect with current scene, all of which can be used to update the edge world
## process the scenes only when following satisfy:
##    1. there are more than 'n_seed_necessary' sources to update the edge world
##    2. there are non-cloud images that require refined camera (cam_global2)
def get_scene_files(scene, res, cam_global, cam_global2, min_cnt, max_cnt, param_file_dir, n_seed_necessary=5, edge_threshold = 15,
                    cropped_image_ratio = 0.3):
  n_seed = 0;
  cnt = 0;
  valid_img_names = [];
  valid_cameras = [];
  cropped_valid_imgs = [];
  cropped_valid_cams = [];
  valid_uncertainties = [];

  # obtain the local scene box
  scene_min_x, scene_min_y, scene_max_x, scene_max_y, scene_voxel_size, lower_left_z, upper_right_z = scene_local_box(scene);
  scene_box_x = scene_max_x - scene_min_x;
  scene_box_y = scene_max_y - scene_min_y;

  lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat = scene_box(scene);
  # check the images that intersect with scene, return the number of the overlapped images
  temp_text_res = "./scene_res.txt";
  cnt = scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, temp_text_res, "PAN");

  ## not enough satellite images overlap with current scene, skip this scene
  if cnt > max_cnt or cnt < min_cnt:
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, n_seed, 0;

  ## correct the filename to ensure it works for multiple platform
  f = open(temp_text_res, 'r')
  lines = f.readlines();
  f.close();
  os.remove(temp_text_res)
  res_files_tmp = [];
  for line in lines:
    res_files_tmp.append(line.rstrip('\n'));

  res_files = [];
  for res_file in res_files_tmp:
    rstr = os.path.abspath(res_file);
    res_file = rstr.replace('\\','/');
    res_files.append(res_file);

  ## first check how many cameras are already in cam_global2 -- use all of these as seed
  for i in range(0, len(res_files), 1):
    head, tail = os.path.split(res_files[i]);
    name, ext = os.path.splitext(tail);
    cam_name = cam_global2 + "%s_corrected.rpb" % name;
    if os.path.exists(cam_name):
      n_seed = n_seed + 1;

  print "%d out of %d cameras are already in cam_global2, n_seed: %d" % (n_seed, len(res_files), n_seed)
  sys.stdout.flush()
  ## if all cameras are already corrected skip this scene
  if n_seed == cnt:
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, n_seed, 0;

  ## check conditions whether we have enough resource and cameras required to be refined
  cnt = 0;            # number of non-cloud images
  img_names = []      # valid non-cloud image name
  cam_names = []      # valid cameras associated with non-cloud images
  cam_cat = []        # stores the category of the cameras (cam_global2 (2), cam_global1 (1) or original camera (0) )
  cropped_imgs = []   # all cropped images
  cropped_cams = []   # all cropped cameras
  uncertainties = []  # all uncertainties

  ## First get rid of cloud images
  for img_file in res_files:
    head, tail = os.path.split(img_file);
    name, ext = os.path.splitext(tail);
    cam_name2 = cam_global2 + "%s_corrected.rpb" % name;
    cam_name1 = cam_global  + "%s_corrected.rpb" % name;
    if os.path.exists(cam_name2):
      curr_cam_cat = 2;
      curr_cam_res = cam_name2;
      cam = load_rational_camera(cam_name2);
      uncertainty_file = param_file_dir + "uncertainty_5m.xml";
    elif os.path.exists(cam_name1):
      curr_cam_cat = 1;
      curr_cam_res = cam_name1
      cam = load_rational_camera(cam_name1);
      uncertainty_file = param_file_dir + "uncertainty_5m.xml";
    else:
      curr_cam_cat = 0;
      curr_cam_res = img_file;
      cam = load_rational_camera_nitf(img_file);
      uncertainty_file = param_file_dir + "uncertainty_%dm.xml" % get_satellite_uncertainty(img_file);
    # cropped the image and get the cloud index
    statuscode, cropped_cam, cropped_image, uncertainty = roi_init(img_file, cam, scene, uncertainty_file);
    if statuscode:
      # check the size of the cropped image (large enough in size and roughly cubic)
      # the cropped image should at least overlap ~10% of the image
      # and the size ratio of the image should be larger than 0.8
      crop_ni, crop_nj = image_size(cropped_image);
      if crop_ni > crop_nj:  crop_img_ratio = float(crop_nj)/float(crop_ni);
      elif crop_ni < crop_nj:  crop_img_ratio = float(crop_ni)/float(crop_nj);
      else: crop_img_raio = 1;
      if crop_img_ratio < cropped_image_ratio:
        print "Cropped image size %d x %d of image %s is not cubic, ignored..." % (crop_ni, crop_nj, name)
        sys.stdout.flush();
        continue;
      cropped_edge_image = bvxm_detect_edges(cropped_image, param_file_dir + "bvxmDetectEdgesProcess.xml");
      edge_sum = img_sum(cropped_edge_image);
      ni, nj = image_size(cropped_edge_image);
      percent = edge_sum/(ni*nj);
      if percent > edge_threshold:
        # store the non-cloud images and all other resources
        img_names.append(img_file)
        cam_names.append(curr_cam_res)
        cam_cat.append(curr_cam_cat);
        cropped_imgs.append(cropped_edge_image);
        cropped_cams.append(cropped_cam);
        uncertainties.append(uncertainty)
        cnt = cnt+1

  print "%d out of %d images are valid without cloud" % (cnt, len(res_files))
  sys.stdout.flush()
  ##
  ## Second check whether all non-cloud images have refined camera global 2
  ## if so, then no need to update the world since it will not contribute any camera refining process
  ##
  seed_num = 0;
  for i in range(0,cnt,1):
    if (cam_cat[i]==2):
      seed_num = seed_num + 1;
  if seed_num == cnt:
    print "%d out of %d valid cameras are already in cam_global2, n_seed: %d" % (seed_num, len(img_names), seed_num)
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, n_seed, 0;

  ## Second check whether we have enough img/cam as valid seed
  seed_num = 0
  for i in range(0, cnt, 1):
    if (cam_cat[i] == 1 or cam_cat[i] == 2):
      seed_num = seed_num + 1;
  if seed_num < n_seed_necessary:
    return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, n_seed, 0;

  ## inserting the valid images based on their camera category
  ## picking seeds from global cam2 as many as possible
  print cam_cat
  n_seed = 0;
  for i in range(0, cnt, 1):
    if (cam_cat[i] == 2):
      n_seed = n_seed+1;
      valid_img_names.append(img_names[i])
      valid_cameras.append(cam_names[i])
      cropped_valid_imgs.append(cropped_imgs[i])
      cropped_valid_cams.append(cropped_cams[i])
      valid_uncertainties.append(uncertainties[i])
  print "%d out of %d cameras are already in cam_global2, n_seed: %d" % (len(cropped_valid_imgs), cnt, n_seed)
  sys.stdout.flush()
  ## second check how many cameras are in camera global 1 --- when n_seed < n_seed_necessary, these serve as seed images too
  for i in range(0, cnt, 1):
    if (cam_cat[i] == 1):
      valid_img_names.append(img_names[i])
      valid_cameras.append(cam_names[i])
      cropped_valid_imgs.append(cropped_imgs[i])
      cropped_valid_cams.append(cropped_cams[i])
      valid_uncertainties.append(uncertainties[i])
      if n_seed < n_seed_necessary:
        n_seed = n_seed+1;
  print "%d out of %d cameras are already in cam_global2 and cam_global, n_seed: %d" % (len(cropped_valid_imgs), cnt, n_seed)
  sys.stdout.flush()
  ## append all the other images
  for i in range(0, cnt, 1):
    if (cam_cat[i] == 0):
      valid_img_names.append(img_names[i])
      valid_cameras.append(cam_names[i])
      cropped_valid_imgs.append(cropped_imgs[i])
      cropped_valid_cams.append(cropped_cams[i])
      valid_uncertainties.append(uncertainties[i])

  return cropped_valid_imgs, cropped_valid_cams, valid_uncertainties, valid_img_names, valid_cameras, n_seed, len(cam_cat);
