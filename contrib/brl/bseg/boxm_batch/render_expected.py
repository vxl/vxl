import boxm_batch;
boxm_batch.register_processes();
boxm_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

print("Loading Scene");
boxm_batch.init_process("boxmLoadSceneProcess");
boxm_batch.set_input_string(0,"C:\\test_images\\octree\\capitol\\scene.xml");
boxm_batch.set_input_string(1,"apm_mog_grey");
boxm_batch.run_process();
(scene_id, scene_type) = boxm_batch.commit_output(0);
scene = dbvalue(scene_id, scene_type);

print("Loading camera");
boxm_batch.init_process("vpglLoadPerspectiveCameraProcess");
boxm_batch.set_input_string(0,"camera_00116.txt");
boxm_batch.run_process();
(cam_id,cam_type)=boxm_batch.commit_output(0);
camera=dbvalue(cam_id, cam_type);

print("Rendering Image");
boxm_batch.init_process("boxmRenderExpectedProcess");
boxm_batch.set_input_from_db(0,scene);
boxm_batch.set_input_from_db(1,camera);
boxm_batch.set_input_unsigned(2,1280);
boxm_batch.set_input_unsigned(3,720);
boxm_batch.run_process();
(img_id,img_type)=boxm_batch.commit_output(0);
(mask_id,mask_type)=boxm_batch.commit_output(1);
img = dbvalue(img_id, img_type);
mask = dbvalue(mask_id, mask_type);

boxm_batch.init_process("vilSaveImageViewProcess");
boxm_batch.set_input_from_db(0,img);
boxm_batch.set_input_string(1,"image.jpg");
boxm_batch.run_process();

boxm_batch.init_process("vilSaveImageViewProcess");
boxm_batch.set_input_from_db(0,mask);
boxm_batch.set_input_string(1,"mask.jpg");
boxm_batch.run_process();

