import os
from multiprocessing import Queue
from taylor_kernel_threads import taylor_kernel_job, execute_jobs
import time
import optparse
import random

import brl_init
import bvpl_octree_batch as batch
dbvalue = brl_init.register_batch(batch)

if __name__ == "__main__":
    parser = optparse.OptionParser(description='Run Taylor Kernels')

    parser.add_option('--model_dir', action="store", dest="model_dir")
    parser.add_option('--taylor_dir', action="store", dest="taylor_dir")
    parser.add_option('--kernel_path', action="store", dest="kernel_path")
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
    kernel_path = options.kernel_path
    nblocks_x = options.nblocks_x
    nblocks_y = options.nblocks_y
    nblocks_z = options.nblocks_z
    num_cores = options.num_cores

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Model Dir"
        sys.exit(-1)

    if not os.path.isdir(model_dir + "/"):
        print "Invalid Kernel Path"
        sys.exit(-1)

    if not os.path.isdir(taylor_dir + "/"):
        os.mkdir(taylor_dir + "/")

    # Kernel names for 2-degree approximation
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

    print("Creating a Scene")
    batch.init_process("boxmCreateSceneProcess")
    batch.set_input_string(0,  model_dir + "/mean_color_scene.xml")
    batch.run_process()
    (scene_id, scene_type) = batch.commit_output(0)
    scene = dbvalue(scene_id, scene_type)

    # Begin multiprocessing
    t1 = time.time()
    work_queue = Queue()
    job_list = []

    blocks_x = [i for i in range(0, nblocks_x)]
    blocks_y = [i for i in range(0, nblocks_y)]
    blocks_z = [i for i in range(0, nblocks_z)]

    random.shuffle(blocks_x)
    random.shuffle(blocks_y)
    random.shuffle(blocks_y)

    # Enqueue jobs
    for curr_kernel in range(0, len(kernel_list)):
        for i in range(0, len(blocks_x)):
            for j in range(0, len(blocks_y)):
                for k in range(0, len(blocks_z)):
                    block_i = blocks_x[i]
                    block_j = blocks_y[j]
                    block_k = blocks_z[k]
                    curr_kernel_path = kernel_path + "/" + \
                        kernel_list[curr_kernel] + ".txt"
                    output_path = taylor_dir + "/" + kernel_list[curr_kernel]
                    if not os.path.isdir(output_path + "/"):
                        os.mkdir(output_path + "/")
                    current_job = taylor_kernel_job(
                        scene, curr_kernel_path, block_i, block_j, block_k, output_path)
                    job_list.append(current_job)

    execute_jobs(job_list, num_cores)
