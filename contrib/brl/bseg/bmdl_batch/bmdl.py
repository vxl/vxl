##########################################################################
# Created by Gamze D. Tunali
#            LEMS, Brown University
#            Oct 15, 2007
##########################################################################
import bmdl_batch
bmdl_batch.register_processes();
bmdl_batch.register_datatypes();

##########################################################################
#Please replace your paths to the LIDAR images instead of first_return.tif, 
#last_return.tif and ground.tif
###########################################################################
print("Labeling the Lidar");
bmdl_batch.init_process("bmdlClassifyProcess");
bmdl_batch.set_input_string(0,"./first_ret.tif");
bmdl_batch.set_input_string(1,"./last_ret.tif");
bmdl_batch.run_process();
label_img = bmdl_batch.commit_output(0);
height_img = bmdl_batch.commit_output(1);

print("Saving Labeled Image");
bmdl_batch.init_process("SaveImageViewProcess");
bmdl_batch.set_input_from_db(0, label_img);
bmdl_batch.set_input_string(1,"./label.tif");
bmdl_batch.run_process();

print("Tracing Boundaries");
bmdl_batch.init_process("bmdlTraceBoundariesProcess");
bmdl_batch.set_input_from_db(0, label_img);
bmdl_batch.set_input_string(1, "./polygons.bin");
bmdl_batch.run_process();

print("Loading Ground Image");
bmdl_batch.init_process("LoadImageViewProcess");
bmdl_batch.set_input_string(0,"./ground.tif");
bmdl_batch.run_process();
ground_img = bmdl_batch.commit_output(0);

print("Generating Mesh");
bmdl_batch.init_process("bmdlGenerateMeshProcess");
bmdl_batch.set_input_string(0, "./polygons.bin");
bmdl_batch.set_input_from_db(1, label_img);
bmdl_batch.set_input_from_db(2, height_img);
bmdl_batch.set_input_from_db(3, ground_img);
bmdl_batch.set_input_string(4, "./meshes.obj");
bmdl_batch.run_process();



