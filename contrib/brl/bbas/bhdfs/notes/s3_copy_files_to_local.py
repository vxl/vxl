# Recovering the correct offsets for rpc cameras
#
# s3_copy_files_to_local.py <hdfs_folder> <name_ending> <local_folder>
#
# all the files in <hdfs_folder> as:
#   <hdfs_working_dir>/<hdfs_folder>/*<name_ending> will be copied over to <local_folder>
# e.g. python s3_copy_files_to_local.py world_dir _normalized.png ./crops
#  <hdfs_working_dir> will be retrieved from <hdfs> manager
#
import glob
import random
import math
import os
import shutil
import time
import sys

import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)

print "create FS"
batch.init_process("bhdfsCreateFSManagerProcess")
batch.set_input_string(0, "default")
batch.set_input_int(1, 0)
batch.run_process()

batch.init_process("bhdfsCopyFilesToLocalProcess")
batch.set_input_string(0, sys.argv[1])  # hdfs path
batch.set_input_string(1, sys.argv[2])  # name ending
batch.set_input_string(2, sys.argv[3])  # local path
statuscode = batch.run_process()
if statuscode != 1:
    print "Unable to copy files to local folder: " + sys.argv[3] + "!\n"
    sys.exit(0)
