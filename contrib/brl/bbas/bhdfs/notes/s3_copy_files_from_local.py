# Recovering the correct offsets for rpc cameras
#
# s3_copy_files_from_local.py <local_folder> <name_ending> <hdfs_folder>
#
# all the files in <local_folder> as:
#   <local_folder>/*<name_ending> will be copied over to <hdfs_working_dir>/<hdfs_folder> on hdfs
# e.g. python s3_copy_files_from_local.py ./crops _normalized.png world_dir
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

file_list = glob.glob(sys.argv[1] + "/*" + sys.argv[2])

batch.init_process("bhdfsCreateDirProcess")
batch.set_input_string(0, sys.argv[3])
statuscode = batch.run_process()
if statuscode != 1:
    print "Unable to create folder: " + sys.argv[3] + " on HDFS, exiting!\n"
    sys.exit(0)

for fname in file_list:
    print fname
    batch.init_process("bhdfsCopyFileProcess")
    batch.set_input_string(0, fname)
    batch.set_input_string(1, sys.argv[3])
    batch.run_process()
