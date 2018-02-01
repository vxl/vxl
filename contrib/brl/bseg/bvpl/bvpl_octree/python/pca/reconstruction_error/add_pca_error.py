# -*- coding: utf-8 -*-
"""
Created on Mon Feb 21, 2011

@author:Isabel Restrepo

Adds the error of a portion of voxels. Each block is processed in a separate thread.
This script assumes that the reconstruction error at each voxel has been computed
"""
import os
import multiprocessing
import Queue
import time
import random
import optparse

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)


class pca_error_job():

    def __init__(self, pca_scenes, dim, fraction, block_i, block_j, block_k):
        self.pca_scenes = pca_scenes
        self.dim = dim
        self.fraction = fraction
        self.block_i = block_i
        self.block_j = block_j
        self.block_k = block_k


def execute_jobs(jobs, num_procs=4):
    # load up work queue
    work_queue = multiprocessing.Queue()
    for job in jobs:
        work_queue.put(job)

    # create a queue to pass to workers to store the results
    result_queue = multiprocessing.Queue()

    # spawn workers
    for i in range(num_procs):
        worker = pca_error_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")

       # collect the results off the queue
    # all_results=[];
#    results = []
#    while len(results) < len(jobs):
#        result = result_queue.get()
#        results.append(result)
#        all_results[job.dim].append(result);
#
#    return all_results;

    # collect the results off the queue
    results = []
    while len(results) < len(jobs):
        result = result_queue.get()
        results.append(result)

    return results


class pca_error_worker(multiprocessing.Process):

    def __init__(self, work_queue, result_queue):
        # base class initialization
        multiprocessing.Process.__init__(self)
        # job management stuff
        self.work_queue = work_queue
        self.result_queue = result_queue
        self.kill_received = False

    def run(self):
        while not self.kill_received:
           # get a task
            try:
                job = self.work_queue.get_nowait()
            except Queue.Empty:
                break

            start_time = time.time()

            print("Adding Errors")
            batch.init_process("bvplAddPCAErrorsProcess")
            batch.set_input_from_db(0, job.pca_scenes)
            batch.set_input_unsigned(1, job.dim)
            batch.set_input_double(2, job.fraction)
            batch.set_input_int(3, job.block_i)
            batch.set_input_int(4, job.block_j)
            batch.set_input_int(5, job.block_k)
            batch.run_process()
            (id, type) = batch.commit_output(0)
            error_val = dbvalue(id, type)
            error = batch.get_output_double(id)

            self.result_queue.put(error)

            print("Runing time for worker:", self.name)
            print(time.time() - start_time)


#***************** The Main Algorithm ************************#

if __name__ == "__main__":

    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Compute PCA Error Scene')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--pca_dir', action="store", dest="pca_dir")
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)
    parser.add_option('--fraction', action="store",
                      dest="fraction", type="float", default=0.0)
    parser.add_option('--nblocks_x', action="store",
                      dest="nblocks_x", type="int")
    parser.add_option('--nblocks_y', action="store",
                      dest="nblocks_y", type="int")
    parser.add_option('--nblocks_z', action="store",
                      dest="nblocks_z", type="int")
    parser.add_option('--dimension', action="store",
                      dest="dimension", type="int")

    options, args = parser.parse_args()

    model_dir = options.model_dir
    pca_dir = options.pca_dir
    fraction = options.fraction
    nblocks_x = options.nblocks_x
    nblocks_y = options.nblocks_y
    nblocks_z = options.nblocks_z
    num_cores = options.num_cores
    dimension = options.dimension

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Model Dir"
        sys.exit(-1)

    if not os.path.isdir(pca_dir + "/"):
        print "Invalid PCA Dir"
        sys.exit(-1)

    print("Loading Data Scene")
    batch.init_process("boxmCreateSceneProcess")
    batch.set_input_string(0,  model_dir + "/mean_color_scene.xml")
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    data_scene = dbvalue(scene_id, scene_type)

    # Load PCA scenes
    pca_feature_dim = 125
    print("Loading PCA Error Scenes")
    batch.init_process("bvplLoadPCAErrorSceneProcess")
    batch.set_input_from_db(0, data_scene)
    batch.set_input_string(1, pca_dir)
    batch.set_input_unsigned(
        2, pca_feature_dim)  # dimension pca feature
    batch.run_process()
    (id, type) = batch.commit_output(0)
    pca_scenes = dbvalue(id, type)

    # Begin multiprocessing
    # Enqueue jobs
    job_list = []
    # for dim in range(0, pca_feature_dim):
    for block_i in range(0, nblocks_x):
        for block_j in range(0, nblocks_y):
            for block_k in range(0, nblocks_z):
                current_job = pca_error_job(
                    pca_scenes, dimension, fraction, block_i, block_j, block_k)
                job_list.append(current_job)

    # run
    results = execute_jobs(job_list, num_cores)

    # dump results
    total_error = 0
    for r in results:
        total_error = total_error + r

    int_frac = int(fraction * 100)
    error_file = pca_dir + "/error_" + str(dimension) + "/error.txt"
    fd = open(error_file, "w")
    print >>fd, total_error / len(results)
    print >>fd, results
    fd.close()
