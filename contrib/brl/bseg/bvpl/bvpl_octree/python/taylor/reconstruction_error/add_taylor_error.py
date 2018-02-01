# -*- coding: utf-8 -*-
"""
Created on Mon Feb 14, 2011

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


class taylor_error_job():

    def __init__(self, error_scene, fraction, block_i, block_j, block_k):
        self.error_scene = error_scene
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
        worker = taylor_error_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")

       # collect the results off the queue
    results = []
    while len(results) < len(jobs):
        result = result_queue.get()
        results.append(result)

    return results


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

            print("Adding Errors")
            batch.init_process("bvplAddTaylorErrorsProcess")
            batch.set_input_from_db(0, job.error_scene)
            batch.set_input_double(1, job.fraction)
            batch.set_input_int(2, job.block_i)
            batch.set_input_int(3, job.block_j)
            batch.set_input_int(4, job.block_k)
            batch.run_process()
            (id, type) = batch.commit_output(0)
            error_val = dbvalue(id, type)
            error = batch.get_output_double(id)

            self.result_queue.put(error)

            print("error")
            print(error)

            print("Runing time for worker:", self.name)
            print(time.time() - start_time)


#***************** The Main Algorithm ************************#

if __name__ == "__main__":

    batch.register_processes()
    batch.register_datatypes()

    # Parse inputs
    parser = optparse.OptionParser(description='Add Taylor Errors in a Scene')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--taylor_dir', action="store", dest="taylor_dir")
    parser.add_option('--fraction', action="store",
                      dest="fraction", type="float", default=0.0)
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)
    parser.add_option('--nblocks_x', action="store",
                      dest="nblocks_x", type="int", default=1)
    parser.add_option('--nblocks_y', action="store",
                      dest="nblocks_y", type="int", default=1)
    parser.add_option('--nblocks_z', action="store",
                      dest="nblocks_z", type="int", default=1)

    options, args = parser.parse_args()

    model_dir = options.model_dir
    taylor_dir = options.taylor_dir
    nblocks_x = options.nblocks_x
    nblocks_y = options.nblocks_y
    nblocks_z = options.nblocks_z
    num_cores = options.num_cores
    fraction = options.fraction

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Model Dir"
        sys.exit(-1)

    if not os.path.isdir(taylor_dir + "/"):
        print "Invalid Taylor Dir"
        sys.exit(-1)

    print("Loading Error Scene")
    batch.init_process("boxmCreateSceneProcess")
    batch.set_input_string(0,  taylor_dir + "/error_scene.xml")
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    error_scene = dbvalue(scene_id, scene_type)

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
                    error_scene, fraction, block_i, block_j, block_k)
                job_list.append(current_job)

    # run
    results = execute_jobs(job_list, num_cores)

    # dump results
    total_error = 0
    for r in results:
        print(r)
        total_error = total_error + r

    # write schedule file
    int_frac = int(fraction * 100)
    error_file = taylor_dir + "/error_" + str(int_frac) + ".txt"
    fd = open(error_file, "w")
    print >>fd, total_error / len(results)
    print >>fd, results
    fd.close()
