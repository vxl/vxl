# Computes normalized pca training error

import os
import optparse
import time
import sys

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)

if __name__ == "__main__":

    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Compute PCA basis')

    parser.add_option('--pca_dir', action="store", dest="pca_dir")
    parser.add_option('--train_fraction', action="store",
                      dest="train_fraction", type="float", default=0.0)
    options, args = parser.parse_args()

    pca_dir = options.pca_dir
    train_fraction = (options.train_fraction) / 10

    if not os.path.isdir(pca_dir + "/"):
        os.makedirs(pca_dir + "/")

    pca_dir = pca_dir + "/" + str(int(train_fraction * 100))
    if not os.path.isdir(pca_dir + "/"):
        os.makedirs(pca_dir + "/")

    start_time = time.time()

    batch.init_process("bvplNormalizePCATrainingErrorProcess")
    batch.set_input_string(0,  pca_dir)
    batch.run_process()

    print ("Runing time for bvplNormalizePCATrainingErrorProcess:")
    print(time.time() - start_time)
