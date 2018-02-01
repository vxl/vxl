import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

python_path = "./"
# number of images used to compute the voxel grid to correct the cameras.
num_train = 5

# first creat an empty world.
print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_params_process("./world_model_params.xml")
batch.run_process()
voxel_world_id = batch.commit_output(0)

########################################
# LIDAR update

print("Writing World")
batch.init_process("bvxmSaveOccupancyRaw")
batch.set_input_from_db(0, voxel_world_id)
batch.set_input_string(1, "./world.raw")
batch.run_process()

print("Creating Lidar")
batch.init_process("bvxmLidarInitProcess")
batch.set_params_process("lidar_params.xml")
batch.set_input_string(
    0, "E:/LIDAR/BaghdadLIDAR/dem_1m_a1_baghdad_tile39.tif")
batch.set_input_string(
    1, "E:/LIDAR/BaghdadLIDAR/dem_1m_a2_baghdad_tile39.tif")
batch.set_input_from_db(2, voxel_world_id)
batch.run_process()
cam_id = batch.commit_output(0)
lidar_id = batch.commit_output(1)

print("Updating World")
batch.init_process("bvxmUpdateLidarProcess")
batch.set_input_from_db(0, lidar_id)
batch.set_input_from_db(1, cam_id)
batch.set_input_from_db(2, voxel_world_id)
batch.set_input_unsigned(3, 0)
batch.run_process()

out_img_id = batch.commit_output(0)
mask_img_id = batch.commit_output(1)
###################################################


print voxel_world_id
# list of the images
f = open('./full_hiafa_images.txt', 'r')
image_fnames = f.readlines()
f.close()
# list of corrected cameras
f = open('./full_hiafa_cam.txt', 'r')
cam_fnames = f.readlines()
f.close()

for i in range(1, len(image_fnames), 1):
    image_filename = image_fnames[i]
    image_filename = image_filename[:-1]
    cam_name = cam_fnames[i]
    cam_name = cam_name[:-1]
    if i < num_train:
        batch.init_process("LoadRationalCameraProcess")
        batch.set_input_string(0, cam_name)
        batch.run_process()
        cam_id = batch.commit_output(0)
    else:
        batch.init_process("LoadRationalCameraNITFProcess")
        batch.set_input_string(0, image_filename)
        batch.run_process()
        cam_id = batch.commit_output(0)
    print cam_id
    # get a roi from the image
    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_filename)
    batch.set_input_from_db(1, cam_id)
    batch.set_input_from_db(2, voxel_world_id)
    batch.set_params_process(python_path + "roi_params.xml")
    statuscode = batch.run_process()
    print statuscode
    if statuscode:
        cropped_cam_id = batch.commit_output(0)
        cropped_image_id = batch.commit_output(1)

        print cropped_image_id
        # RPC camera correction
        batch.init_process("bvxmGenerateEdgeMapProcess")
        batch.set_input_from_db(0, cropped_image_id)
        batch.set_params_process(python_path + "edge_map_params.xml")
        batch.run_process()
        cropped_edge_image_id = batch.commit_output(0)
        print("Detect Scale")
        batch.init_process("bvxmDetectScaleProcess")
        batch.set_input_from_db(0, voxel_world_id)
        batch.set_input_from_db(1, cropped_cam_id)
        batch.set_input_from_db(2, cropped_image_id)
        statuscode = batch.run_process()
        curr_scale_id = batch.commit_output(0)

        batch.init_process("bvxmRpcRegistrationProcess")
        batch.set_input_from_db(0, voxel_world_id)
        batch.set_input_from_db(1, cropped_cam_id)
        batch.set_input_from_db(2, cropped_edge_image_id)
        if i < num_train:
            batch.set_input_bool(3, 0)
        else:
            batch.set_input_bool(3, 1)
        batch.set_input_from_db(4, curr_scale_id)
        batch.set_params_process(
            python_path + "rpc_registration_parameters.xml")
        batch.run_process()
        cam_id = batch.commit_output(0)
        voxel_image_id = batch.commit_output(1)

        print cam_id

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, cropped_image_id)
        batch.set_input_string(1, "./initial/ini" + str(i) + ".png")
        batch.run_process()

        map_type = "10bins_1d_radial"
        print("Illumination Index")
        batch.init_process("bvxmIllumIndexProcess")
        batch.set_input_string(0, map_type)
        batch.set_input_string(1, image_filename)
        batch.set_input_unsigned(2, 8)
        batch.set_input_unsigned(3, 0)
        batch.run_process()
        bin_id = batch.commit_output(0)
        print bin_id

        app_type = "apm_mog_grey"

        print voxel_world_id

        # Normalizing images
        print(" Normalizing Image ")
        batch.init_process("bvxmNormalizeImageProcess")
        batch.set_params_process("./normalize.xml")
        batch.set_input_from_db(0, cropped_image_id)
        batch.set_input_from_db(1, cam_id)
        batch.set_input_from_db(2, voxel_world_id)
        batch.set_input_string(3, app_type)
        batch.set_input_from_db(4, bin_id)
        batch.set_input_from_db(5, curr_scale_id)
        batch.run_process()
        normalized_img_id = batch.commit_output(0)
        float1_id = batch.commit_output(1)
        float2_id = batch.commit_output(2)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, normalized_img_id)
        batch.set_input_string(
            1, "./normalized/normalized" + str(i) + ".png")
        batch.run_process()

        curr_image_id = normalized_img_id

        print("Updating World")
        batch.init_process("bvxmUpdateProcess")
        batch.set_input_from_db(0, curr_image_id)
        batch.set_input_from_db(1, cam_id)
        batch.set_input_from_db(2, voxel_world_id)
        batch.set_input_string(3, app_type)
        batch.set_input_from_db(4, bin_id)
        batch.set_input_from_db(5, curr_scale_id)
        batch.run_process()
        out_img_id = batch.commit_output(0)
        mask_img_id = batch.commit_output(1)

        print("Display changes")
        batch.init_process("bvxmChangeDetectionDisplayProcess")
        batch.set_params_process("./change_display_params.xml")
        batch.set_input_from_db(0, cropped_image_id)
        batch.set_input_from_db(1, out_img_id)
        batch.set_input_from_db(2, mask_img_id)
        batch.run_process()
        change_img_id = batch.commit_output(0)
        prob_img_id = batch.commit_output(1)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, change_img_id)
        batch.set_input_string(1, "./changes/change" + str(i) + ".png")
        batch.run_process()

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, prob_img_id)
        batch.set_input_string(1, "./changes/prob_map" + str(i) + ".png")
        batch.run_process()

        print("Writing World")
        batch.init_process("bvxmSaveOccupancyRaw")
        batch.set_input_from_db(0, voxel_world_id)
        batch.set_input_string(1, "./world" + str(i) + ".raw")
        batch.set_input_from_db(2, curr_scale_id)
        batch.run_process()

# printing the database
batch.print_db()
