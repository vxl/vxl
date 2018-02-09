# -*- coding: utf-8 -*-
"""
Created on Mon Feb 14, 2011

@author:Isabel Restrepo

Compuets PCA reconstruction error. Each block is processed in a separate thread.
This script assumes that the pca basis has been computed as gone by extract_pca_kernels.py
"""
import os
import multiprocessing
import Queue
import time
import random
import optparse
import sys

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)


class pca_error_job():

    def __init__(self, pca_info, pca_error_scenes, block_i, block_j, block_k, dim):
        self.pca_info = pca_info
        self.pca_error_scenes = pca_error_scenes
        self.block_i = block_i
        self.block_j = block_j
        self.block_k = block_k
        self.dim = dim


def execute_jobs(jobs, num_procs=4):
    work_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()
    for job in jobs:
        work_queue.put(job)

    for i in range(num_procs):
        worker = pca_error_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")


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

            print("Computing Error Scene")
            batch.init_process("bvplComputePCAErrorBlockProcess")
            batch.set_input_from_db(0, job.pca_info)
            batch.set_input_from_db(1, job.pca_error_scenes)
            batch.set_input_int(2, job.block_i)
            batch.set_input_int(3, job.block_j)
            batch.set_input_int(4, job.block_k)
            batch.set_input_unsigned(5, job.dim)
            batch.run_process()

            print("Runing time for worker:", self.name)
            print(time.time() - start_time)

#*******************The Main Algorithm ************************#
if __name__ == "__main__":
    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Compute PCA Error Scene')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--pca_dir', action="store", dest="pca_dir")
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)
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

    # Load pca scenes
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

    print("Loading PCA Info")
    batch.init_process("bvplLoadPCAInfoProcess")
    batch.set_input_string(0, pca_dir)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    pca_info = dbvalue(id, type)

    # Begin multiprocessing
    work_queue = multiprocessing.Queue()
    job_list = []

    # Enqueue jobs
    all_indices = []
    for block_i in range(0, nblocks_x):
        for block_j in range(0, nblocks_y):
            for block_k in range(0, nblocks_z):
                idx = [block_i, block_j, block_k]
                all_indices.append(idx)

    random.shuffle(all_indices)

    for i in range(0, len(all_indices)):
        idx = all_indices[i]
        current_job = pca_error_job(pca_info, pca_scenes, idx[
            0], idx[1], idx[2], dimension)
        job_list.append(current_job)

    execute_jobs(job_list, num_cores)
