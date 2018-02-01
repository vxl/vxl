import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

python_path = "./"
#voxel_world_path = "./edges_0.vox";

num_train = 20
skip_list = [8, 14, 15, 18, 23, 29]

# printing the database
batch.print_db()

# first creat an empty world.
print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_params_process("./world_model_params.xml")
batch.run_process()
voxel_world_id = batch.commit_output(0)

# list of the images
f = open('./full_hiafa_images.txt', 'r')
image_fnames = f.readlines()
f.close()
# list of corrected cameras
f = open('./full_hiafa_cam.txt', 'r')
cam_fnames = f.readlines()
f.close()

# number of images used to compute the voxel grid to correct the cameras.
num_train = 5


for i in range(0, len(image_fnames), 1):
    print(str(i))
    keep_moving = 0
    for j in range(0, len(skip_list), 1):
        if i == skip_list[j]:
            keep_moving = 1

    if keep_moving == 1:
        continue

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
        batch.init_process("LoadRationalNITFCameraProcess")
        batch.set_input_string(0, image_filename)
        batch.run_process()
        cam_id = batch.commit_output(0)

    # get a roi from the image
    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_filename)
    batch.set_input_from_db(1, cam_id)
    batch.set_input_from_db(2, voxel_world_id)
    batch.set_params_process(python_path + "roi_params.xml")
    statuscode = batch.run_process()
    if statuscode:
        cropped_cam_id = batch.commit_output(0)
        cropped_image_id = batch.commit_output(1)
        uncertainty_id = batch.commit_output(2)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, cropped_image_id)
        batch.set_input_string(1, "./ini" + str(i) + ".png")
        batch.run_process()

        batch.init_process("bvxmRpcRegistrationProcess")
        batch.set_input_from_db(0, voxel_world_id)
        batch.set_input_from_db(1, cropped_cam_id)
        batch.set_input_from_db(2, cropped_image_id)
        if i < num_train:
            batch.set_input_bool(3, 0)
        else:
            batch.set_input_bool(3, 1)
        batch.set_input_from_db(4, uncertainty_id)
        batch.set_input_unsigned(5, 0)
        batch.set_params_process("bvxmRpcRegistrationProcess.xml")
        batch.run_process()
        corrected_cam_id = batch.commit_output(0)
        edge_image_id = batch.commit_output(1)
        expected_edge_image_id = batch.commit_output(2)

        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, edge_image_id)
        batch.set_input_string(1, str(i) + ".edge_image.jpg")
        batch.run_process()

        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, expected_edge_image_id)
        batch.set_input_string(1, str(i) + ".expected_edge_image.jpg")
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
        batch.run_process()
        normalized_img_id = batch.commit_output(0)
        float1_id = batch.commit_output(1)
        float2_id = batch.commit_output(2)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, normalized_img_id)
        batch.set_input_string(1, "./normalized" + str(i) + ".png")
        batch.run_process()
        curr_image_id = normalized_img_id

        print("Updating World")
        batch.init_process("bvxmUpdateProcess")
        batch.set_input_from_db(0, normalized_img_id)
        batch.set_input_from_db(1, cam_id)
        batch.set_input_from_db(2, voxel_world_id)
        batch.set_input_string(3, app_type)
        batch.set_input_from_db(4, bin_id)
        batch.run_process()
        out_img_id = batch.commit_output(0)
        mask_img_id = batch.commit_output(1)

        print("Display changes")
        batch.init_process("bvxmChangeDetectionDisplayProcess")
        batch.set_params_process("./change_display_params.xml")
        batch.set_input_from_db(0, curr_image_id)
        batch.set_input_from_db(1, out_img_id)
        batch.set_input_from_db(2, mask_img_id)
        batch.run_process()
        change_img_id = batch.commit_output(0)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, change_img_id)
        batch.set_input_string(1, "./change" + str(i) + ".png")
        batch.run_process()

        print("Writing World")
        batch.init_process("bvxmSaveOccupancyRaw")
        batch.set_input_from_db(0, voxel_world_id)
        batch.set_input_string(1, "./world.raw")
        batch.run_process()
