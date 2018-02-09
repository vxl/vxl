import multiprocessing
import Queue
import time
import os
import optparse

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)


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
            batch.init_process("boxmCreateSceneProcess")
            batch.set_input_string(0, job.input_scene_path)
            batch.run_process()
            (scene_id, scene_type) = batch.commit_output(0)
            scene = dbvalue(scene_id, scene_type)

            print("Save Scene")
            batch.init_process("boxmSaveSceneRawProcess")
            batch.set_input_from_db(0, scene)
            batch.set_input_string(
                1, job.output_scene_path + "_float")
            batch.set_input_unsigned(2, 0)
            batch.set_input_unsigned(3, 1)
            batch.run_process()

#            resolution = 0;
#            enforce_level = 1;

#            print("Convert to regular grid");
#            batch.init_process("boxmSceneToBvxmGridProcess");
#            batch.set_input_from_db(0,scene);
#            batch.set_input_string(1, job.output_scene_path + ".vox");
#            batch.set_input_unsigned(2, resolution);
#            batch.set_input_bool(3, enforce_level);
#            batch.run_process();
#            (grid_id, grid_type) = batch.commit_output(0);
#            grid = dbvalue(grid_id, grid_type);
#
#            print("Save Grid");
#            batch.init_process("bvxmSaveGridRawProcess");
#            batch.set_input_from_db(0,grid);
#            batch.set_input_string(1,job.output_scene_path + ".raw");
#            batch.run_process();

            print ("Runing time for worker:", self.name)
            print(time.time() - start_time)


#*********************The Main Algorithm ****************************#
if __name__ == "__main__":

    batch.register_processes()
    batch.register_datatypes()

    parser = optparse.OptionParser(
        description='Save taylor responses to raw file')

    parser.add_option('--taylor_dir', action="store", dest="taylor_dir")
    parser.add_option('--num_cores', action="store",
                      dest="num_cores", type="int", default=4)

    options, args = parser.parse_args()

    taylor_dir = options.taylor_dir
    num_cores = options.num_cores

    if not os.path.isdir(taylor_dir + "/"):
        print "Invalid Taylor Dir"
        sys.exit(-1)

    kernel_list = []

    kernel_list.append("I0")
    kernel_list.append("Ix")
    kernel_list.append("Iy")
    kernel_list.append("Iz")
    kernel_list.append("Ixx")
    kernel_list.append("Iyy")
    kernel_list.append("Izz")
    kernel_list.append("Ixy")
    kernel_list.append("Ixz")
    kernel_list.append("Iyz")

    # Begin multiprocessing
    t1 = time.time()
    work_queue = multiprocessing.Queue()
    job_list = []

    # Enqueue jobs
#  for i in range(0, len(kernel_list)):
#
#      scene_in_path = taylor_dir + "/" + kernel_list[i] + "/float_response_scene.xml";
#      scene_out_path = taylor_dir + "/" + kernel_list[i]+ "/float_response_scene";
#
#      current_job = save_scene_job(scene_in_path, scene_out_path);
#
#      job_list.append(current_job);

    # append error scene
    scene_in_path = taylor_dir + "/error_scene.xml"
    scene_out_path = taylor_dir + "/error_scene"
    current_job = save_scene_job(scene_in_path, scene_out_path)
    job_list.append(current_job)

    execute_jobs(job_list, num_cores)
