# -*- coding: utf-8 -*-
"""
Created on Mon Feb 14, 2011

@author:Isabel Restrepo

Compuets Taylor reconstruction error. Each block is processed in a separate thread.
This script assumes that the taylor kernels have been run on the data scene as done in run_taylor_kernels.py
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


class taylor_error_job():

    def __init__(self, data_scene, taylor_scenes, block_i, block_j, block_k):
        self.data_scene = data_scene
        self.taylor_scenes = taylor_scenes
        self.block_i = block_i
        self.block_j = block_j
        self.block_k = block_k


def execute_jobs(jobs, num_procs=4):
    work_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()
    for job in jobs:
        work_queue.put(job)

    for i in range(num_procs):
        worker = taylor_error_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")


class taylor_error_worker(multiprocessing.Process):

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
            batch.init_process("bvplComputeTaylorErrorProcess")
            batch.set_input_from_db(0, job.data_scene)
            batch.set_input_from_db(1, job.taylor_scenes)
            batch.set_input_int(2, job.block_i)
            batch.set_input_int(3, job.block_j)
            batch.set_input_int(4, job.block_k)
            batch.run_process()

            print("Runing time for worker:", self.name)
            print(time.time() - start_time)

#*******************The Main Algorithm ************************#
if __name__ == "__main__":
    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Compute Taylor Error Scene')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--taylor_dir', action="store", dest="taylor_dir")
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)
    parser.add_option('--nblocks_x', action="store",
                      dest="nblocks_x", type="int")
    parser.add_option('--nblocks_y', action="store",
                      dest="nblocks_y", type="int")
    parser.add_option('--nblocks_z', action="store",
                      dest="nblocks_z", type="int")

    options, args = parser.parse_args()

    model_dir = options.model_dir
    taylor_dir = options.taylor_dir
    nblocks_x = options.nblocks_x
    nblocks_y = options.nblocks_y
    nblocks_z = options.nblocks_z
    num_cores = options.num_cores

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Model Dir"
        sys.exit(-1)

    if not os.path.isdir(taylor_dir + "/"):
        print "Invalid Taylor Dir"
        sys.exit(-1)

    print("Loading Data Scene")
    batch.init_process("boxmCreateSceneProcess")
    batch.set_input_string(0,  model_dir + "/mean_color_scene.xml")
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    data_scene = dbvalue(scene_id, scene_type)

    # Load taylor scenes
    print("Loading Taylor Scenes")
    batch.init_process("bvplLoadTaylorScenesProcess")
    batch.set_input_string(0, taylor_dir)
    batch.set_input_int(1, -2)  # min and max points of the kernel
    batch.set_input_int(2, -2)
    batch.set_input_int(3, -2)
    batch.set_input_int(4, 2)
    batch.set_input_int(5, 2)
    batch.set_input_int(6, 2)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    taylor_scenes = dbvalue(id, type)

    # Begin multiprocessing
    work_queue = multiprocessing.Queue()
    job_list = []

    blocks_x = [i for i in range(0, nblocks_x)]
    blocks_y = [i for i in range(0, nblocks_y)]
    blocks_z = [i for i in range(0, nblocks_z)]

    random.shuffle(blocks_x)
    random.shuffle(blocks_y)
    random.shuffle(blocks_y)

    # Enqueue jobs
    for i in range(0, len(blocks_x)):
        for j in range(0, len(blocks_y)):
            for k in range(0, len(blocks_z)):
                block_i = blocks_x[i]
                block_j = blocks_y[j]
                block_k = blocks_z[k]
                current_job = taylor_error_job(
                    data_scene, taylor_scenes, block_i, block_j, block_k)
                job_list.append(current_job)

    execute_jobs(job_list, num_cores)
