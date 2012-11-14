# Recovering the correct offsets for rpc cameras
#
# s3_copy_files_from_local.py <local_folder> <name_ending> <hdfs_folder>
#
# all the files in <local_folder> as:
#   <local_folder>/*<name_ending> will be copied over to <hdfs_working_dir>/<hdfs_folder> on hdfs
# e.g. python s3_copy_files_from_local.py ./crops _normalized.png world_dir
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

file_list = glob.glob(sys.argv[1] + "/*" + sys.argv[2]);

boxm2_batch.init_process("bhdfsCreateDirProcess");
boxm2_batch.set_input_string(0, sys.argv[3]);
statuscode = boxm2_batch.run_process();
if statuscode != 1:
  print "Unable to create folder: " + sys.argv[3] + " on HDFS, exiting!\n";
  sys.exit(0);

for fname in file_list:
    print fname
    boxm2_batch.init_process("bhdfsCopyFileProcess");
    boxm2_batch.set_input_string(0,fname);
    boxm2_batch.set_input_string(1,sys.argv[3]);
    boxm2_batch.run_process();

