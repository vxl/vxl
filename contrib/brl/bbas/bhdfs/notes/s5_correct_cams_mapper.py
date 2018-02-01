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

mytime = time.clock()

print "create FS"
batch.init_process("bhdfsCreateFSManagerProcess")
batch.set_input_string(0, "default")
batch.set_input_int(1, 0)
batch.run_process()

# read the hadoop output folder from the configuration file
f = open("conf_mapper.txt", 'r')
dir_names = f.readlines()
f.close()
output_path = dir_names[0].strip()
print output_path
exh_search_rad = int(dir_names[1].strip())
print exh_search_rad

for line in sys.stdin:
    line = line.strip()
    words = line.split()
    print words[0]
    print words[1]

    batch.init_process("bhdfsLoadImageViewProcess")
    batch.set_input_string(0, words[0])
    batch.run_process()
    (imid, imtype) = batch.commit_output(0)
    s_exp_img = dbvalue(imid, imtype)

    batch.init_process("bhdfsLoadImageViewProcess")
    batch.set_input_string(0, words[1])
    batch.run_process()
    (imid, imtype) = batch.commit_output(0)
    norm_image = dbvalue(imid, imtype)

    # register the input image to the exp image, find the translation that needs to be added to img
    # so that mutual info is max with exp img of the world
    batch.init_process("ihogRegisterTranslationalProcess")
    batch.set_input_from_db(0, s_exp_img)      # from img
    batch.set_input_from_db(1, norm_image)  # to img
    batch.set_input_int(2, exh_search_rad)
    batch.run_process()
    (id, type) = batch.commit_output(0)
    tx = dbvalue(id, type)
    (id, type) = batch.commit_output(1)
    ty = dbvalue(id, type)
    offset_x = batch.get_output_double(tx.id)
    offset_y = batch.get_output_double(ty.id)

    [head, tail] = os.path.split(words[0])
    [iname, ext] = os.path.splitext(tail)

    off_fname = output_path + "/" + iname + "_offsets.txt"
    print off_fname

    batch.init_process("bhdfsGenerateFileProcess")
    batch.set_input_string(0, off_fname)
    batch.set_input_string(1, str(offset_x) + " " + str(offset_y) + "\n")
    batch.run_process()

mytime = time.clock()
print "time passed: " + str(mytime) + " secs which is " + str(mytime / 60) + " minutes.\n"
