#!/usr/bin/env python

# Recovering the correct offsets for rpc cameras
#
# load two images, expected view of a scene from a given camera
# and the image corresponding to that camera, and register the image to the expected image
# print the recovered offsets
#
#  assumes a conf_mapper.txt file is submitted to the nodes along with the job
#   the first line of the configuration file contains the output folder relative to the current working dir on the cluster
#   the second line contains the exhaustive search radius parameter of the process
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


mytime = time.clock();

print "create FS";
boxm2_batch.init_process("bhdfsCreateFSManagerProcess");
boxm2_batch.set_input_string(0,"default");
boxm2_batch.set_input_int(1,0);
boxm2_batch.run_process();

# read the hadoop output folder from the configuration file
f=open("conf_mapper.txt", 'r')
dir_names=f.readlines();
f.close();
output_path = dir_names[0].strip();
print output_path;
exh_search_rad = int(dir_names[1].strip());
print exh_search_rad;

for line in sys.stdin:
  line = line.strip();
  words = line.split();
  print words[0]; 
  print words[1];
  
  boxm2_batch.init_process("bhdfsLoadImageViewProcess");
  boxm2_batch.set_input_string(0,words[0]);
  boxm2_batch.run_process();
  (imid, imtype) = boxm2_batch.commit_output(0);
  s_exp_img = dbvalue(imid, imtype);
  
  boxm2_batch.init_process("bhdfsLoadImageViewProcess");
  boxm2_batch.set_input_string(0,words[1]);
  boxm2_batch.run_process();
  (imid, imtype) = boxm2_batch.commit_output(0);
  norm_image = dbvalue(imid, imtype);
  
  # register the input image to the exp image, find the translation that needs to be added to img
  # so that mutual info is max with exp img of the world
  boxm2_batch.init_process("ihogRegisterTranslationalProcess");
  boxm2_batch.set_input_from_db(0,s_exp_img);      # from img
  boxm2_batch.set_input_from_db(1,norm_image);  # to img
  boxm2_batch.set_input_int(2, exh_search_rad);
  boxm2_batch.run_process();
  (id,type) = boxm2_batch.commit_output(0);
  tx = dbvalue(id,type);
  (id,type) = boxm2_batch.commit_output(1);
  ty = dbvalue(id,type);
  offset_x = boxm2_batch.get_output_double(tx.id);
  offset_y = boxm2_batch.get_output_double(ty.id);
  
  [head, tail] = os.path.split(words[0]);
  [iname, ext] = os.path.splitext(tail);
  
  off_fname = output_path + "/" + iname + "_offsets.txt";
  print off_fname;
  
  boxm2_batch.init_process("bhdfsGenerateFileProcess");
  boxm2_batch.set_input_string(0, off_fname);
  boxm2_batch.set_input_string(1, str(offset_x) + " " + str(offset_y) + "\n");
  boxm2_batch.run_process();
  
mytime = time.clock();
print "time passed: " + str(mytime) + " secs which is " + str(mytime/60) + " minutes.\n";


