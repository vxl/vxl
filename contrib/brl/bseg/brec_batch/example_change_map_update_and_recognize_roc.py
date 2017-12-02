# This is a sample script for:
#  updating a change map and suppressing the recognized vehicles changes
#  assumes that ground-truth changes are marked as polygonal regions via tools in bwm_main and saved as binary files
# by Ozge C. Ozcanli
# 11/16/2008

import bvxm_batch
bvxm_batch.register_processes()
bvxm_batch.register_datatypes()

# number of images with correct cameras.
num_cam = 2
normalize_and_save = 0

# first creat an empty world.
print("Creating Voxel World")
bvxm_batch.init_process("bvxmCreateVoxelWorldProcess")
bvxm_batch.set_params_process("./world_model_params.xml")
bvxm_batch.run_process()
voxel_world_id = bvxm_batch.commit_output(0)

python_path = "./"
change_type = "sewage"

import os
import shutil
output_path = python_path + "testing_output_rec/"
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
        bvxm_batch.init_process("LoadRationalCameraProcess")
        bvxm_batch.set_input_string(0, cam_name)
        bvxm_batch.run_process()
        orig_cam_id = bvxm_batch.commit_output(0)
    else:
        bvxm_batch.init_process("LoadRationalCameraNITFProcess")
        bvxm_batch.set_input_string(0, image_filename)
        bvxm_batch.run_process()
        orig_cam_id = bvxm_batch.commit_output(0)

    # get a roi from the image
    bvxm_batch.init_process("bvxmRoiInitProcess")
    bvxm_batch.set_input_string(0, image_filename)
    bvxm_batch.set_input_from_db(1, orig_cam_id)
    bvxm_batch.set_input_from_db(2, voxel_world_id)
    bvxm_batch.set_params_process("./roi_params.xml")
    statuscode = bvxm_batch.run_process()
    print statuscode
    if statuscode:
        cropped_cam_id = bvxm_batch.commit_output(0)
        cropped_image_id = bvxm_batch.commit_output(1)
        uncertainty_id = bvxm_batch.commit_output(2)

        curr_scale = 0

        map_type = "10bins_1d_radial"
        print("Illumination Index")
        bvxm_batch.init_process("bvxmIllumIndexProcess")
        bvxm_batch.set_input_string(0, map_type)
        bvxm_batch.set_input_string(1, image_filename)
        bvxm_batch.set_input_unsigned(2, 8)
        bvxm_batch.set_input_unsigned(3, 0)
        bvxm_batch.run_process()
        bin_id = bvxm_batch.commit_output(0)

        app_type = "apm_mog_grey"

        if normalize_and_save == 1:
            # Normalizing images
            print(" Normalizing Image ")
            bvxm_batch.init_process("bvxmNormalizeImageProcess")
            bvxm_batch.set_params_process("./normalize.xml")
            bvxm_batch.set_input_from_db(0, cropped_image_id)
            bvxm_batch.set_input_from_db(1, cropped_cam_id)
            bvxm_batch.set_input_from_db(2, voxel_world_id)
            bvxm_batch.set_input_string(3, app_type)
            bvxm_batch.set_input_from_db(4, bin_id)
            bvxm_batch.set_input_unsigned(5, curr_scale)
            bvxm_batch.run_process()
            normalized_img_id = bvxm_batch.commit_output(0)
            float1_id = bvxm_batch.commit_output(1)
            float2_id = bvxm_batch.commit_output(2)

            print("Saving Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, normalized_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "normalized" + str(i) + ".png")
            bvxm_batch.run_process()

            curr_image_id = normalized_img_id

            print("Detect Changes")
            bvxm_batch.init_process("bvxmDetectChangesProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_from_db(1, cropped_cam_id)
            bvxm_batch.set_input_from_db(2, voxel_world_id)
            bvxm_batch.set_input_string(3, app_type)
            bvxm_batch.set_input_from_db(4, bin_id)
            bvxm_batch.set_input_unsigned(5, curr_scale)
            bvxm_batch.run_process()
            out_img_id = bvxm_batch.commit_output(0)
            mask_img_id = bvxm_batch.commit_output(1)

            print("Saving Prob Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "density_map" + str(i) + ".tiff")
            bvxm_batch.run_process()

            print("Saving Map Image")
            bvxm_batch.init_process("SaveImageViewBinaryProcess")
            bvxm_batch.set_input_from_db(0, mask_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "density_mask" + str(i) + ".bin")
            bvxm_batch.run_process()
        else:
            print("Loading normalized Image")
            bvxm_batch.init_process("LoadImageViewProcess")
            bvxm_batch.set_input_string(
                0, output_path + "normalized" + str(i) + ".png")
            bvxm_batch.run_process()
            curr_image_id = bvxm_batch.commit_output(0)

            print("Loading Prob Image")
            bvxm_batch.init_process("LoadImageViewProcess")
            bvxm_batch.set_input_string(
                0, output_path + "density_map" + str(i) + ".tiff")
            bvxm_batch.run_process()
            out_img_id = bvxm_batch.commit_output(0)

            print("Loading Map Image")
            bvxm_batch.init_process("LoadImageViewBinaryProcess")
            bvxm_batch.set_input_string(
                0, output_path + "density_mask" + str(i) + ".bin")
            bvxm_batch.run_process()
            mask_img_id = bvxm_batch.commit_output(0)

        print("Convert density to prob map")
        bvxm_batch.init_process("brecDensityToProbMapProcess")
        bvxm_batch.set_params_process("./density_to_prob_map_params.xml")
        bvxm_batch.set_input_from_db(0, out_img_id)
        bvxm_batch.run_process()
        prob_map_id = bvxm_batch.commit_output(0)

        # ground-truth polygons for change areas need to be prepared via tools in bwm_main and saved as binary files
        # each test image need to have a separate parameter xml file that records the test image dimensions after ROI cropping process
        # polygonal regions need to be marked wrt to cropped ROI coordinate
        # frame (i.e. first crop the image, then open the cropped version in
        # bwm_main for ground-truthing)
        print("Run the gt generator process")
        bvxm_batch.init_process("bvglGenerateMaskProcess")
        bvxm_batch.set_params_process(
            "./params_generate_mask_test_img_" + str(i) + ".xml")
        bvxm_batch.set_input_string(
            0, "./gt/test_img_" + str(i) + "_gt_" + change_type + ".bin")
        bvxm_batch.set_input_string(1, change_type)  # change type
        bvxm_batch.run_process()
        gt_byte_id = bvxm_batch.commit_output(0)
        gt_mask_id = bvxm_batch.commit_output(1)

        print("Saving GT Image")
        bvxm_batch.init_process("SaveImageViewProcess")
        bvxm_batch.set_input_from_db(0, gt_byte_id)
        bvxm_batch.set_input_string(
            1, output_path + "test_img_" + str(i) + "_gt.png")
        bvxm_batch.run_process()

        npasses = 1
        sigma = 0.05
        print("Updating change map")
        bvxm_batch.init_process("brecUpdateChangesProcess")
        bvxm_batch.set_input_from_db(0, out_img_id)
        bvxm_batch.set_input_from_db(1, curr_image_id)
        bvxm_batch.set_input_unsigned(2, npasses)
        bvxm_batch.set_input_float(3, sigma)
        bvxm_batch.run_process()
        out_updated_img_id = bvxm_batch.commit_output(0)
        out_updated_byte_img_id = bvxm_batch.commit_output(1)

        print("Saving Image")
        bvxm_batch.init_process("SaveImageViewProcess")
        bvxm_batch.set_input_from_db(0, out_updated_byte_img_id)
        bvxm_batch.set_input_string(1, output_path + "change_map_updated_n" + str(
            npasses) + "_sigma_" + str(sigma) + "_" + str(i) + ".png")
        bvxm_batch.run_process()

        print("Run the ROC process")
        bvxm_batch.init_process("brecProbMapROCProcess")
        bvxm_batch.set_input_from_db(0, out_updated_img_id)
        bvxm_batch.set_input_from_db(1, mask_img_id)
        bvxm_batch.set_input_from_db(2, gt_mask_id)
        bvxm_batch.set_input_string(3, output_path + "out_roc_updated_n_" + str(
            npasses) + "_sigma_" + str(sigma) + "_" + str(i) + ".txt")
        bvxm_batch.run_process()

        # run the process to recognize structures
        if i == 0:
            print("Recognize road-side structures of Type 2")
            bvxm_batch.init_process("brecRecStructureProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_unsigned(1, 2)
            # rotation angle passed as zero, original orientation of detector
            # is used
            bvxm_batch.set_input_float(2, 0.0)
            bvxm_batch.run_process()
            out_road_prob_map_id = bvxm_batch.commit_output(0)
            out_road_img_id = bvxm_batch.commit_output(1)

            print("Saving Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_road_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "road_map_1_" + str(i) + ".png")
            bvxm_batch.run_process()

            print("Recognize Vehicles of Type 0")
            bvxm_batch.init_process("brecRecStructureProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_unsigned(1, 0)
            bvxm_batch.set_input_float(2, 0.0)
            bvxm_batch.run_process()
            out_vehicle_prob_map_id0 = bvxm_batch.commit_output(0)
            out_vehicle_img_id0 = bvxm_batch.commit_output(1)

            print("Saving Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_vehicle_img_id0)
            bvxm_batch.set_input_string(
                1, output_path + "vehicle_map_0_" + str(i) + ".png")
            bvxm_batch.run_process()

            # run the process to recognize structures
            print("Recognize Vehicles of Type 1")
            bvxm_batch.init_process("brecRecStructureProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_unsigned(1, 1)
            bvxm_batch.set_input_float(2, 0.0)
            bvxm_batch.run_process()
            out_vehicle_prob_map_id1 = bvxm_batch.commit_output(0)
            out_vehicle_img_id1 = bvxm_batch.commit_output(1)

            #print("Saving Image");
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_vehicle_img_id1)
            bvxm_batch.set_input_string(
                1, output_path + "vehicle_map_1_" + str(i) + ".png")
            bvxm_batch.run_process()

            bvxm_batch.init_process("MapImageBinaryProcess")
            bvxm_batch.set_input_from_db(0, out_vehicle_prob_map_id0)
            bvxm_batch.set_input_from_db(1, out_vehicle_prob_map_id1)
            bvxm_batch.set_input_string(2, "add")
            bvxm_batch.run_process()
            vehicle_prob_added_map_id = bvxm_batch.commit_output(0)

            bvxm_batch.init_process("MapImageBinaryProcess")
            bvxm_batch.set_input_from_db(0, vehicle_prob_added_map_id)
            bvxm_batch.set_input_from_db(1, out_road_prob_map_id)
            bvxm_batch.set_input_string(2, "add")
            bvxm_batch.run_process()
            rec_prob_map_id = bvxm_batch.commit_output(0)

        else:
            print("Recognize building structure of Type 3")
            bvxm_batch.init_process("brecRecStructureProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_unsigned(1, 3)
            bvxm_batch.set_input_float(2, 0.0)
            bvxm_batch.run_process()
            out_b_prob_map_id = bvxm_batch.commit_output(0)
            out_b_img_id = bvxm_batch.commit_output(1)

            print("Saving Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_b_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "building_map_3_" + str(i) + ".png")
            bvxm_batch.run_process()

            print("Recognize building structure of Type 4")
            bvxm_batch.init_process("brecRecStructureProcess")
            bvxm_batch.set_input_from_db(0, curr_image_id)
            bvxm_batch.set_input_unsigned(1, 4)
            bvxm_batch.set_input_float(2, 0.0)
            bvxm_batch.run_process()
            out_b_prob_map_id1 = bvxm_batch.commit_output(0)
            out_b_img_id1 = bvxm_batch.commit_output(1)

            print("Saving Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, out_b_img_id1)
            bvxm_batch.set_input_string(
                1, output_path + "building_map_4_" + str(i) + ".png")
            bvxm_batch.run_process()

            bvxm_batch.init_process("MapImageBinaryProcess")
            bvxm_batch.set_input_from_db(0, out_b_prob_map_id)
            bvxm_batch.set_input_from_db(1, out_b_prob_map_id1)
            bvxm_batch.set_input_string(2, "add")
            bvxm_batch.run_process()
            rec_prob_map_id = bvxm_batch.commit_output(0)

        bvxm_batch.init_process("brecProbMapSupressProcess")
        bvxm_batch.set_input_from_db(0, out_updated_img_id)
        # supressor map is the recognition output
        bvxm_batch.set_input_from_db(1, rec_prob_map_id)
        bvxm_batch.run_process()
        out_sup_prob_map_id = bvxm_batch.commit_output(0)

        for j in range(j_size):
            print("Threshold prob map")
            bvxm_batch.init_process("brecProbMapThresholdProcess")
            bvxm_batch.set_input_from_db(0, out_sup_prob_map_id)
            bvxm_batch.set_input_from_db(1, mask_img_id)
            bvxm_batch.set_input_from_db(2, curr_image_id)
            bvxm_batch.set_input_float(3, j_arr[j])
            bvxm_batch.run_process()
            thres_img_id = bvxm_batch.commit_output(0)

            print("Saving Thresholded Image")
            bvxm_batch.init_process("SaveImageViewProcess")
            bvxm_batch.set_input_from_db(0, thres_img_id)
            bvxm_batch.set_input_string(
                1, output_path + "thres_after_updated_rec_" + str(i) + "_" + str(j_arr[j]) + ".png")
            bvxm_batch.run_process()

        print("Run the ROC process")
        bvxm_batch.init_process("brecProbMapROCProcess")
        bvxm_batch.set_input_from_db(0, out_sup_prob_map_id)
        bvxm_batch.set_input_from_db(1, mask_img_id)
        bvxm_batch.set_input_from_db(2, gt_mask_id)
        bvxm_batch.set_input_string(
            3, output_path + "out_roc_updated_rec_" + str(i) + ".txt")
        bvxm_batch.run_process()

        print("Calculate Area")
        bvxm_batch.init_process("MapImageProcess")
        bvxm_batch.set_input_from_db(0, out_sup_prob_map_id)
        bvxm_batch.set_input_string(1, "not")
        bvxm_batch.run_process()
        inverse_out_sup_prob_map_id = bvxm_batch.commit_output(0)
        bvxm_batch.init_process("brecChangeAreaProcess")
        bvxm_batch.set_input_from_db(0, inverse_out_sup_prob_map_id)
        bvxm_batch.set_input_from_db(1, mask_img_id)
        bvxm_batch.run_process()
        area_id = bvxm_batch.commit_output(0)
        area_perc_id = bvxm_batch.commit_output(1)
        area = bvxm_batch.get_input_float(area_id)
        area_perc = bvxm_batch.get_input_float(area_perc_id)
        print("\n---------------\n Calculated Area: " + str(area) +
              " perc over all image: " + str(area_perc) + "\n-----------\n")
