#!/bin/bash
"""
Created on Mon Feb 14, 2011

@author:Isabel Restrepo

A script that encapsulates all steps needed to computer pca training error as a the number of training samples varies
"""

if [ $1 = "-r" ] ; then
  export PYTHONPATH=/projects/vxl/bin/release/lib
else
  if [ $1 = "-d" ] ; then
     export PYTHONPATH=/projects/vxl/bin/Debug/lib
  else
    echo "Need a flag, either -d or -r";
  fi
fi

echo "PYTHONPATH=" $PYTHONPATH

#model_dir="/Users/isa/Experiments/CapitolBOXMSmall";
#pca_dir="/Users/isa/Experiments/PCA/CapitolBOXMSmall";

model_dir="/homelocal/isa/Experiments/CapitolBOXM_6_4_4";
pca_dir="/homelocal/isa/Experiments/PCA/CapitolBOXM_6_4_4";


#model_dir="/Users/isa/Experiments/DowntownBOXM_3_3_1";
#pca_dir="/Users/isa/Experiments/PCA/DowntownBOXM_3_3_1";


##Compute Expected Color Scene
#python /Projects/voxels-at-lems/scripts/boxm/compute_expected_color_scene.py --model_dir $model_dir --model_name $model_name

#Compute reconstruction at each voxel in the scene
for ((dim = 2; dim <11; dim++))
do
  #Compute PCA basis
  python extract_pca_kernels.py --model_dir $model_dir --pca_dir $pca_dir --train_fraction $dim
done

