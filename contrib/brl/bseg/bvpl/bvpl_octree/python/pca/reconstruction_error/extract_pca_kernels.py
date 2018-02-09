# Computes the gaussian gradients on a boxm_alpha_scene

import os
import optparse
import time

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)

if __name__ == "__main__":

    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Compute PCA basis')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--pca_dir', action="store", dest="pca_dir")
    parser.add_option('--train_fraction', action="store",
                      dest="train_fraction", type="float", default=0.0)
    options, args = parser.parse_args()

    model_dir = options.model_dir
    pca_dir = options.pca_dir
    train_fraction = options.train_fraction

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Model Dir"
        sys.exit(-1)

    if not os.path.isdir(pca_dir + "/"):
        os.makedirs(pca_dir + "/")

    print("Extracting Principal Components patches")

    print("Creating a Scene")
    batch.init_process("boxmCreateSceneProcess")
    batch.set_input_string(0,  model_dir + "/mean_color_scene.xml")
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)

    start_time = time.time()

    print("Extract PC")
    batch.init_process("bvplDiscoverPCAFeaturesProcess")
    batch.set_input_from_db(0, scene)
    batch.set_input_string(1,  pca_dir)
    batch.set_input_double(2,  train_fraction)
    batch.set_input_int(3, -2)  # min and max points of the kernel
    batch.set_input_int(4, -2)
    batch.set_input_int(5, -2)
    batch.set_input_int(6, 2)
    batch.set_input_int(7, 2)
    batch.set_input_int(8, 2)
    batch.run_process()

    print("Runing time forbvplDiscoverPCAFeaturesProcess:")
    print(time.time() - start_time)
