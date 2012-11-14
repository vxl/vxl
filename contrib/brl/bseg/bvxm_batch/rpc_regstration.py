#############################################################################
## Created by Vishal Jain
## Nov 10, 2009
## LEMS, Brown University
#############################################################################

import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

f=open('images.txt', 'r')
image_fnames=f.readlines();
f.close();

print("Creating Voxel World");
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
bvxm_batch.set_params_process("./bvxmCreateVoxelWorldProcess.xml");
bvxm_batch.set_input_string(1,"ocp_opinion");
bvxm_batch.run_process();
(world_id,world_type)= bvxm_batch.commit_output(0);
world = dbvalue(world_id,world_type);

for i in range(0,len(image_fnames),1):
  image_fnames[i] = image_fnames[i].strip();

  bvxm_batch.init_process("vpglLoadRationalCameraNITFProcess");
  bvxm_batch.set_input_string(0,image_fnames[i]);
  bvxm_batch.run_process();
  (cam_id,cam_type)=bvxm_batch.commit_output(0);
  cam = dbvalue(cam_id,cam_type);

  bvxm_batch.init_process("bvxmRoiInitProcess");
  bvxm_batch.set_input_string(0,image_fnames[i]);
  bvxm_batch.set_input_from_db(1,cam);
  bvxm_batch.set_input_from_db(2,world);
  bvxm_batch.set_params_process("roi_params.xml");
  statuscode=bvxm_batch.run_process();

  print statuscode;
  if statuscode:
    (cropped_cam_id, cropped_cam_type) = bvxm_batch.commit_output(0);
    cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type);

    (cropped_image_id, cropped_image_type) = bvxm_batch.commit_output(1);
    cropped_image=dbvalue(cropped_image_id, cropped_image_type);

    (uncertainty_id,uncertainty_type) = bvxm_batch.commit_output(2);
    uncertainty = dbvalue(uncertainty_id,uncertainty_type);

    print("Compass edge detector  Image");
    bvxm_batch.init_process("bilCompassEdgeDetectorProcess");
    bvxm_batch.set_input_from_db(0,cropped_image);
    bvxm_batch.set_input_unsigned(1,8);
    bvxm_batch.set_input_double(2,2.0);
    bvxm_batch.set_input_double(3,0.4);
    bvxm_batch.run_process();
    (cropped_edge_image_id,cropped_edge_image_type) = bvxm_batch.commit_output(0);
    cropped_edge_image = dbvalue(cropped_edge_image_id,cropped_edge_image_type);

    bvxm_batch.init_process("bvxmRpcRegistrationProcess");
    bvxm_batch.set_input_from_db(0,world);
    bvxm_batch.set_input_from_db(1,cropped_cam);
    bvxm_batch.set_input_from_db(2,cropped_edge_image);
    bvxm_batch.set_input_bool(3,0);
    bvxm_batch.set_input_float(4,25);
    bvxm_batch.set_input_float(5,0);
    bvxm_batch.set_input_unsigned(6,0);
    bvxm_batch.run_process();
    (id, type) = bvxm_batch.commit_output(0);
    corrected_cam = dbvalue(id, type);
    (id, type) = bvxm_batch.commit_output(1);
    corrected_img = dbvalue(id, type);
    cam_name_local="./camera"+str(i)+".txt";

    print(" saving camera  ");
    bvxm_batch.init_process("vpglSaveRationalCameraProcess");
    bvxm_batch.set_input_from_db(0,corrected_cam);
    bvxm_batch.set_input_string(1,cam_name_local);
    bvxm_batch.run_process();

    print("Saving Image");
    bvxm_batch.init_process("vilSaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,corrected_img);
    bvxm_batch.set_input_string(1,"./edge"+str(i)+".png");
    bvxm_batch.run_process();

    print("Saving Image");
    bvxm_batch.init_process("vilSaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,cropped_edge_image);
    bvxm_batch.set_input_string(1,"./compassedge"+str(i)+".png");
    bvxm_batch.run_process();

