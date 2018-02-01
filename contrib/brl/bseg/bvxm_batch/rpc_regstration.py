#############################################################################
# Created by Vishal Jain
# Nov 10, 2009
# LEMS, Brown University
#############################################################################

import brl_init
import bvxm_batch as batch
dbvalue = brl_init.register_batch(batch)

f = open('images.txt', 'r')
image_fnames = f.readlines()
f.close()

print("Creating Voxel World")
batch.init_process("bvxmCreateVoxelWorldProcess")
batch.set_params_process("./bvxmCreateVoxelWorldProcess.xml")
batch.set_input_string(1, "ocp_opinion")
batch.run_process()
(world_id, world_type) = batch.commit_output(0)
world = dbvalue(world_id, world_type)

for i in range(0, len(image_fnames), 1):
    image_fnames[i] = image_fnames[i].strip()

    batch.init_process("vpglLoadRationalCameraNITFProcess")
    batch.set_input_string(0, image_fnames[i])
    batch.run_process()
    (cam_id, cam_type) = batch.commit_output(0)
    cam = dbvalue(cam_id, cam_type)

    batch.init_process("bvxmRoiInitProcess")
    batch.set_input_string(0, image_fnames[i])
    batch.set_input_from_db(1, cam)
    batch.set_input_from_db(2, world)
    batch.set_params_process("roi_params.xml")
    statuscode = batch.run_process()

    print statuscode
    if statuscode:
        (cropped_cam_id, cropped_cam_type) = batch.commit_output(0)
        cropped_cam = dbvalue(cropped_cam_id, cropped_cam_type)

        (cropped_image_id, cropped_image_type) = batch.commit_output(1)
        cropped_image = dbvalue(cropped_image_id, cropped_image_type)

        (uncertainty_id, uncertainty_type) = batch.commit_output(2)
        uncertainty = dbvalue(uncertainty_id, uncertainty_type)

        print("Compass edge detector  Image")
        batch.init_process("bilCompassEdgeDetectorProcess")
        batch.set_input_from_db(0, cropped_image)
        batch.set_input_unsigned(1, 8)
        batch.set_input_double(2, 2.0)
        batch.set_input_double(3, 0.4)
        batch.run_process()
        (cropped_edge_image_id, cropped_edge_image_type) = batch.commit_output(0)
        cropped_edge_image = dbvalue(
            cropped_edge_image_id, cropped_edge_image_type)

        batch.init_process("bvxmRpcRegistrationProcess")
        batch.set_input_from_db(0, world)
        batch.set_input_from_db(1, cropped_cam)
        batch.set_input_from_db(2, cropped_edge_image)
        batch.set_input_bool(3, 0)
        batch.set_input_float(4, 25)
        batch.set_input_float(5, 0)
        batch.set_input_unsigned(6, 0)
        batch.run_process()
        (id, type) = batch.commit_output(0)
        corrected_cam = dbvalue(id, type)
        (id, type) = batch.commit_output(1)
        corrected_img = dbvalue(id, type)
        cam_name_local = "./camera" + str(i) + ".txt"

        print(" saving camera  ")
        batch.init_process("vpglSaveRationalCameraProcess")
        batch.set_input_from_db(0, corrected_cam)
        batch.set_input_string(1, cam_name_local)
        batch.run_process()

        print("Saving Image")
        batch.init_process("vilSaveImageViewProcess")
        batch.set_input_from_db(0, corrected_img)
        batch.set_input_string(1, "./edge" + str(i) + ".png")
        batch.run_process()

        print("Saving Image")
        batch.init_process("vilSaveImageViewProcess")
        batch.set_input_from_db(0, cropped_edge_image)
        batch.set_input_string(1, "./compassedge" + str(i) + ".png")
        batch.run_process()
