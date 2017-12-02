"""
Created on April 17, 2012

@author:Isabel Restrepo

Functions to run filters of a boxm2_scene
"""

from boxm2_register import boxm2_batch, dbvalue
import os

#********************************************************************
#  Create a vector of filters identified by the factory name
#  and the direction specified
#********************************************************************


def create_kernel_vector(factory_name, dir_type, dim_x, dim_y, dim_z, supp_x, supp_y, supp_z):
    print("Creating Filtering Kernels")
    boxm2_batch.init_process("bvpl_create_generic_kernel_vector_process")
    boxm2_batch.set_input_float(0, dim_x)
    boxm2_batch.set_input_float(1, dim_y)
    boxm2_batch.set_input_float(2, dim_z)
    boxm2_batch.set_input_float(3, supp_x)
    boxm2_batch.set_input_float(4, supp_y)
    boxm2_batch.set_input_float(5, supp_z)
    boxm2_batch.set_input_string(6, factory_name)
    boxm2_batch.set_input_string(7, dir_type)
    boxm2_batch.run_process()
    (id, type) = boxm2_batch.commit_output(0)
    filters = dbvalue(id, type)

    return filters


def write_kernel_vector(filters, output_prefix):
    print("Creating Filtering Kernels")
    boxm2_batch.init_process("bvpl_write_generic_kernel_vector_process")
    boxm2_batch.set_input_from_db(0, filters)
    boxm2_batch.set_input_string(1, output_prefix)
    boxm2_batch.run_process()

#********************************************************************
#  Apply a vector of filters to a boxm2_scene
#********************************************************************


def apply_filters(scene, cache, device, filters):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Filtering Scene")
        boxm2_batch.init_process("boxm2_ocl_kernel_vector_filter_process")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_from_db(3, filters)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False

#********************************************************************
#  Apply a vector of filters to a boxm2_scene -- CPP version -- inputs kernels as txt files and creates *.bin files
#       read the kernel from filter_basename + str(filter_id) + ".txt"
#********************************************************************


def apply_filters_cpp(scene, cpp_cache, prob_thres, filter_basename, filter_id, octree_lvl):
    boxm2_batch.init_process("boxm2CppFilterResponseProcess")
    boxm2_batch.set_input_from_db(0, scene)
    boxm2_batch.set_input_from_db(1, cpp_cache)
    boxm2_batch.set_input_float(2, prob_thres)
    boxm2_batch.set_input_string(3, filter_basename)
    # id kernel --> read the kernel from filter_basename + str(filter_id) +
    # ".txt"
    boxm2_batch.set_input_unsigned(4, filter_id)
    boxm2_batch.set_input_unsigned(5, octree_lvl)  # octree level to run kernel
    boxm2_batch.run_process()

#********************************************************************
#  Apply a vector of filters to a boxm2_scene
#********************************************************************


def interpolate_normals(scene, cache, device, filters):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Interpolating Normal")
        boxm2_batch.init_process(
            "boxm2_ocl_aggregate_normal_from_filter_vector_process")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_from_db(3, filters)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False

#********************************************************************
#  Flip normals towards direction of maximum visibility
#********************************************************************


def flip_normals(scene, cache, device, use_sum=False):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Flipping Normal")
        boxm2_batch.init_process("boxm2OclFlipNormalsUsingVisProcess")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_bool(3, use_sum)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def make_inside_empty(scene, cache, device, use_sum=False):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Flipping Normal")
        boxm2_batch.init_process("boxm2OclMakeInsideVoxelsEmptyProcess")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_bool(3, use_sum)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def update_parents_alpha(scene, cache, device):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Update Parents Alpha")
        boxm2_batch.init_process("boxm2OclUpdateParentsAlphaProcess")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def create_coarse_model(scene, cache, device, model_dir):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Create a Coarser Scene")
        boxm2_batch.init_process("boxm2OclCreateCoarserSceneProcess")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_string(3, model_dir)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def filter_scene_data(scene, cache, device, filters, filter_idx):
    if cache.type == "boxm2_opencl_cache_sptr":
        print("Filtering Scene Data")
        boxm2_batch.init_process("boxm2_ocl_filter_scene_data_process")
        boxm2_batch.set_input_from_db(0, device)
        boxm2_batch.set_input_from_db(1, scene)
        boxm2_batch.set_input_from_db(2, cache)
        boxm2_batch.set_input_from_db(3, filters)
        boxm2_batch.set_input_int(4, filter_idx)
        return boxm2_batch.run_process()
    else:
        print "ERROR: Cache type not recognized: ", cache.type
        return False


def aggregate_normal_from_filter_vector(scene, cache, device, filter_vector):
    """ compute normals based on response of filter vector """
    boxm2_batch.init_process(
        "boxm2_ocl_aggregate_normal_from_filter_vector_process")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, cache)
    boxm2_batch.set_input_from_db(3, filter_vector)
    return boxm2_batch.run_process()
