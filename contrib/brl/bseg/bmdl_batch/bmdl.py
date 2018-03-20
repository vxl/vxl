##########################################################################
# Created by Gamze D. Tunali
#            LEMS, Brown University
#            Oct 15, 2007
##########################################################################
import brl_init
import bmdl_batch as batch
dbvalue = brl_init.register_batch(batch)

##########################################################################
# Please replace your paths to the LIDAR images instead of first_return.tif,
#last_return.tif and ground.tif
###########################################################################
print("Labeling the Lidar")
batch.init_process("bmdlClassifyProcess")
batch.set_input_string(0, "./first_ret.tif")
batch.set_input_string(1, "./last_ret.tif")
batch.run_process()
label_img = batch.commit_output(0)
height_img = batch.commit_output(1)

print("Saving Labeled Image")
batch.init_process("SaveImageViewProcess")
batch.set_input_from_db(0, label_img)
batch.set_input_string(1, "./label.tif")
batch.run_process()

print("Tracing Boundaries")
batch.init_process("bmdlTraceBoundariesProcess")
batch.set_input_from_db(0, label_img)
batch.set_input_string(1, "./polygons.bin")
batch.run_process()

print("Loading Ground Image")
batch.init_process("LoadImageViewProcess")
batch.set_input_string(0, "./ground.tif")
batch.run_process()
ground_img = batch.commit_output(0)

print("Generating Mesh")
batch.init_process("bmdlGenerateMeshProcess")
batch.set_input_string(0, "./polygons.bin")
batch.set_input_from_db(1, label_img)
batch.set_input_from_db(2, height_img)
batch.set_input_from_db(3, ground_img)
batch.set_input_string(4, "./meshes.obj")
batch.run_process()
