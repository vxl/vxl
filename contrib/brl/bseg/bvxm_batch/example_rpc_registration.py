import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

num_train = 5
skip_list = [9, 15, 16, 24, 30]

print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_params_process("./bvxmCreateVoxelWorldProcess.xml")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
world = dbvalue(world_id, world_type)

# updating with LIDAR
print("Creating Lidar")
batch.init_process("bvxmLidarInitProcess")
batch.set_params_process("lidar_params.xml")
batch.set_input_string(
    0, "C:/test_images/BaghdadLIDAR/dem_1m_a1_baghdad_tile39.tif")
batch.set_input_string(
    1, "C:/test_images/BaghdadLIDAR/dem_1m_a2_baghdad_tile39.tif")
batch.set_input_from_db(2, world)
batch.run_process()
(cam_id, cam_type) = batch.commit_output(0)
cam = dbvalue(cam_id, cam_type)
(lidar_id, lidar_type) = batch.commit_output(1)
lidar = dbvalue(lidar_id, lidar_type)

print("Updating World")
batch.init_process("bvxmUpdateLidarProcess")
batch.set_input_from_db(0, lidar)
batch.set_input_from_db(1, cam)
batch.set_input_from_db(2, world)
batch.set_input_unsigned(3, 0)
batch.run_process()
##########################

f = open('./images.txt', 'r')
image_fnames = f.readlines()
f.close()
f = open('./cameras.txt', 'r')
cam_fnames = f.readlines()
f.close()

n_normal = 0

for i in range(0, len(image_fnames), 1):
    image_fnames[i] = image_fnames[i].strip()
    cam_fnames[i] = cam_fnames[i].strip()

    keep_moving = 0
    for j in range(0, len(skip_list), 1):
        if i == skip_list[j]:
            keep_moving = 1

    if keep_moving == 1:
        continue

    str_pad = ""
    if i < 10:
        str_pad = "0"

    image_filename = image_fnames[i]
    cam_name = cam_fnames[i]
    print i
    print '---------------'
    if i < num_train:
        batch.init_process("vpglLoadRationalCameraProcess")
        batch.set_input_string(0, cam_name)
        batch.run_process()
        (cam_id, cam_type) = batch.commit_output(0)

    else:
        batch.init_process("vpglLoadRationalCameraNITFProcess")
        batch.set_input_string(0, image_filename)
        batch.run_process()
        (cam_id, cam_type) = batch.commit_output(0)

    cam = dbvalue(cam_id, cam_type)
    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_filename)
    batch.set_input_from_db(1, cam)
    batch.set_input_from_db(2, world)
    batch.set_params_process("bvxmRoiInitProcess.xml")
    statuscode = batch.run_process()

    print statuscode
    if statuscode:
        (cropped_cam_id, cropped_cam_type) = batch.commit_output(0)
        cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type)

        (cropped_image_id, cropped_image_type) = batch.commit_output(1)
        cropped_image = dbvalue(cropped_image_id, cropped_image_type)

        (uncertainty_id, uncertainty_type) = batch.commit_output(2)
        uncertainty = dbvalue(uncertainty_id, uncertainty_type)

        batch.init_process("vilSaveImageViewProcess")
        batch.set_input_from_db(0, cropped_image)
        batch.set_input_string(
            1, "output_cropped_image_" + str_pad + str(i) + ".jpg")
        batch.run_process()

        batch.init_process("bvxmDetectEdgesProcess")
        batch.set_input_from_db(0, cropped_image)
        batch.set_params_process("./bvxmDetectEdgesProcess.xml")
        batch.run_process()
        (cropped_edge_image_id, cropped_edge_image) = batch.commit_output(0)
        cropped_edge_image = dbvalue(cropped_edge_image_id, cropped_edge_image)

        batch.init_process("vilSaveImageViewProcess")
        batch.set_input_from_db(0, cropped_edge_image)
        batch.set_input_string(
            1, "output_cropped_edge_image_" + str_pad + str(i) + ".jpg")
        batch.run_process()

        if i < num_train:
            batch.init_process("bvxmUpdateEdgesProcess")
            batch.set_input_from_db(0, world)
            batch.set_input_from_db(1, cropped_cam)
            batch.set_input_from_db(2, cropped_edge_image)
            batch.set_input_unsigned(3, 0)
            batch.set_params_process("./bvxmUpdateEdgesProcess.xml")
            batch.run_process()
        else:
            batch.init_process("bvxmRpcRegistrationProcess")
            batch.set_input_from_db(0, world)
            batch.set_input_from_db(1, cropped_cam)
            batch.set_input_from_db(2, cropped_edge_image)
            batch.set_input_bool(3, 0)
            batch.set_input_from_db(4, uncertainty)
            batch.set_input_unsigned(5, 0)
            batch.run_process()
            (cam_id, cam_type) = batch.commit_output(0)
            cam = dbvalue(cam_id, cam_type)
            (expected_edge_image_id,
             expected_edge_image_type) = batch.commit_output(1)
            expected_edge_image = dbvalue(
                expected_edge_image_id, expected_edge_image_type)

            batch.init_process("vilSaveImageViewProcess")
            batch.set_input_from_db(0, expected_edge_image)
            batch.set_input_string(
                1, "output_expected_edge_image_after_" + str_pad + str(i) + ".jpg")
            batch.run_process()

            map_type = "10bins_1d_radial"
            print("Illumination Index")
            batch.init_process("bvxmIllumIndexProcess")
            batch.set_input_string(0, map_type)
            batch.set_input_string(1, image_filename)
            batch.set_input_unsigned(2, 8)
            batch.set_input_unsigned(3, 0)
            batch.run_process()
            (bin_id, bin_type) = batch.commit_output(0)
            bin = dbvalue(bin_id, bin_type)
            print bin_id

            # only to get (ni,nj)
            batch.init_process("vilLoadImageViewProcess")
            batch.set_input_string(
                0, "output_cropped_image_" + str_pad + str(i) + ".jpg")
            batch.run_process()
            (ni_id, type) = batch.commit_output(1)
            (nj_id, type) = batch.commit_output(2)
            ni = batch.get_input_unsigned(ni_id)
            nj = batch.get_input_unsigned(nj_id)
            print ni, nj

            app_type = "apm_mog_grey"
            batch.init_process("bvxmCreateMOGImageProcess")
            batch.set_params_process("./create_mog.xml")
            batch.set_input_from_db(0, world)
            batch.set_input_string(1, app_type)
            batch.set_input_from_db(2, bin)
            batch.set_input_unsigned(3, 0)
            batch.set_input_from_db(4, cam)
            batch.set_input_unsigned(5, ni)
            batch.set_input_unsigned(6, nj)
            batch.run_process()
            (voxel_slab_id, voxel_slab_type) = batch.commit_output(0)
            voxel_slab = dbvalue(voxel_slab_id, voxel_slab_type)

            # Normalizing images
            print(" Normalizing Image ")
            batch.init_process("bvxmNormalizeImageProcess")
            batch.set_params_process("./normalize.xml")
            batch.set_input_from_db(0, cropped_image)
            batch.set_input_from_db(1, voxel_slab)
            batch.set_input_string(2, app_type)
            batch.run_process()
            (normalized_img_id, normalized_img_type) = batch.commit_output(0)
            normalized_img = dbvalue(normalized_img_id, normalized_img_type)
            (float1_id, float1_type) = batch.commit_output(1)
            (float2_id, float2_type) = batch.commit_output(2)

            print("Saving Image")
            batch.init_process("vilSaveImageViewProcess")
            batch.set_input_from_db(0, normalized_img)
            batch.set_input_string(
                1, "./normalized/normalized" + str(i) + ".png")
            batch.run_process()

            print("Updating World")
            batch.init_process("bvxmUpdateProcess")
            batch.set_input_from_db(0, normalized_img)
            batch.set_input_from_db(1, cam)
            batch.set_input_from_db(2, world)
            batch.set_input_string(3, app_type)
            batch.set_input_from_db(4, bin)
            batch.set_input_unsigned(5, 0)
            batch.run_process()
            (out_img_id, type) = batch.commit_output(0)
            (mask_img_id, type) = batch.commit_output(1)

            print("Writing World")
            batch.init_process("bvxmSaveOccupancyRawProcess")
            batch.set_input_from_db(0, world)
            batch.set_input_string(1, "./world" + str(i) + ".raw")
            batch.set_input_unsigned(2, 0)
            batch.run_process()

# test the offset differences
f = open('./base_offsets.txt', 'r')
base_lines = f.readlines()
f.close()

f = open('./normalization_parameters.txt', 'r')
lines = f.readlines()
f.close()

for i in range(0, len(base_lines), 1):
    keep_moving = 0
    for j in range(0, len(skip_list), 1):
        if i == skip_list[j]:
            keep_moving = 1

    if keep_moving == 1:
        continue

    line1 = base_lines[i]
    base_offsets = map(float, line1.split())
    # print base_offsets;
    line2 = lines[i]
    offsets = map(float, line2.split())
    # print base_offsets;
    tx = offsets[0] - base_offsets[0]
    ty = offsets[1] - base_offsets[1]
    print tx, ty
