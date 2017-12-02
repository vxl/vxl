from boxm2_scene_adaptor import *


def update_max_vis(device, scene, opencl_cache, cam, ni, nj, mask_ptr, tnear=1000000, tfar=0.00001):
    print("Updating Max Vis")
    boxm2_batch.init_process("boxm2OclUpdateMaxVisScoreProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, opencl_cache)
    boxm2_batch.set_input_from_db(3, cam)
    boxm2_batch.set_input_unsigned(4, ni)
    boxm2_batch.set_input_unsigned(5, nj)
    if(mask_ptr is not None):
        boxm2_batch.set_input_from_db(6, mask_ptr)
    boxm2_batch.set_input_float(7, tnear)
    boxm2_batch.set_input_float(8, tfar)
    return boxm2_batch.run_process()


def fuse_based_visibility(device, sceneA, sceneB, opencl_cache):
    print("Updating Max Vis")
    boxm2_batch.init_process("boxm2OclFuseBasedVisibilityProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, sceneA)
    boxm2_batch.set_input_from_db(2, sceneB)
    boxm2_batch.set_input_from_db(3, opencl_cache)
    return boxm2_batch.run_process()


def update_view_normal_dot(device, scene, opencl_cache, cam, ni, nj, mask_ptr, tnear=1000000, tfar=0.00001):
    print("Updating View Normal Dot")
    boxm2_batch.init_process("boxm2OclUpdateViewNormalDotProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, opencl_cache)
    boxm2_batch.set_input_from_db(3, cam)
    boxm2_batch.set_input_unsigned(4, ni)
    boxm2_batch.set_input_unsigned(5, nj)
    if(mask_ptr is not None):
        boxm2_batch.set_input_from_db(6, mask_ptr)
    boxm2_batch.set_input_float(7, tnear)
    boxm2_batch.set_input_float(8, tfar)
    return boxm2_batch.run_process()


def fuse_based_orientation(device, sceneA, sceneB, opencl_cache):
    print("Updating Dot product")
    boxm2_batch.init_process("boxm2OclFuseBasedOrientationProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, sceneA)
    boxm2_batch.set_input_from_db(2, sceneB)
    boxm2_batch.set_input_from_db(3, opencl_cache)
    return boxm2_batch.run_process()


def update_view_surface_density(device, scene, opencl_cache, cam, ni, nj, depth, std_depth, tnear=1000000, tfar=0.00001):
    print("Updating View Surface Density")
    boxm2_batch.init_process("boxm2OclUpdateSurfaceDensityProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, opencl_cache)
    boxm2_batch.set_input_from_db(3, cam)
    boxm2_batch.set_input_unsigned(4, ni)
    boxm2_batch.set_input_unsigned(5, nj)
    boxm2_batch.set_input_from_db(6, depth)
    boxm2_batch.set_input_from_db(7, std_depth)
    boxm2_batch.set_input_float(8, tnear)
    boxm2_batch.set_input_float(9, tfar)
    return boxm2_batch.run_process()


def fuse_based_surface_density(device, sceneA, sceneB, opencl_cache):
    print("Fuse two models using Surface Density")
    boxm2_batch.init_process("boxm2OclFuseSurfaceDensityProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, sceneA)
    boxm2_batch.set_input_from_db(2, sceneB)
    boxm2_batch.set_input_from_db(3, opencl_cache)
    return boxm2_batch.run_process()


def compute_dispersion(device, scene, opencl_cache, filename, coordinate_type="Cartesian"):
    print("Fuse two models using Surface Density")
    boxm2_batch.init_process("boxm2OclComputeExpectationViewDirectionProcess")
    boxm2_batch.set_input_from_db(0, device)
    boxm2_batch.set_input_from_db(1, scene)
    boxm2_batch.set_input_from_db(2, opencl_cache)
    boxm2_batch.set_input_string(3, filename)
    boxm2_batch.set_input_string(4, coordinate_type)
    return boxm2_batch.run_process()
