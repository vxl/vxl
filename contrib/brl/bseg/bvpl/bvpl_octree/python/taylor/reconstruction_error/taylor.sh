#!/bin/bash
"""
Created on Mon Feb 14, 2011

@author:Isabel Restrepo

A script that encapsulates all steps needed to computer taylor reconstruction error
"""

if [ $1 = "-r" ] ; then
  export PYTHONPATH=/Projects/vxl/bin/Release/lib
else
  if [ $1 = "-d" ] ; then
    export PYTHONPATH=/Projects/vxl/bin/Debug/lib
  else
    echo "Need a flag, either -d or -r";
  fi
fi

echo "PYTHONPATH=" $PYTHONPATH

#model_dir="/Users/isa/Experiments/CapitolBOXMSmall";
#taylor_dir="/Users/isa/Experiments/Taylor/CapitolBOXMSmall_test";
#kernel_path="/Projects/vxl/src/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5";
#nblocks_x=1;
#nblocks_y=1;
#nblocks_z=1;


#model_dir="/Users/isa/Experiments/CapitolBOXM_6_4_4";
#model_name="capitol_scene";
#taylor_dir="/Users/isa/Experiments/Taylor/CapitolBOXM_6_4_4";
#kernel_path="/Projects/vxl/src/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5";
#nblocks_x=6;
#nblocks_y=4;
#nblocks_z=4;


model_dir="/Users/isa/Experiments/DowntownBOXM_12_12_4";
model_name="downtown_scene";
taylor_dir="/Users/isa/Experiments/Taylor/DowntownBOXM_12_12_4";
kernel_path="/Projects/vxl/src/contrib/brl/bseg/bvpl/doc/taylor2_5_5_5";
nblocks_x=12;
nblocks_y=12;
nblocks_z=4;

fraction=1.0;
num_cores=10;

cd "/Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor"

# #Compute Expected Color Scene
#python /Projects/voxels-at-lems/scripts/boxm/compute_expected_color_scene.py --model_dir $model_dir --model_name $model_name
#
# #Compute Responses to Taylor Kernels
#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/run_taylor_kernels.py --model_dir $model_dir --taylor_dir $taylor_dir --kernel_path $kernel_path --num_cores $num_cores --nblocks_x $nblocks_x --nblocks_y $nblocks_y --nblocks_z $nblocks_z
#
# #Compute reconstruction at each voxel in the scene
#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/compute_taylor_error_scene.py --model_dir $model_dir --taylor_dir $taylor_dir --num_cores $num_cores --nblocks_x $nblocks_x --nblocks_y $nblocks_y --nblocks_z $nblocks_z
#
# #Add error of fraction of voxels
#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/add_taylor_error.py --model_dir $model_dir --taylor_dir $taylor_dir --num_cores $num_cores --nblocks_x $nblocks_x --nblocks_y $nblocks_y --nblocks_z $nblocks_z --fraction $fraction
#
# #Save all scenes to drishti raw file
python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/save_taylor_raw.py --taylor_dir $taylor_dir --num_cores $num_cores

#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/explore_histogram.py --taylor_dir $taylor_dir --num_cores $num_cores
