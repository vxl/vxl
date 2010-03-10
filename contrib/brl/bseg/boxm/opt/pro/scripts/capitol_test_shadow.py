import boxm_batch
boxm_batch.register_processes()
boxm_batch.register_datatypes()

######### Parameters ###########
do_init = 1
do_refine = 1
num_its = 40
refine_prob = 0.2
shadow_prior = 0.1
shadow_sigma = 4.0/255

model_dir = "c:/research/boxm/models/capitol_test_shadow"
image_id_fname = model_dir + "/image_list.txt"
image_fname = "c:/research/data/capitol/video_grey_halfres/frame_%05d.png"
camera_fname = "c:/research/data/capitol/cameras_KRT_halfres/camera_%05d.txt"
camera_idx = range(0,255,8)
output_dir = "c:/research/boxm/output"
expected_fname = output_dir + "/expected_image_capitol_shadow_%03d.tiff"
raw_fname = output_dir + "/capitol_shadow_"
ncells_fname = model_dir + "/ncells.txt"
###############################


class dbvalue:
  def __init__(self, index, type):
    self.id = index
    self.type = type

image_ids = []
# write camera indices to file
fd = open(image_id_fname,"w")
print >>fd, len(camera_idx)
for c in camera_idx:
  img_id = "frame_%d" % c
  image_ids.append(img_id)
  print >>fd, img_id

fd.close()

# load scene 
print("Creating a Scene")
boxm_batch.init_process("boxmCreateSceneProcess")
boxm_batch.set_input_string(0, model_dir + "/scene.xml")
boxm_batch.run_process()
(scene_id, scene_type) = boxm_batch.commit_output(0)
scene = dbvalue(scene_id, scene_type)

if (do_init):
  # clean the scene
  print("Cleaning the Scene")
  boxm_batch.init_process("boxmCleanSceneProcess")
  boxm_batch.set_input_from_db(0,scene)
  boxm_batch.run_process() 
  
  # initialize scene
  print("Initializing Scene")
  boxm_batch.init_process("boxmInitSceneProcess")
  boxm_batch.set_input_from_db(0,scene)
  boxm_batch.run_process()

print("Loading Virtual Camera")
boxm_batch.init_process("vpglLoadPerspectiveCameraProcess")
boxm_batch.set_input_string(0,camera_fname % 45)
boxm_batch.run_process()
(id,type) = boxm_batch.commit_output(0)
vcam = dbvalue(id,type)

for it in range(0,num_its):
  for c in range(0,len(camera_idx)):
    
    print("Loading Camera")
    boxm_batch.init_process("vpglLoadPerspectiveCameraProcess")
    boxm_batch.set_input_string(0,camera_fname % camera_idx[c])
    boxm_batch.run_process()
    (id,type) = boxm_batch.commit_output(0)
    cam = dbvalue(id,type)
    
    print("Loading Image")
    boxm_batch.init_process("vilLoadImageViewProcess")
    boxm_batch.set_input_string(0,image_fname % camera_idx[c])
    boxm_batch.run_process()
    (id,type) = boxm_batch.commit_output(0)
    image = dbvalue(id,type)
    
    print "Generating opt_samples for camera ", camera_idx[c]
    boxm_batch.init_process("boxmGenerateOpt2SamplesProcess")
    boxm_batch.set_input_from_db(0,image)
    boxm_batch.set_input_from_db(1,cam)
    boxm_batch.set_input_from_db(2,scene)
    boxm_batch.set_input_string(3,image_ids[c])
    boxm_batch.set_input_float(4,shadow_prior)
    boxm_batch.set_input_float(5,shadow_sigma)
    boxm_batch.set_input_bool(6,0)
    boxm_batch.run_process()
  
  # Do the optimization
  boxm_batch.init_process("boxmOpt2UpdateProcess")
  boxm_batch.set_input_from_db(0,scene)
  boxm_batch.set_input_string(1,image_id_fname)
  boxm_batch.run_process()
  
  # Generate Expected Image 
  print("Generating Expected Image")
  boxm_batch.init_process("boxmRenderExpectedRTProcess")
  boxm_batch.set_input_from_db(0,scene)
  boxm_batch.set_input_from_db(1,vcam) 
  boxm_batch.set_input_unsigned(2,1280/2)
  boxm_batch.set_input_unsigned(3,720/2)
  boxm_batch.set_input_bool(4,0)
  boxm_batch.run_process()
  (id,type) = boxm_batch.commit_output(0)
  expected = dbvalue(id,type)
  (id,type) = boxm_batch.commit_output(1)
  mask = dbvalue(id,type)
  
  print("saving expected image")
  boxm_batch.init_process("vilSaveImageViewProcess")
  boxm_batch.set_input_from_db(0,expected)
  boxm_batch.set_input_string(1,expected_fname % it)
  boxm_batch.run_process()

  boxm_batch.remove_data(expected.id)
  boxm_batch.remove_data(mask.id)
    
  if ( (do_refine) & (it+1 < num_its) ):    
    print("Refining Scene")
    boxm_batch.init_process("boxmRefineSceneProcess")
    boxm_batch.set_input_from_db(0,scene)
    boxm_batch.set_input_float(1,refine_prob)
    boxm_batch.set_input_bool(2,False)
    boxm_batch.run_process()
    (id,type) = boxm_batch.commit_output(0)
    ncells_db = dbvalue(id,type)
    # get the actual value and print to file
    ncells = boxm_batch.get_input_unsigned(ncells_db.id)
    fd = open(ncells_fname,"a")
    print >>fd, ncells
    fd.close()
  
  boxm_batch.remove_data(cam.id)
  boxm_batch.remove_data(image.id)
  
  print("Save Scene")
  boxm_batch.init_process("boxmSaveOccupancyRawProcess")
  boxm_batch.set_input_from_db(0,scene)
  boxm_batch.set_input_string(1,raw_fname)
  boxm_batch.set_input_unsigned(2,0)
  boxm_batch.set_input_unsigned(3,0)
  boxm_batch.run_process()

boxm_batch.remove_data(scene.id)

# switch processes to make sure scene destructor is called
boxm_batch.init_process("vilLoadImageViewProcess")

print("Done.")
