import multiprocessing
import Queue
import time

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)


class taylor_kernel_job():

    def __init__(self, scene, kernel_path, block_i, block_j, block_k, output_path):
        self.scene = scene
        self.kernel_path = kernel_path
        self.block_i = block_i
        self.block_j = block_j
        self.block_k = block_k
        self.output_path = output_path


def execute_jobs(jobs, num_procs=5):
    work_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()
    for job in jobs:
        work_queue.put(job)

    for i in range(num_procs):
        worker = taylor_kernel_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")


class taylor_kernel_worker(multiprocessing.Process):

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

            print("Creating taylor kernel")
            batch.init_process("bvplLoadTaylorKernelProcess")
            batch.set_input_string(0, job.kernel_path)
            batch.run_process()
            (kernel_id, kernel_type) = batch.commit_output(0)
            kernel = dbvalue(kernel_id, kernel_type)

            print("Running Kernel")
            batch.init_process("bvplBlockKernelOperatorProcess")
            batch.set_input_from_db(0, job.scene)
            batch.set_input_from_db(1, kernel)
            batch.set_input_int(2, job.block_i)
            batch.set_input_int(3, job.block_j)
            batch.set_input_int(4, job.block_k)
            batch.set_input_string(5, "algebraic")
            batch.set_input_string(6, job.output_path)
            batch.run_process()

            print ("Runing time for worker:", self.name)
            print(time.time() - start_time)
