import boxm2_batch;

boxm2_batch.register_processes();
boxm2_batch.register_datatypes();

class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

print "Loading a Scene";
boxm2_batch.init_process("boxm2LoadSceneProcess");
boxm2_batch.set_input_string(0,"f:/visdt/boxm2_empty/scene.xml");
boxm2_batch.run_process();
(scene_id, scene_type) = boxm2_batch.commit_output(0);
scene = dbvalue(scene_id, scene_type);

print scene;
