#This is a sample script for:
#  detecting edges from lidar input
#  updating the edge exitence probability voxel world using lidar
#  RPC camera correction using the voxel world
#by Ibrahim Eden
#09/27/2008

import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

lidar_1st_image_original = "C:/test_images/BaghdadLIDAR/dem_1m_a1_baghdad_tile39.tif";
lidar_2nd_image_original = "C:/test_images/BaghdadLIDAR/dem_1m_a2_baghdad_tile39.tif";

# first creat an empty world
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
bvxm_batch.set_params_process("bvxmCreateVoxelWorldProcess.xml");
bvxm_batch.run_process();
world_id = bvxm_batch.commit_output(0);

# initialize the lidar data
bvxm_batch.init_process("bvxmLidarInitProcess");
bvxm_batch.set_input_string(0,lidar_1st_image_original);
bvxm_batch.set_input_string(1,lidar_2nd_image_original);
bvxm_batch.set_input_from_db(2,world_id);
bvxm_batch.run_process();
lidar_camera_id = bvxm_batch.commit_output(0);
lidar_1st_image_id = bvxm_batch.commit_output(1);
lidar_2nd_image_id = bvxm_batch.commit_output(2);
lidar_mask_image_id = bvxm_batch.commit_output(3);

# detect edges from lidar input
bvxm_batch.init_process("bvxmLidarEdgeDetectionProcess");
bvxm_batch.set_input_from_db(0,lidar_1st_image_id);
bvxm_batch.set_input_from_db(1,lidar_2nd_image_id);
bvxm_batch.set_params_process("bvxmLidarEdgeDetectionProcess.xml");
bvxm_batch.run_process();
lidar_height_id = bvxm_batch.commit_output(0);
lidar_edges_id = bvxm_batch.commit_output(1);
lidar_edges_prob_id = bvxm_batch.commit_output(2);

# update the voxel world using lidar edges
bvxm_batch.init_process("bvxmUpdateEdgesLidarProcess");
bvxm_batch.set_input_from_db(0,lidar_height_id);
bvxm_batch.set_input_from_db(1,lidar_edges_id);
bvxm_batch.set_input_from_db(2,lidar_edges_prob_id);
bvxm_batch.set_input_from_db(3,lidar_camera_id);
bvxm_batch.set_input_from_db(4,world_id);
bvxm_batch.set_input_unsigned(5,0);
bvxm_batch.run_process();

bvxm_batch.init_process("bvxmSaveEdgesRaw");
bvxm_batch.set_input_from_db(0,world_id);
bvxm_batch.set_input_string(1,"output_edges.raw");
bvxm_batch.set_input_unsigned(2,0);
bvxm_batch.run_process();

# list of the images
f=open('./images.txt', 'r')
image_fnames=f.readlines();
f.close();

# list of the cameras
f=open('./cameras.txt', 'r')
cam_fnames=f.readlines();
f.close();

for i in range(0,len(image_fnames),1):
  image_fnames[i] = image_fnames[i].strip();
  cam_fnames[i] = cam_fnames[i].strip();

  image_filename=image_fnames[i];
  cam_name=cam_fnames[i];
  
  bvxm_batch.init_process("LoadRationalCameraNITFProcess");
  bvxm_batch.set_input_string(0,image_filename);
  bvxm_batch.run_process();
  cam_id = bvxm_batch.commit_output(0);

  # get a roi from the image 
  bvxm_batch.init_process("bvxmRoiInitProcess");
  bvxm_batch.set_input_string(0,image_filename);
  bvxm_batch.set_input_from_db(1,cam_id);
  bvxm_batch.set_input_from_db(2,world_id);
  bvxm_batch.set_params_process("bvxmRoiInitProcess.xml");
  statuscode=bvxm_batch.run_process();
  
  print statuscode;
  if statuscode:
    cropped_cam_id = bvxm_batch.commit_output(0);
    cropped_image_id = bvxm_batch.commit_output(1);  
    uncertainty_id = bvxm_batch.commit_output(2);  

    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,cropped_image_id);
    bvxm_batch.set_input_string(1,str(i)+".cropped_image.tiff");
    bvxm_batch.run_process();
       
    bvxm_batch.init_process("bvxmRpcRegistrationProcess");
    bvxm_batch.set_input_from_db(0,world_id);
    bvxm_batch.set_input_from_db(1,cropped_cam_id);
    bvxm_batch.set_input_from_db(2,cropped_image_id);
    if i<num_train:
      bvxm_batch.set_input_bool(3,0);
    else:
      bvxm_batch.set_input_bool(3,1);
    bvxm_batch.set_input_from_db(4,uncertainty_id);
    bvxm_batch.set_input_unsigned(5,0);
    bvxm_batch.set_params_process("bvxmRpcRegistrationProcess.xml");
    bvxm_batch.run_process();
    corrected_cam_id = bvxm_batch.commit_output(0);
    edge_image_id = bvxm_batch.commit_output(1);
    expected_edge_image_id = bvxm_batch.commit_output(2);
    
    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,edge_image_id);
    bvxm_batch.set_input_string(1,str(i)+".edge_image.tiff");
    bvxm_batch.run_process();

    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,expected_edge_image_id);
    bvxm_batch.set_input_string(1,str(i)+".expected_edge_image.tiff");
    bvxm_batch.run_process();

    print("Done");
