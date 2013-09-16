from bvxm_register import bvxm_batch, dbvalue;
import os
#############################################################################
# PROVIDES higher level python functions to make bvxm_batch
# code more readable/refactored
#############################################################################

## loads/creates scene
def create_scene(scene_params_xml):
  print("Creating Voxel World");
  bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
  bvxm_batch.set_params_process(scene_params_xml);  ## "./bvxmCreateVoxelWorldProcess.xml"
  bvxm_batch.run_process();
  (world_id, world_type) = bvxm_batch.commit_output(0);
  world = dbvalue(world_id, world_type);
  return world

def scene_box(scene):
  bvxm_batch.init_process("bvxmSceneBoxProcess");
  bvxm_batch.set_input_from_db(0, scene);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  lower_left_lon = bvxm_batch.get_output_double(id);
  (id, type) = bvxm_batch.commit_output(1);
  lower_left_lat = bvxm_batch.get_output_double(id);
  (id, type) = bvxm_batch.commit_output(2);
  upper_right_lon = bvxm_batch.get_output_double(id);
  (id, type) = bvxm_batch.commit_output(3);
  upper_right_lat = bvxm_batch.get_output_double(id);
  return lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat;

def roi_init(image_filename, cam, world, roi_init_params_xml):
  bvxm_batch.init_process("bvxmRoiInitProcess");
  bvxm_batch.set_input_string(0,image_filename);
  bvxm_batch.set_input_from_db(1,cam);
  bvxm_batch.set_input_from_db(2,world);
  bvxm_batch.set_params_process(roi_init_params_xml);  ##"bvxmRoiInitProcess.xml");
  statuscode=bvxm_batch.run_process();
  if statuscode:
    (cropped_cam_id, cropped_cam_type) = bvxm_batch.commit_output(0);
    cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type);
    (cropped_image_id, cropped_image_type) = bvxm_batch.commit_output(1);
    cropped_image=dbvalue(cropped_image_id, cropped_image_type);
    (uncertainty_id,uncertainty_type) = bvxm_batch.commit_output(2);
    uncertainty = dbvalue(uncertainty_id,uncertainty_type);
    return statuscode, cropped_cam, cropped_image, uncertainty
  else:
    return statuscode, dbvalue(0, ""), dbvalue(0, ""), dbvalue(0, "")

def bvxm_detect_edges(cropped_image, edge_params_xml):
  bvxm_batch.init_process("bvxmDetectEdgesProcess");
  bvxm_batch.set_input_from_db(0,cropped_image);
  bvxm_batch.set_params_process(edge_params_xml);
  bvxm_batch.run_process();
  (cropped_edge_image_id,cropped_edge_image) = bvxm_batch.commit_output(0);
  cropped_edge_image = dbvalue(cropped_edge_image_id,cropped_edge_image);
  return cropped_edge_image

def update_edges(world, cropped_cam, cropped_edge_image, update_params_xml, scale=0):
  bvxm_batch.init_process("bvxmUpdateEdgesProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.set_input_from_db(1,cropped_cam);
  bvxm_batch.set_input_from_db(2,cropped_edge_image);
  bvxm_batch.set_input_unsigned(3,0);
  bvxm_batch.set_params_process(update_params_xml); # "./bvxmUpdateEdgesProcess.xml");
  bvxm_batch.run_process();

def rpc_registration(world, cropped_cam, cropped_edge_image, uncertainty, shift_3d_flag=0, scale=0):
  bvxm_batch.init_process("bvxmRpcRegistrationProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.set_input_from_db(1,cropped_cam);
  bvxm_batch.set_input_from_db(2,cropped_edge_image);
  bvxm_batch.set_input_bool(3,shift_3d_flag);
  bvxm_batch.set_input_from_db(4,uncertainty);
  bvxm_batch.set_input_unsigned(5,scale);
  bvxm_batch.run_process();
  (cam_id,cam_type) = bvxm_batch.commit_output(0);
  cam = dbvalue(cam_id,cam_type);
  (expected_edge_image_id,expected_edge_image_type) = bvxm_batch.commit_output(1);
  expected_edge_image=dbvalue(expected_edge_image_id,expected_edge_image_type);
  (offset_u_id,offset_u_type) = bvxm_batch.commit_output(2);
  offset_u = bvxm_batch.get_output_double(offset_u_id);
  (offset_v_id,offset_v_type) = bvxm_batch.commit_output(3);
  offset_v = bvxm_batch.get_output_double(offset_v_id);
  return cam, expected_edge_image, offset_u, offset_v

def render_height_map(world):
  print("Rendering height map");
  bvxm_batch.init_process("bvxmHeightmapOrthoProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_d_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  out_h_img = dbvalue(id, type);
  return out_h_img, out_d_img

def render_ortho_edgemap(world, scale=0):
  print("Rendering ortho edge map");
  bvxm_batch.init_process("bvxmEdgemapOrthoProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.set_input_unsigned(1,0);  ## scale
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_e_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  out_e_img_byte = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(2);
  out_h_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(3);
  ortho_cam = dbvalue(id, type);
  return out_e_img, out_e_img_byte, out_h_img, ortho_cam

def render_exp_image(cam, ni, nj, world, app_model, bin_index=0, scale_index=0):
  bvxm_batch.init_process("bvxmRenderExpectedImageProcess");
  bvxm_batch.set_input_from_db(0,cam);
  bvxm_batch.set_input_unsigned(1,ni);
  bvxm_batch.set_input_unsigned(2,nj);
  bvxm_batch.set_input_from_db(3,world);
  bvxm_batch.set_input_string(4,app_model);
  bvxm_batch.set_input_unsigned(5,bin_index);       ## set bin index to be 0 for all images
  bvxm_batch.set_input_unsigned(6,scale_index);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  out_conf_img = dbvalue(id, type);
  return out_img, out_conf_img

def render_exp_edge_img(cam, ni, nj, world, scale=0):
  bvxm_batch.init_process("bvxmExpectedEdgeImageProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.set_input_from_db(1,cam);
  bvxm_batch.set_input_unsigned(2,ni);
  bvxm_batch.set_input_unsigned(3,nj);
  bvxm_batch.set_input_unsigned(4,scale);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  exp_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  exp_img_byte = dbvalue(id, type);
  return exp_img, exp_img_byte

def update_appearance(img, cam, world, app_type, bin_index=0, scale_index=0):
  bvxm_batch.init_process("bvxmUpdateProcess");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_from_db(1,cam);
  bvxm_batch.set_input_from_db(2,world);
  bvxm_batch.set_input_string(3,app_type);
  bvxm_batch.set_input_unsigned(4,bin_index);       ## set bin index to be 0 for all images
  bvxm_batch.set_input_unsigned(5,scale_index);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  density_img = dbvalue(id, type);
  (id, type) = bvxm_batch.commit_output(1);
  density_mask_img = dbvalue(id, type);
  return density_img, density_mask_img

def save_occupancy_raw(world, filename, app_model, scale=0):
  bvxm_batch.init_process("bvxmSaveOccupancyRawProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.set_input_string(1,filename);
  bvxm_batch.set_input_unsigned(2, scale);
  bvxm_batch.set_input_string(3, app_model);
  bvxm_batch.run_process();

############## some utilities, put here for now ##########
def image_to_vrml_points(out_e_img, out_h_img, output_filename, prob_thres, max_scene_height):
  bvxm_batch.init_process("bvrmlImageToPointsProcess");
  bvxm_batch.set_input_from_db(0,out_e_img);
  bvxm_batch.set_input_from_db(1,out_h_img);
  bvxm_batch.set_input_string(2,output_filename);
  bvxm_batch.set_input_float(3,prob_thres);
  bvxm_batch.set_input_float(4,max_scene_height);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_img = dbvalue(id, type);
  return out_img

def segment_image(img, weight_thres, margin=0, min_size=50, sigma=1,neigh=8):
  bvxm_batch.init_process("sdetSegmentImageProcess");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_int(1,margin);
  bvxm_batch.set_input_int(2,neigh);
  bvxm_batch.set_input_float(3,weight_thres);
  bvxm_batch.set_input_float(4,sigma);
  bvxm_batch.set_input_int(5,min_size);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  seg_img = dbvalue(id, type);
  return seg_img

def segment_image_using_edges(img, edge_img, weight_thres, margin=0, min_size=50, sigma=1,neigh=8):
  bvxm_batch.init_process("sdetSegmentUsingEdgesProcess");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_from_db(1,edge_img);
  bvxm_batch.set_input_int(2,margin);
  bvxm_batch.set_input_int(3,neigh);
  bvxm_batch.set_input_float(4,weight_thres);
  bvxm_batch.set_input_float(5,sigma);
  bvxm_batch.set_input_int(6,min_size);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  seg_img = dbvalue(id, type);
  return seg_img

def segment_image_using_height(img, height_img, weight_thres, margin=0, min_size=50, sigma=1,neigh=8):
  bvxm_batch.init_process("sdetSegmentUsingHeightMapProcess");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_from_db(1,height_img);
  bvxm_batch.set_input_int(2,margin);
  bvxm_batch.set_input_int(3,neigh);
  bvxm_batch.set_input_float(4,weight_thres);
  bvxm_batch.set_input_float(5,sigma);
  bvxm_batch.set_input_int(6,min_size);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  seg_img = dbvalue(id, type);
  return seg_img

def segment_image_using_height2(img, height_img, edge_img, weight_thres, margin=0, min_size=50, sigma=1,neigh=8):
  bvxm_batch.init_process("sdetSegmentUsingHeightMapProcess2");
  bvxm_batch.set_input_from_db(0,img);
  bvxm_batch.set_input_from_db(1,height_img);
  bvxm_batch.set_input_from_db(2,edge_img);
  bvxm_batch.set_input_int(3,margin);
  bvxm_batch.set_input_int(4,neigh);
  bvxm_batch.set_input_float(5,weight_thres);
  bvxm_batch.set_input_float(6,sigma);
  bvxm_batch.set_input_int(7,min_size);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  seg_img = dbvalue(id, type);
  return seg_img
