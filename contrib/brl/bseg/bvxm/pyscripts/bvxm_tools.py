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

