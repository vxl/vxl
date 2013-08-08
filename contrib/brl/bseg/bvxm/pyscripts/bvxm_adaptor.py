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
  return cam, expected_edge_image

def render_height_map(world):
  print("Rendering height map");
  bvxm_batch.init_process("bvxmHeightmapOrthoProcess");
  bvxm_batch.set_input_from_db(0,world);
  bvxm_batch.run_process();
  (id, type) = bvxm_batch.commit_output(0);
  out_h_img = dbvalue(id, type);
  return out_h_img

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
  return out_e_img, out_e_img_byte, out_h_img

############## some utilities, put here for now ##########
def image_to_vrml_points(out_e_img, out_h_img, output_filename, prob_thres):
  bvxm_batch.init_process("bvrmlImageToPointsProcess");
  bvxm_batch.set_input_from_db(0,out_e_img);
  bvxm_batch.set_input_from_db(1,out_h_img);
  bvxm_batch.set_input_string(2,output_filename);
  bvxm_batch.set_input_float(3,prob_thres);
  bvxm_batch.run_process();
