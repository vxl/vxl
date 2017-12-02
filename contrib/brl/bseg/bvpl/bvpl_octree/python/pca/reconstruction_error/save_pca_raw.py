import bvpl_octree_batch
import multiprocessing
import Queue
import time
import os
import optparse


class dbvalue:

    def __init__(self, index, type):
        self.id = index    # unsigned integer
        self.type = type   # string


class save_scene_job():

    def __init__(self, input_scene_path, output_scene_path):
        self.input_scene_path = input_scene_path
        self.output_scene_path = output_scene_path


def execute_jobs(jobs, num_procs=5):
    work_queue = multiprocessing.Queue()
    result_queue = multiprocessing.Queue()
    for job in jobs:
        work_queue.put(job)

    for i in range(num_procs):
        worker = save_scene_worker(work_queue, result_queue)
        worker.start()
        print("worker with name ", worker.name, " started!")


class save_scene_worker(multiprocessing.Process):

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

            print("Creating a Scene")
            bvpl_octree_batch.init_process("boxmCreateSceneProcess")
            bvpl_octree_batch.set_input_string(0, job.input_scene_path)
            bvpl_octree_batch.run_process()
            (scene_id, scene_type) = bvpl_octree_batch.commit_output(0)
            scene = dbvalue(scene_id, scene_type)

            print("Save Scene")
            bvpl_octree_batch.init_process("boxmSaveSceneRawProcess")
            bvpl_octree_batch.set_input_from_db(0, scene)
            bvpl_octree_batch.set_input_string(1, job.output_scene_path)
            bvpl_octree_batch.set_input_unsigned(2, 0)
            bvpl_octree_batch.set_input_unsigned(3, 1)
            bvpl_octree_batch.run_process()

            print ("Runing time for worker:", self.name)
            print(time.time() - start_time)


#*********************The Main Algorithm ****************************#
if __name__ == "__main__":

    bvpl_octree_batch.register_processes()
    bvpl_octree_batch.register_datatypes()

    parser = optparse.OptionParser(
        description='Save taylor responses to raw file')

    parser.add_option('--pca_dir', action="store", dest="pca_dir")
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)

    options, args = parser.parse_args()

    pca_dir = options.pca_dir
    num_cores = options.num_cores

    if not os.path.isdir(pca_dir + "/"):
        print "Invalid Taylor Dir"
        sys.exit(-1)

    # Begin multiprocessing
    t1 = time.time()
    work_queue = multiprocessing.Queue()
    job_list = []

    # Enqueue jobs
    pca_feature_dim = 125
    for dim in range(0, pca_feature_dim):

        scene_in_path = pca_dir + "/error_" + str(dim) + "/error_scene.xml"
        scene_out_path = pca_dir + "/error_" + str(dim) + "/error_scene"

        current_job = save_scene_job(scene_in_path, scene_out_path)

        job_list.append(current_job)

    execute_jobs(job_list, num_cores)
