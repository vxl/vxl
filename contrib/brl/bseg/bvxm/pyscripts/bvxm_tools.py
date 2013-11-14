from bvxm_register import bvxm_batch,dbvalue;
from bvxm_adaptor import *
from bvxm_volm_adaptor import *
from bvxm_vpgl_adaptor import *
from bvxm_vil_adaptor import *
import os, time, glob
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
def build_edge_world(scene, scene_id, world_dir, image_fnames, cameras, n_seed, param_file_dir, corrected_global_cams, out_dir, save=0):
  ## remove the .vox files if any
  edge_threshold = 15;  ## percent

  edge_files = glob.glob(world_dir + "edges_*.vox");
  for edge_file_name in edge_files:
    print "removing " + edge_file_name + "...";
    os.remove(edge_file_name);

  t1 = time.clock();
  out_imgs = out_dir + "img_%d_%s.tif";
  out_edge_imgs = out_dir + "img_%d.tif";
  cnt_updates = 0;
  for i in range(0,len(image_fnames),1):
    image_fnames[i] = image_fnames[i].strip();
    image_filename=image_fnames[i];
    head, tail = os.path.split(image_filename);
    name, ext = os.path.splitext(tail);
    print name
    print cameras[i]

    print '-----------------------------';
    print "scene: %d, image: %d out of %d" % (scene_id, i, len(image_fnames));
    print '-----------------------------';

    ## check if the camera name has _corrected in it
    if "_corrected" in cameras[i]:
      cam = load_rational_camera(cameras[i]);
      uncertainty_file = param_file_dir + "uncertainty_5m.xml";
    else:
      cam = load_rational_camera_nitf(cameras[i]);
      uncertainty_file = param_file_dir + "uncertainty_%dm.xml" % get_satellite_uncertainty(image_filename);

    print image_filename;
    print uncertainty_file;
    statuscode, cropped_cam, cropped_image, uncertainty = roi_init(image_filename, cam, scene, uncertainty_file);
    print statuscode;

    if statuscode:

      cropped_edge_image = bvxm_detect_edges(cropped_image, param_file_dir + "bvxmDetectEdgesProcess.xml");

      edge_sum = img_sum(cropped_edge_image);
      ni, nj = image_size(cropped_edge_image);
      percent = edge_sum/(ni*nj);  ## use this value to get rid of cloud images
      print " !!!!!! EDGE SUM: " + str(edge_sum) + " image size: " + str(ni) + " " + str(nj) + " total: " + str(ni*nj) + " percentage: " + str(percent);

      if percent < edge_threshold:
        print " !!!!!!!!!!!!!!!! SKIPPING LESS THAN " + str(edge_threshold) + " PERCENT EDGE COVERAGE!\n";
        bvxm_batch.remove_data(cropped_edge_image.id)
        bvxm_batch.remove_data(cropped_image.id)
        bvxm_batch.remove_data(cropped_cam.id)
        bvxm_batch.remove_data(uncertainty.id)
        bvxm_batch.remove_data(cam.id)
        continue;

      if i >= n_seed:

        cam_cor, expected_edge_image, offset_u, offset_v = rpc_registration(scene, cropped_cam, cropped_edge_image, uncertainty, 0, 0);
        if save:
          bvxm_save_image(expected_edge_image, out_exp_edge_imgs % cnt_updates);

        print "------- will correct with offset_u: " + str(offset_u) + " offset_v: " + str(offset_v);
        cam_global_cor = correct_rational_camera(cam, offset_u, offset_v);
        save_rational_camera(cam_global_cor,corrected_global_cams % name);

        update_edges(scene, cam_cor, cropped_edge_image, param_file_dir + "bvxmUpdateEdgesProcess.xml");

        bvxm_batch.remove_data(expected_edge_image.id)
        bvxm_batch.remove_data(cam_global_cor.id);
        bvxm_batch.remove_data(cam_cor.id)
        '''
        offset_u = 0; offset_v = 0;
        print "------- will correct with offset_u: " + str(offset_u) + " offset_v: " + str(offset_v);
        cam_global_cor = correct_rational_camera(cam, offset_u, offset_v);
        save_rational_camera(cam_global_cor,corrected_global_cams % name);
        '''

      else:
        update_edges(scene, cropped_cam, cropped_edge_image, param_file_dir + "bvxmUpdateEdgesProcess.xml");

      cnt_updates = cnt_updates + 1;

      if save:
        bvxm_save_image(cropped_image, out_imgs % (cnt_updates, name));
        bvxm_save_image(cropped_edge_image, out_edge_imgs % cnt_updates);

      bvxm_batch.remove_data(cropped_edge_image.id)

    bvxm_batch.remove_data(cropped_image.id)
    bvxm_batch.remove_data(cropped_cam.id)
    bvxm_batch.remove_data(uncertainty.id)
    bvxm_batch.remove_data(cam.id)

  out_e_img, out_e_img_byte, out_h_img, ortho_cam = render_ortho_edgemap(scene);
  bvxm_save_image(out_e_img, out_dir + "scene_%d_ortho_edge_map.tif" % scene_id);
  bvxm_batch.remove_data(out_e_img.id);
  bvxm_batch.remove_data(out_e_img_byte.id);
  bvxm_batch.remove_data(out_h_img.id);
  bvxm_batch.remove_data(ortho_cam.id);

  t2 = time.clock();
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

## returns scene, image_fnames, cameras, ## only process the scenes with number of resources between min and max cnt
## returns image_fnames, cameras, n_seed, cnt =
def get_scene_files(scene, res, cam_global, cam_global2, min_cnt, max_cnt, n_seed_necessary=5):
  image_fnames = [];
  cameras = [];
  n_seed = 0;
  names = [];
  cnt = 0;
  lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat = scene_box(scene);

  temp_text_res = "./scene_res.txt";
  cnt = scene_resources(res, lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, temp_text_res, "PAN");
  f = open(temp_text_res);
  res_files = f.readlines();
  f.close();
  os.remove(temp_text_res);

  if cnt > max_cnt or cnt < min_cnt:
    return image_fnames, cameras, n_seed, cnt;

  ## first check how many cameras are already in cam_global2 -- use all of these as seed
  for i in range(0, len(res_files), 1):
    res_files[i] = res_files[i].strip();
    head, tail = os.path.split(res_files[i]);
    name, ext = os.path.splitext(tail);

    cam_name = cam_global2 + "%s_corrected.rpb" % name;
    if os.path.exists(cam_name):
      image_fnames.append(res_files[i]);
      cameras.append(cam_name);
      n_seed = n_seed + 1;
      names.append(name);

  print "%d out of %d cameras are already in cam_global2, n_seed: %d" % (len(names), cnt, n_seed)
  ## if all cameras are already corrected skip this scene
  if n_seed == cnt:
    return image_fnames, cameras, n_seed, 0

  ## second check how many cameras are already in cam_global1 -- may use as seed if n_seed < n_seed_necessary
  for i in range(0, len(res_files), 1):
    res_files[i] = res_files[i].strip();
    head, tail = os.path.split(res_files[i]);
    name, ext = os.path.splitext(tail);
    if name in names:
      continue;

    cam_name = cam_global + "%s_corrected.rpb" % name;
    if os.path.exists(cam_name):
      image_fnames.append(res_files[i]);
      cameras.append(cam_name);
      if n_seed < n_seed_necessary:
        n_seed = n_seed + 1;
      names.append(name);

  print "%d out of %d cameras are already in cam_global2 and cam_global, n_seed: %d" % (len(names), cnt, n_seed)
  ## if there are still not enough seeds skip this scene
  if n_seed < n_seed_necessary:
    return image_fnames, cameras, n_seed, 0

  ## third append the original cameras
  for i in range(0, len(res_files), 1):
    res_files[i] = res_files[i].strip();
    head, tail = os.path.split(res_files[i]);
    name, ext = os.path.splitext(tail);
    if name in names:
      continue;

    image_fnames.append(res_files[i]);
    cameras.append(res_files[i]);

  return image_fnames, cameras, n_seed, cnt
