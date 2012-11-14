# Recovering the correct offsets for rpc cameras
#
# s3_copy_files_to_local.py <hdfs_folder> <name_ending> <local_folder>
#
# all the files in <hdfs_folder> as:
#   <hdfs_working_dir>/<hdfs_folder>/*<name_ending> will be copied over to <local_folder>
# e.g. python s3_copy_files_to_local.py world_dir _normalized.png ./crops
#  <hdfs_working_dir> will be retrieved from <hdfs> manager
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

boxm2_batch.init_process("bhdfsCopyFilesToLocalProcess");
boxm2_batch.set_input_string(0, sys.argv[1]);  # hdfs path
boxm2_batch.set_input_string(1, sys.argv[2]);  # name ending
boxm2_batch.set_input_string(2, sys.argv[3]);  # local path
statuscode = boxm2_batch.run_process();
if statuscode != 1:
  print "Unable to copy files to local folder: " + sys.argv[3] + "!\n";
  sys.exit(0);

