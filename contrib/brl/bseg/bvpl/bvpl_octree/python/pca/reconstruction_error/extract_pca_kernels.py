# Computes the gaussian gradients on a boxm_alpha_scene

import bvpl_octree_batch;
import os;
import optparse;
import time;
class dbvalue:
  def __init__(self, index, type):
    self.id = index;   # unsigned integer
    self.type = type;  # string


if __name__=="__main__":

  bvpl_octree_batch.register_processes();
  bvpl_octree_batch.register_datatypes();

  #Parse inputs
  parser = optparse.OptionParser(description='Compute PCA basis');

  parser.add_option('--model_dir', action="store", dest="model_dir");
  parser.add_option('--pca_dir', action="store", dest="pca_dir");
  parser.add_option('--train_fraction', action="store", dest="train_fraction", type="float", default =0.0);
  options, args = parser.parse_args();

  model_dir = options.model_dir;
  pca_dir = options.pca_dir;
  train_fraction = options.train_fraction;

  if not os.path.isdir(model_dir +"/"):
      print "Invalid Model Dir";
      sys.exit(-1);

  if not os.path.isdir( pca_dir + "/"):
      os.makedirs( pca_dir + "/");


  print("Extracting Principal Components patches");

  print("Creating a Scene");
  bvpl_octree_batch.init_process("boxmCreateSceneProcess");
  bvpl_octree_batch.set_input_string(0,  model_dir +"/mean_color_scene.xml");
  bvpl_octree_batch.run_process();
  (scene_id, scene_type) = bvpl_octree_batch.commit_output(0);
  scene= dbvalue(scene_id, scene_type);

  start_time = time.time();

  print("Extract PC");
  bvpl_octree_batch.init_process("bvplDiscoverPCAFeaturesProcess");
  bvpl_octree_batch.set_input_from_db(0, scene);
  bvpl_octree_batch.set_input_string(1,  pca_dir);
  bvpl_octree_batch.set_input_double(2,  train_fraction);
  bvpl_octree_batch.set_input_int(3, -2); #min and max points of the kernel
  bvpl_octree_batch.set_input_int(4, -2);
  bvpl_octree_batch.set_input_int(5, -2);
  bvpl_octree_batch.set_input_int(6, 2);
  bvpl_octree_batch.set_input_int(7, 2);
  bvpl_octree_batch.set_input_int(8, 2);
  bvpl_octree_batch.run_process();

  print("Runing time forbvplDiscoverPCAFeaturesProcess:");
  print(time.time() - start_time);

