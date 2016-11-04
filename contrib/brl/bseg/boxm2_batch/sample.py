import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

print "Loading a Scene"
batch.init_process("boxm2LoadSceneProcess")
batch.set_input_string(0, "f:/visdt/boxm2_empty/scene.xml")
batch.run_process()
(scene_id, scene_type) = batch.commit_output(0)
scene = dbvalue(scene_id, scene_type)

print scene
