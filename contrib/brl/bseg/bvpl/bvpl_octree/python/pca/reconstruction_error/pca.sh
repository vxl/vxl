#!/bin/bash
"""
Created on Mon Feb 14, 2011

@author:Isabel Restrepo

A script that encapsulates all steps needed to computer pca reconstruction error as a function of number of components used
"""

if [ $1 = "-r" ] ; then
  export PYTHONPATH=/Projects/vxl/bin/release/lib
else
  if [ $1 = "-d" ] ; then
    export PYTHONPATH=/Projects/vxl/bin/Debug/lib
  else
    echo "Need a flag, either -d or -r";
  fi
fi

echo "PYTHONPATH=" $PYTHONPATH

#model_dir="/Users/isa/Experiments/CapitolBOXMSmall";
#pca_dir="/Users/isa/Experiments/PCA/CapitolBOXMSmall/10";
#nblocks_x=1;
#nblocks_y=1;
#nblocks_z=1;


#model_dir="/Users/isa/Experiments/CapitolBOXM_6_4_4";
#model_name="capitol_scene";
#pca_dir="/Users/isa/Experiments/PCA/CapitolBOXM_6_4_4/10";
#nblocks_x=6;
#nblocks_y=4;
#nblocks_z=4;


model_dir="/Users/isa/Experiments/DowntownBOXM_3_3_1";
model_name="downtown_scene";
pca_dir="/Users/isa/Experiments/PCA/DowntownBOXM_3_3_1/10";
nblocks_x=3;
nblocks_y=3;
nblocks_z=1;

train_fraction=0.1;
test_fraction=1.0;
num_cores=5;


##************Compute Expected Color Scene
#python /Projects/voxels-at-lems/scripts/boxm/compute_expected_color_scene.py --model_dir $model_dir --model_name $model_name

#Compute PCA basis
#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/PCA/release/reconstruction_error/extract_pca_kernels.py --model_dir $model_dir --pca_dir $pca_dir --train_fraction $train_fraction

#Compute reconstruction at each voxel in the scene
for ((dim = 45; dim <125; dim++))
do
  python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/PCA/release/reconstruction_error/compute_pca_error_scene.py --model_dir $model_dir --pca_dir $pca_dir --num_cores $num_cores --nblocks_x $nblocks_x --nblocks_y $nblocks_y --nblocks_z $nblocks_z --dimension $dim

  #Add error of fraction of voxels

  python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/PCA/release/reconstruction_error/add_pca_error.py --model_dir $model_dir --pca_dir $pca_dir --num_cores $num_cores --nblocks_x $nblocks_x --nblocks_y $nblocks_y --nblocks_z $nblocks_z --fraction $test_fraction --dimension $dim
done

# #Save all scenes to drishti raw file
#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/PCA/release/reconstruction_error/save_pca_raw.py --pca_dir $pca_dir --num_cores $num_cores

#python /Projects/voxels-at-lems/scripts/bvpl/bvpl_octree/taylor/release/reconstruction_error/explore_histogram.py --pca_dir $pca_dir --num_cores $num_cores
