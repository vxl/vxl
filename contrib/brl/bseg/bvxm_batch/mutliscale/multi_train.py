import bvxm_batch
bvxm_batch.register_processes();
bvxm_batch.register_datatypes();

python_path = "./";
# number of images used to compute the voxel grid to correct the cameras.
num_train = 5;

# first creat an empty world.
print("Creating Voxel World");
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess");
bvxm_batch.set_params_process("./world_model_params.xml");
bvxm_batch.run_process();
voxel_world_id = bvxm_batch.commit_output(0);

########################################
#LIDAR update

print("Writing World");
bvxm_batch.init_process("bvxmSaveOccupancyRaw");
bvxm_batch.set_input_from_db(0,voxel_world_id);
bvxm_batch.set_input_string(1,"./world.raw");
bvxm_batch.run_process();

print("Creating Lidar");
bvxm_batch.init_process("bvxmLidarInitProcess");
bvxm_batch.set_params_process("lidar_params.xml");
bvxm_batch.set_input_string(0,"E:/LIDAR/BaghdadLIDAR/dem_1m_a1_baghdad_tile39.tif");
bvxm_batch.set_input_string(1,"E:/LIDAR/BaghdadLIDAR/dem_1m_a2_baghdad_tile39.tif");
bvxm_batch.set_input_from_db(2,voxel_world_id);
bvxm_batch.run_process();
cam_id = bvxm_batch.commit_output(0);
lidar_id = bvxm_batch.commit_output(1);

print("Updating World");
bvxm_batch.init_process("bvxmUpdateLidarProcess");
bvxm_batch.set_input_from_db(0,lidar_id);
bvxm_batch.set_input_from_db(1,cam_id);
bvxm_batch.set_input_from_db(2,voxel_world_id);
bvxm_batch.set_input_unsigned(3,0);
bvxm_batch.run_process();
 
out_img_id = bvxm_batch.commit_output(0);
mask_img_id = bvxm_batch.commit_output(1);
###################################################


print voxel_world_id;
#list of the images
f=open('./full_hiafa_images.txt', 'r')
image_fnames=f.readlines();
f.close();
# list of corrected cameras
f=open('./full_hiafa_cam.txt', 'r')
cam_fnames=f.readlines();
f.close();

for i in range(1,len(image_fnames),1):
  image_filename=image_fnames[i]
  image_filename=image_filename[:-1];
  cam_name=cam_fnames[i];
  cam_name=cam_name[:-1];
  if i<num_train:
    bvxm_batch.init_process("LoadRationalCameraProcess");
    bvxm_batch.set_input_string(0,cam_name);
    bvxm_batch.run_process();
    cam_id = bvxm_batch.commit_output(0);
  else:
    bvxm_batch.init_process("LoadRationalCameraNITFProcess");
    bvxm_batch.set_input_string(0,image_filename);
    bvxm_batch.run_process();
    cam_id = bvxm_batch.commit_output(0);
  print cam_id;
  # get a roi from the image 
  bvxm_batch.init_process("bvxmRoiInitProcess");
  bvxm_batch.set_input_string(0,image_filename);
  bvxm_batch.set_input_from_db(1,cam_id);
  bvxm_batch.set_input_from_db(2,voxel_world_id);
  bvxm_batch.set_params_process(python_path + "roi_params.xml");
  statuscode=bvxm_batch.run_process();
  print statuscode;
  if statuscode:
    cropped_cam_id = bvxm_batch.commit_output(0);
    cropped_image_id = bvxm_batch.commit_output(1);  
    
    print cropped_image_id;
    # RPC camera correction 
    bvxm_batch.init_process("bvxmGenerateEdgeMapProcess");
    bvxm_batch.set_input_from_db(0,cropped_image_id);
    bvxm_batch.set_params_process(python_path + "edge_map_params.xml");
    bvxm_batch.run_process();
    cropped_edge_image_id = bvxm_batch.commit_output(0);
    print("Detect Scale");
    bvxm_batch.init_process("bvxmDetectScaleProcess");
    bvxm_batch.set_input_from_db(0,voxel_world_id);
    bvxm_batch.set_input_from_db(1,cropped_cam_id);
    bvxm_batch.set_input_from_db(2,cropped_image_id);
    statuscode=bvxm_batch.run_process();
    curr_scale_id = bvxm_batch.commit_output(0);
     
    bvxm_batch.init_process("bvxmRpcRegistrationProcess");
    bvxm_batch.set_input_from_db(0,voxel_world_id);
    bvxm_batch.set_input_from_db(1,cropped_cam_id);
    bvxm_batch.set_input_from_db(2,cropped_edge_image_id);
    if i<num_train:
      bvxm_batch.set_input_bool(3,0);
    else:
      bvxm_batch.set_input_bool(3,1);
    bvxm_batch.set_input_from_db(4,curr_scale_id);
    bvxm_batch.set_params_process(python_path + "rpc_registration_parameters.xml");
    bvxm_batch.run_process();
    cam_id = bvxm_batch.commit_output(0);
    voxel_image_id = bvxm_batch.commit_output(1);
    
    print cam_id;
    
    print("Saving Image");
    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,cropped_image_id);
    bvxm_batch.set_input_string(1,"./initial/ini"+str(i)+".png");
    bvxm_batch.run_process();
    
    
    map_type="10bins_1d_radial";
    print("Illumination Index");
    bvxm_batch.init_process("bvxmIllumIndexProcess");
    bvxm_batch.set_input_string(0,map_type);
    bvxm_batch.set_input_string(1,image_filename);
    bvxm_batch.set_input_unsigned(2,8);
    bvxm_batch.set_input_unsigned(3,0);
    bvxm_batch.run_process();
    bin_id = bvxm_batch.commit_output(0);
    print bin_id;
    
    app_type="apm_mog_grey";
    
    print voxel_world_id;
    
    # Normalizing images
    print(" Normalizing Image ");
    bvxm_batch.init_process("bvxmNormalizeImageProcess");
    bvxm_batch.set_params_process("./normalize.xml");
    bvxm_batch.set_input_from_db(0,cropped_image_id);
    bvxm_batch.set_input_from_db(1,cam_id);
    bvxm_batch.set_input_from_db(2,voxel_world_id);
    bvxm_batch.set_input_string(3,app_type);
    bvxm_batch.set_input_from_db(4,bin_id);
    bvxm_batch.set_input_from_db(5,curr_scale_id);
    bvxm_batch.run_process();
    normalized_img_id = bvxm_batch.commit_output(0);
    float1_id = bvxm_batch.commit_output(1);
    float2_id = bvxm_batch.commit_output(2);
    
    print("Saving Image");
    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,normalized_img_id);
    bvxm_batch.set_input_string(1,"./normalized/normalized"+str(i)+".png");
    bvxm_batch.run_process();
    
    curr_image_id=normalized_img_id;
    
    print("Updating World");
    bvxm_batch.init_process("bvxmUpdateProcess");
    bvxm_batch.set_input_from_db(0,curr_image_id);
    bvxm_batch.set_input_from_db(1,cam_id);
    bvxm_batch.set_input_from_db(2,voxel_world_id);
    bvxm_batch.set_input_string(3,app_type);
    bvxm_batch.set_input_from_db(4,bin_id);
    bvxm_batch.set_input_from_db(5,curr_scale_id);
    bvxm_batch.run_process();
    out_img_id = bvxm_batch.commit_output(0);
    mask_img_id = bvxm_batch.commit_output(1);
    
    print("Display changes");
    bvxm_batch.init_process("bvxmChangeDetectionDisplayProcess");
    bvxm_batch.set_params_process("./change_display_params.xml");
    bvxm_batch.set_input_from_db(0,cropped_image_id);
    bvxm_batch.set_input_from_db(1,out_img_id);
    bvxm_batch.set_input_from_db(2,mask_img_id);
    bvxm_batch.run_process();
    change_img_id = bvxm_batch.commit_output(0);
    prob_img_id = bvxm_batch.commit_output(1);
    
    print("Saving Image");
    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,change_img_id);
    bvxm_batch.set_input_string(1,"./changes/change"+str(i)+".png");
    bvxm_batch.run_process();

    print("Saving Image");
    bvxm_batch.init_process("SaveImageViewProcess");
    bvxm_batch.set_input_from_db(0,prob_img_id);
    bvxm_batch.set_input_string(1,"./changes/prob_map"+str(i)+".png");
    bvxm_batch.run_process();
    
    print("Writing World");
    bvxm_batch.init_process("bvxmSaveOccupancyRaw");
    bvxm_batch.set_input_from_db(0,voxel_world_id);
    bvxm_batch.set_input_string(1,"./world"+str(i)+".raw");
    bvxm_batch.set_input_from_db(2,curr_scale_id);
    bvxm_batch.run_process();

# printing the database
bvxm_batch.print_db();

