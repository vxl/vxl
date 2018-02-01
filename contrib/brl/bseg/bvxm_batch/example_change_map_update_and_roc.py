# This is a sample script for:
#  updating a change map for better ROC performance
#  assumes that ground-truth changes are marked as polygonal regions via tools in bwm_main and saved as binary files
# by Ozge C. Ozcanli
# 11/16/2008

import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

# number of images with correct cameras.
num_cam = 2
normalize_and_save = 0

# first creat an empty world.
print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_params_process("./world_model_params.xml")
batch.run_process()
voxel_world_id = batch.commit_output(0)

python_path = "./"
change_type = "sewage"

import os
import shutil
output_path = python_path + "testing_output/"
# if os.path.exists(output_path):
# shutil.rmtree(output_path);
# os.mkdir(output_path);
if os.path.exists(output_path) != 1:
    os.mkdir(output_path)

# list of the images
f = open("testing_images.txt", 'r')
image_fnames = f.readlines()
f.close()
# list of corrected cameras
f = open("testing_cameras.txt", 'r')
cam_fnames = f.readlines()
f.close()

import array
j_size = 5
j_arr = [0 for col in range(j_size)]
j_arr[0] = 0.1
j_arr[1] = 0.3
j_arr[2] = 0.5
j_arr[3] = 0.7
j_arr[4] = 0.9

for i in range(0, len(image_fnames), 1):
    print(str(i))

    image_filename = image_fnames[i]
    image_filename = image_filename[:-1]

    if i < num_cam:
        cam_name = cam_fnames[i]
        cam_name = cam_name[:-1]
        batch.init_process("LoadRationalCameraProcess")
        batch.set_input_string(0, cam_name)
        batch.run_process()
        orig_cam_id = batch.commit_output(0)
    else:
        batch.init_process("LoadRationalCameraNITFProcess")
        batch.set_input_string(0, image_filename)
        batch.run_process()
        orig_cam_id = batch.commit_output(0)

    # get a roi from the image
    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_filename)
    batch.set_input_from_db(1, orig_cam_id)
    batch.set_input_from_db(2, voxel_world_id)
    batch.set_params_process("./roi_params.xml")
    statuscode = batch.run_process()
    print statuscode
    if statuscode:
        cropped_cam_id = batch.commit_output(0)
        cropped_image_id = batch.commit_output(1)
        uncertainty_id = batch.commit_output(2)

        curr_scale = 0

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

        if normalize_and_save == 1:
            # Normalizing images
            print(" Normalizing Image ")
            batch.init_process("bvxmNormalizeImageProcess")
            batch.set_params_process("./normalize.xml")
            batch.set_input_from_db(0, cropped_image_id)
            batch.set_input_from_db(1, cropped_cam_id)
            batch.set_input_from_db(2, voxel_world_id)
            batch.set_input_string(3, app_type)
            batch.set_input_from_db(4, bin_id)
            batch.set_input_unsigned(5, curr_scale)
            batch.run_process()
            normalized_img_id = batch.commit_output(0)
            float1_id = batch.commit_output(1)
            float2_id = batch.commit_output(2)

            print("Saving Image")
            batch.init_process("SaveImageViewProcess")
            batch.set_input_from_db(0, normalized_img_id)
            batch.set_input_string(
                1, output_path + "normalized" + str(i) + ".png")
            batch.run_process()

            curr_image_id = normalized_img_id

            print("Detect Changes")
            batch.init_process("bvxmDetectChangesProcess")
            batch.set_input_from_db(0, curr_image_id)
            batch.set_input_from_db(1, cropped_cam_id)
            batch.set_input_from_db(2, voxel_world_id)
            batch.set_input_string(3, app_type)
            batch.set_input_from_db(4, bin_id)
            batch.set_input_unsigned(5, curr_scale)
            batch.run_process()
            out_img_id = batch.commit_output(0)
            mask_img_id = batch.commit_output(1)

            print("Saving Prob Image")
            batch.init_process("SaveImageViewProcess")
            batch.set_input_from_db(0, out_img_id)
            batch.set_input_string(
                1, output_path + "density_map" + str(i) + ".tiff")
            batch.run_process()

            print("Saving Map Image")
            batch.init_process("SaveImageViewBinaryProcess")
            batch.set_input_from_db(0, mask_img_id)
            batch.set_input_string(
                1, output_path + "density_mask" + str(i) + ".bin")
            batch.run_process()
        else:
            print("Loading normalized Image")
            batch.init_process("LoadImageViewProcess")
            batch.set_input_string(
                0, output_path + "normalized" + str(i) + ".png")
            batch.run_process()
            curr_image_id = batch.commit_output(0)

            print("Loading Prob Image")
            batch.init_process("LoadImageViewProcess")
            batch.set_input_string(
                0, output_path + "density_map" + str(i) + ".tiff")
            batch.run_process()
            out_img_id = batch.commit_output(0)

            print("Loading Map Image")
            batch.init_process("LoadImageViewBinaryProcess")
            batch.set_input_string(
                0, output_path + "density_mask" + str(i) + ".bin")
            batch.run_process()
            mask_img_id = batch.commit_output(0)

        print("Convert density to prob map")
        batch.init_process("brecDensityToProbMapProcess")
        batch.set_params_process("./density_to_prob_map_params.xml")
        batch.set_input_from_db(0, out_img_id)
        batch.run_process()
        prob_map_id = batch.commit_output(0)

        for j in range(j_size):
            print("Threshold prob map")
            batch.init_process("brecProbMapThresholdProcess")
            batch.set_input_from_db(0, prob_map_id)
            batch.set_input_from_db(1, mask_img_id)
            batch.set_input_from_db(2, cropped_image_id)
            batch.set_input_float(3, j_arr[j])
            batch.run_process()
            thres_img_id = batch.commit_output(0)

            print("Saving Thresholded Image")
            batch.init_process("SaveImageViewProcess")
            batch.set_input_from_db(0, thres_img_id)
            batch.set_input_string(
                1, output_path + "thres" + str(i) + "_" + str(j_arr[j]) + ".png")
            batch.run_process()

        # ground-truth polygons for change areas need to be prepared via tools in bwm_main and saved as binary files
        # each test image need to have a separate parameter xml file that records the test image dimensions after ROI cropping process
        # polygonal regions need to be marked wrt to cropped ROI coordinate
        # frame (i.e. first crop the image, then open the cropped version in
        # bwm_main for ground-truthing)
        print("Run the gt generator process")
        batch.init_process("bvglGenerateMaskProcess")
        batch.set_params_process(
            "./params_generate_mask_test_img_" + str(i) + ".xml")
        batch.set_input_string(
            0, "./gt/test_img_" + str(i) + "_gt_" + change_type + ".bin")
        batch.set_input_string(1, change_type)  # change type
        batch.run_process()
        gt_byte_id = batch.commit_output(0)
        gt_mask_id = batch.commit_output(1)

        print("Saving GT Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, gt_byte_id)
        batch.set_input_string(
            1, output_path + "test_img_" + str(i) + "_gt.png")
        batch.run_process()

        print("Run the ROC process")
        batch.init_process("brecProbMapROCProcess")
        batch.set_input_from_db(0, prob_map_id)
        batch.set_input_from_db(1, mask_img_id)
        batch.set_input_from_db(2, gt_mask_id)
        batch.set_input_string(
            3, output_path + "out_roc_" + str(i) + ".txt")
        batch.run_process()

        npasses = 1
        sigma = 0.05
        print("Updating change map")
        batch.init_process("brecUpdateChangesProcess")
        batch.set_input_from_db(0, out_img_id)
        batch.set_input_from_db(1, curr_image_id)
        batch.set_input_unsigned(2, npasses)
        batch.set_input_float(3, sigma)
        batch.run_process()
        out_updated_img_id = batch.commit_output(0)
        out_updated_byte_img_id = batch.commit_output(1)

        print("Saving Image")
        batch.init_process("SaveImageViewProcess")
        batch.set_input_from_db(0, out_updated_byte_img_id)
        batch.set_input_string(1, output_path + "change_map_updated_n" + str(
            npasses) + "_sigma_" + str(sigma) + "_" + str(i) + ".png")
        batch.run_process()

        print("Run the ROC process")
        batch.init_process("brecProbMapROCProcess")
        batch.set_input_from_db(0, out_updated_img_id)
        batch.set_input_from_db(1, mask_img_id)
        batch.set_input_from_db(2, gt_mask_id)
        batch.set_input_string(3, output_path + "out_roc_updated_n_" + str(
            npasses) + "_sigma_" + str(sigma) + "_" + str(i) + ".txt")
        batch.run_process()
