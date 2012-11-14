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
import boxm2_batch;
boxm2_batch.register_processes();
boxm2_batch.register_datatypes();
import glob;
import random;
import math;
import os;
import shutil;
import time;
import sys;
class dbvalue:
  def __init__(self, index, type):
    self.id = index    # unsigned integer
    self.type = type   # string

print "create FS";
boxm2_batch.init_process("bhdfsCreateFSManagerProcess");
boxm2_batch.set_input_string(0,"default");
boxm2_batch.set_input_int(1,0);
boxm2_batch.run_process();

boxm2_batch.init_process("bbasStringArrayProcess");
boxm2_batch.set_input_string(0, sys.argv[2]);
boxm2_batch.run_process();
boxm2_batch.set_input_string(0, sys.argv[3]);
boxm2_batch.run_process();
boxm2_batch.finish_process();
(arr_id,arr_type)=boxm2_batch.commit_output(0);
arr = dbvalue(arr_id, arr_type);

# generate an input file on the output hdfs folder, this file will be streamed to mapper processes
# a vector of strings are inputted which will be used to form globs to retrieve file names
# the output file will have <name1> <name2> ... <name n> names on each line depending on this vector's size
boxm2_batch.init_process("bhdfsGenerateStdInFileProcess");
boxm2_batch.set_input_string(0, sys.argv[1]);  # folder on hdfs to find those files with the given endings
boxm2_batch.set_input_from_db(1, arr);  # the array of file name endings
boxm2_batch.set_input_string(2, sys.argv[4]);  # the full path of on hdfs of the input stdin.txt file to save it
boxm2_batch.run_process();
