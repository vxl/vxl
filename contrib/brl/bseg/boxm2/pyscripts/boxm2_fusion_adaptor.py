import brl_init
import boxm2_batch as batch
dbvalue = brl_init.register_batch(batch)


def update_max_vis(device, scene, opencl_cache, cam, ni, nj, mask_ptr, tnear=1000000, tfar=0.00001):
    print("Updating Max Vis")
    batch.init_process("boxm2OclUpdateMaxVisScoreProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    if(mask_ptr is not None):
        batch.set_input_from_db(6, mask_ptr)
    batch.set_input_float(7, tnear)
    batch.set_input_float(8, tfar)
    return batch.run_process()


def fuse_based_visibility(device, sceneA, sceneB, opencl_cache):
    print("Updating Max Vis")
    batch.init_process("boxm2OclFuseBasedVisibilityProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, sceneA)
    batch.set_input_from_db(2, sceneB)
    batch.set_input_from_db(3, opencl_cache)
    return batch.run_process()


def update_view_normal_dot(device, scene, opencl_cache, cam, ni, nj, mask_ptr, tnear=1000000, tfar=0.00001):
    print("Updating View Normal Dot")
    batch.init_process("boxm2OclUpdateViewNormalDotProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    if(mask_ptr is not None):
        batch.set_input_from_db(6, mask_ptr)
    batch.set_input_float(7, tnear)
    batch.set_input_float(8, tfar)
    return batch.run_process()


def fuse_based_orientation(device, sceneA, sceneB, opencl_cache):
    print("Updating Dot product")
    batch.init_process("boxm2OclFuseBasedOrientationProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, sceneA)
    batch.set_input_from_db(2, sceneB)
    batch.set_input_from_db(3, opencl_cache)
    return batch.run_process()


def update_view_surface_density(device, scene, opencl_cache, cam, ni, nj, depth, std_depth, tnear=1000000, tfar=0.00001):
    print("Updating View Surface Density")
    batch.init_process("boxm2OclUpdateSurfaceDensityProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_from_db(3, cam)
    batch.set_input_unsigned(4, ni)
    batch.set_input_unsigned(5, nj)
    batch.set_input_from_db(6, depth)
    batch.set_input_from_db(7, std_depth)
    batch.set_input_float(8, tnear)
    batch.set_input_float(9, tfar)
    return batch.run_process()


def fuse_based_surface_density(device, sceneA, sceneB, opencl_cache):
    print("Fuse two models using Surface Density")
    batch.init_process("boxm2OclFuseSurfaceDensityProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, sceneA)
    batch.set_input_from_db(2, sceneB)
    batch.set_input_from_db(3, opencl_cache)
    return batch.run_process()


def compute_dispersion(device, scene, opencl_cache, filename, coordinate_type="Cartesian"):
    print("Fuse two models using Surface Density")
    batch.init_process("boxm2OclComputeExpectationViewDirectionProcess")
    batch.set_input_from_db(0, device)
    batch.set_input_from_db(1, scene)
    batch.set_input_from_db(2, opencl_cache)
    batch.set_input_string(3, filename)
    batch.set_input_string(4, coordinate_type)
    return batch.run_process()
