# Recovering the correct offsets for rpc cameras
#
# s4_prepare_stdin_txt.py <local_folder1> <name_ending1> <local folder2> <name_ending2> <hdfs file name> <hdfs_output_folder>
#
# prepare a file list with all the files in <local_folder1> as:
#   <local_folder1>/*<name_ending1>
#  a list such as
#   <hdfs_working_dir>/<hdfs_output_folder>/<file name1> <hdfs_working_dir>/<hdfs_output_folder>/<file name2>
#  and copy the file to <hdfs_working_dir>/<hdfs file name> ;  <hdfs_working_dir> will be retrieved from <hdfs> manager
# e.g. python ./crops _normalized.png ./crops ./crops/exp_imgs _exp.png some_hdfs_folder/input_stdin.txt hadoop_output_folder
#
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

batch.init_process("bbasStringArrayProcess")
batch.set_input_string(0, sys.argv[2])
batch.run_process()
batch.set_input_string(0, sys.argv[3])
batch.run_process()
batch.finish_process()
(arr_id, arr_type) = batch.commit_output(0)
arr = dbvalue(arr_id, arr_type)

# generate an input file on the output hdfs folder, this file will be streamed to mapper processes
# a vector of strings are inputted which will be used to form globs to retrieve file names
# the output file will have <name1> <name2> ... <name n> names on each
# line depending on this vector's size
batch.init_process("bhdfsGenerateStdInFileProcess")
# folder on hdfs to find those files with the given endings
batch.set_input_string(0, sys.argv[1])
batch.set_input_from_db(1, arr)  # the array of file name endings
# the full path of on hdfs of the input stdin.txt file to save it
batch.set_input_string(2, sys.argv[4])
batch.run_process()
